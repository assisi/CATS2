#!/usr/bin/env python3

import zmq
import time
import threading
import numpy as np
#from subprocess import Popen
import os
import sys
#import shutil

import sklearn.mixture
import scipy.stats


_referenceClockwiseFrequencies = np.array([0.54221289054743016, 0.52835117979620538, 0.52208400620158846, 0.57521062264859135, 0.50908453160904876, 0.50638694999787903, 0.49528456227357132])
_modulatedClockwiseFrequencies = np.array([0.50204807811659569, 0.54202494383763267, 0.60291051956382291, 0.77506372940684087, 0.59165256627185558, 0.58573393753034475, 0.63525429066288774, 0.49027599860253873])


def _print(param):
    print(param)
    sys.stdout.flush()


def _likelihood_ratio(l1, l2):
    if l1 > l2:
        return(2*(l1-l2))
    else:
        return(2*(l2-l1))


class InterspeciesManager(object):
    """ Interspecies manager, to bridge the Interspecies interface to CATS instances """

    def __init__(self, cats_interfaces, interspecies_interface_subscriber_addr, interspecies_interface_publisher_addr, publishing_period = 30.0):
        self.cats_interfaces = cats_interfaces
        self.interspecies_interface_subscriber_addr = interspecies_interface_subscriber_addr
        self.interspecies_interface_publisher_addr = interspecies_interface_publisher_addr
        self.publishing_period = publishing_period
        #self._managing_thread = threading.Thread(target = self._manage_interspecies)
        #self._current_behaviour = Behaviour()
        #self._current_behaviour_lock = threading.Lock()
        self._raw_history = {}
        self._history = {}
        self._history_lock = threading.Lock()
        self._last_history_index = None

        # Reset the behaviour to SI in all cats instances
        for instance in self.cats_interfaces.values():
            instance.set_robot_behaviour("SI")

        self._initGMM()

        # Create and connect subscriber
        self.context = zmq.Context(2)
        self.subscriber = self.context.socket(zmq.SUB)
        #self.subscriber.setsockopt(zmq.RCVTIMEO, 1000)
        self.subscriber.setsockopt(zmq.SUBSCRIBE, b'')
        self.subscriber.connect(self.interspecies_interface_subscriber_addr)
        _print("Successfully connected Interspecies subscriber to address: '%s'" % self.interspecies_interface_subscriber_addr)

        # Create and connect publisher
        self.publisher = self.context.socket(zmq.PUB)
        self.publisher.bind(self.interspecies_interface_publisher_addr)
        _print("Successfully connected Interspecies publisher to address: '%s'" % self.interspecies_interface_publisher_addr)

        # Create and start threads
        self.incoming_thread = threading.Thread(target = self._receive_data)
        #self.outgoing_thread = threading.Thread(target = self._send_data)
        self._stop = False
        self.incoming_thread.start()
        #self.outgoing_thread.start()
        #self.initiate_probing_trial("setup-2", 0.50)

    def _initGMM(self):
        self._gmmRef = sklearn.mixture.GaussianMixture()
        self._rRef = self._gmmRef.fit(_referenceClockwiseFrequencies[:, np.newaxis])
        self._gmmMod = sklearn.mixture.GaussianMixture()
        self._rMod = self._gmmMod.fit(_modulatedClockwiseFrequencies[:, np.newaxis])

    def computeModulationScore(self, fishFrequency):
        scoreRef = self._rRef.score(fishFrequency)
        scoreMod = self._rMod.score(fishFrequency)
        lr = _likelihood_ratio(scoreRef, scoreMod)
        surprise = scoreMod > scoreRef
        p = scipy.stats.chi2.sf(lr, 1)
        return p, surprise

    def stop_all(self):
        """Stops all threads."""
        self._stop = True
        #while self.outgoing_thread.isAlive():
        #    time.sleep(0.1)
        #_print('Interspecies subscriber thread finished')
        while self.incoming_thread.isAlive():
            time.sleep(0.1)
        _print('Interspecies receiving thread finished')

#    def set_behaviour(self, behaviour):
#        self._current_behaviour_lock.acquire()
#        self._current_behaviour = behaviour
#        self._current_behaviour_lock.release()

#    def _manage_interspecies(self):
#        while not self._stop_threads:
#            try:
#                pass # TODO
#            except zmq.ZMQError as e:
#                time.sleep(0.1)
#                continue
#            for elapsed_time in range(int(self.period)):
#                if not self._stop_threads:
#                    time.sleep(1.0)

    def get_raw_history(self, index = None):
        if index:
            self._history_lock.acquire()
            result = self._raw_history[index]
            self._history_lock.release()
        else:
            self._history_lock.acquire()
            result = self._raw_history
            self._history_lock.release()
        return result

    def get_history(self, index = None):
        if index:
            self._history_lock.acquire()
            result = self._fish_history[index]
            self._history_lock.release()
        else:
            self._history_lock.acquire()
            result = self._fish_history
            self._history_lock.release()
        return result


    def _receive_data(self):
        """ Handle incoming data streams from the Interspecies interface """
        self._incoming_thread_starting_time = time.time()
        while not self._stop:
            try:
                [name, message_type, sender, data] = self.subscriber.recv_multipart(flags=zmq.NOBLOCK)
                self._incoming_thread_elapsed_time = int(time.time() - self._incoming_thread_starting_time)

                #if message_type.decode('ascii').lower() != "statistics" and message_type.decode('ascii').lower() != "robottargetpositionchanged":
                _print("#%d\tfrom:%s\tname:%s device:%s command:%s data:%s" % (self._incoming_thread_elapsed_time, self.interspecies_interface_subscriber_addr, name, message_type, sender, data))

                # Save packet in history
                self._history_lock.acquire()
                self._raw_history[self._incoming_thread_elapsed_time] = [name, message_type, sender, data]
                if message_type.decode('ascii').lower() == "proberq":
                    data_list = data.decode('ascii').split(';')
                    data_dict = {}
                    for entry in data_list:
                        if not len(entry):
                            continue
                        entry_split = entry.split(':')
                        data_dict[entry_split[0].lower()] = entry_split[1]
                    self._history[self._incoming_thread_elapsed_time] = data_dict
                    self._last_history_index = self._incoming_thread_elapsed_time
                self._history_lock.release()

                # Handle probe Requests
                if message_type.decode('ascii').lower() == "proberq":
                    setup_name = name.decode('ascii').lower()
                    #setup_id = int(setup_name.split('-')[1]) - 1
                    confidence = float(data_dict['confidence'])
                    #self.initiate_probing_trial(setup_id, confidence)
                    self.initiate_probing_trial(setup_name, confidence)
            except zmq.ZMQError as e:
                time.sleep(0.1)
                continue


    # TODO put in a separate ProbingRequest class
    #def initiate_probing_trial(self, setup_id, confidence):
    def initiate_probing_trial(self, setup_name, confidence):
        """ Initiate a probing trial using the setup_name CATS instance """
        assert (confidence >= 0. and confidence <= 1.)
        #if (setup_id < 0 and setup_id >= len(self.cats_interfaces)):
        #print("Warning: setup id '%s' does not exists. Using 'setup-1' instead" % (setup_id + 1))
        #setup_id = 0 # XXX

        # Verify if setup exists
        if not self.cats_interfaces.get(setup_name):
            _print("Warning: received a Probe request but setup '%s' does not exists ! Sending a FailedProbe message..." % (setup_name))
            # Send a response to the Interspecies interface
            response_name = bytes("FishManager", 'ascii')
            response_message_type = bytes("FailedProbe", 'ascii')
            response_sender = bytes(setup_name, 'ascii')
            response_data = bytes("", 'ascii')
            try:
                self.publisher.send_multipart([response_name, response_message_type, response_sender, response_data])
            except zmq.ZMQError as e:
                _print("DEBUG1:", e)
            return

        # Determine trial duration from the confidence level
        #trial_duration = 4 * 60 # XXX Force trials to last 4 minutes
        trial_duration = self.publishing_period # XXX Force trials to last 4 minutes

        _print("Initiating a new probing trial on '%s' for %i sec with a confidence of %f" % (setup_name, trial_duration, confidence))

        # Send new behaviour to the CATS instance
        #self.cats_interfaces[setup_id].set_robot_behaviour("CW")
        robot_behaviour = self.cats_interfaces[setup_name].switch_robot_behaviour()

        # Wait for the trial to be completed
        time.sleep(trial_duration)

        # Reset robot behaviour to Social Integration through the CATS instance
        self.cats_interfaces[setup_name].set_robot_behaviour("SI")

        # Compute a modulation score
        statistics = self.cats_interfaces[setup_name].get_last_history()
        #_print("DEBUG1: ", statistics)
        fishClockWiseFrequency = float(statistics['fishclockwisepercent'])
        #if robot_behaviour == "CW":
        #    modulation_score = abs(fishClockWiseFrequency - np.mean(_referenceClockwiseFrequencies)) # TODO more adequate scheme
        #elif robot_behaviour == "CCW":
        #    modulation_score = abs(1.0 - fishClockWiseFrequency - np.mean(1.0 - _referenceClockwiseFrequencies)) # TODO more adequate scheme
        #else:
        #    modulation_score = 0.0
        #if modulation_score > 0.10: # TODO more adequate scheme
        #    surprise = 1
        #else:
        #    surprise = 0
        if robot_behaviour == "CW":
            modulation_score, surprise = self.computeModulationScore(fishClockWiseFrequency)
        elif robot_behaviour == "CCW":
            modulation_score, surprise = self.computeModulationScore(1 - fishClockWiseFrequency)
        else:
            modulation_score, surprise = 0.0, 0.0


        _print("Trial completed with a modulation score of: %f and a surprise of: %i" % (modulation_score, surprise))

        # Send a response to the Interspecies interface
        response_name = bytes("FishManager", 'ascii')
        response_message_type = bytes("ProbeDone", 'ascii')
        #response_sender = bytes("setup-%i" % (setup_id + 1), 'ascii')
        #response_sender = bytes("setup-%i" % (setup_id + 2), 'ascii') # XXX 
        response_sender = bytes(setup_name, 'ascii')
        response_data = bytes("Score:%f;Modulated:%i;Duration:%i;" % (modulation_score, surprise, trial_duration), 'ascii')
        try:
            self.publisher.send_multipart([response_name, response_message_type, response_sender, response_data])
        except zmq.ZMQError as e:
            _print("DEBUG1:", e)

        _print("Successfully sent probe response: to:%s\tname:%s device:%s command:%s data:%s" % (self.interspecies_interface_publisher_addr, response_name, response_message_type, response_sender, response_data))




class CatsIntersetupInterface:
    """ Interface to CATS intersetup system through ZMQ sockets """

    def __init__(self, instance_name, subscriber_addr = 'tcp://fishtrack:5555', publisher_addr = 'tcp://fishtrack:5556', publishing_period = 1.0, robot_behaviour = "CW"):
        self.instance_name = instance_name
        self.subscriber_addr = subscriber_addr
        self.publisher_addr = publisher_addr
        self.publishing_period = publishing_period
        self._robot_behaviour = robot_behaviour
        self._last_directional_robot_behaviour = robot_behaviour
        self._robot_behaviour_lock = threading.Lock()
        self._raw_history = {}
        self._history = {}
        self._history_lock = threading.Lock()
        self._last_history_index = None

        # Create and connect subscriber
        self.context = zmq.Context(2)
        self.subscriber = self.context.socket(zmq.SUB)
        #self.subscriber.setsockopt(zmq.RCVTIMEO, 1000)
        self.subscriber.setsockopt(zmq.SUBSCRIBE, b'')
        self.subscriber.connect(self.subscriber_addr)
        _print("Successfully connected CATS instance subscriber to address: '%s'" % self.subscriber_addr)

        # Create and connect publisher
        self.publisher = self.context.socket(zmq.PUB)
        self.publisher.bind(self.publisher_addr)
        _print("Successfully connected CATS instance publisher to address: '%s'" % self.publisher_addr)

        # Create and start threads
        self.incoming_thread = threading.Thread(target = self._receive_data)
        self.outgoing_thread = threading.Thread(target = self._send_data)
        self._stop = False
        self.incoming_thread.start()
        self.outgoing_thread.start()

    def get_raw_history(self, index = None):
        if index:
            self._history_lock.acquire()
            result = self._raw_history[index]
            self._history_lock.release()
        else:
            self._history_lock.acquire()
            result = self._raw_history
            self._history_lock.release()
        return result

    def get_history(self, index = None):
        if index:
            self._history_lock.acquire()
            result = self._fish_history[index]
            self._history_lock.release()
        else:
            self._history_lock.acquire()
            result = self._fish_history
            self._history_lock.release()
        return result

    def get_last_history(self):
        self._history_lock.acquire()
        result = self._history[self._last_history_index]
        self._history_lock.release()
        return result


    def set_robot_behaviour(self, robot_behaviour):
        self._robot_behaviour_lock.acquire()
        self._robot_behaviour = robot_behaviour
        if robot_behaviour == "CW" or robot_behaviour == "CCW":
            self._last_directional_robot_behaviour = robot_behaviour
        self._robot_behaviour_lock.release()

    def get_robot_behaviour(self):
        self._robot_behaviour_lock.acquire()
        robot_behaviour = self._robot_behaviour
        self._robot_behaviour_lock.release()
        return robot_behaviour


    def switch_robot_behaviour(self):
        self._robot_behaviour_lock.acquire()
        last_behaviour = self._last_directional_robot_behaviour
        self._robot_behaviour_lock.release()
        if last_behaviour == "CW":
            self.set_robot_behaviour("CCW")
        elif last_behaviour == "CCW":
            self.set_robot_behaviour("CW")
        return self.get_robot_behaviour()



    def _receive_data(self):
        """ Handle incoming data streams from a CATS instance """
        self._incoming_thread_starting_time = time.time()
        while not self._stop:
            try:
                [name, message_type, sender, data] = self.subscriber.recv_multipart(flags=zmq.NOBLOCK)
                self._incoming_thread_elapsed_time = int(time.time() - self._incoming_thread_starting_time)

                if message_type.decode('ascii').lower() != "statistics" and message_type.decode('ascii').lower() != "robottargetpositionchanged":
                    _print("#%d\tfrom:%s\tname:%s device:%s command:%s data:%s" % (self._incoming_thread_elapsed_time, self.subscriber_addr, name, message_type, sender, data))

                # Save packet in history
                self._history_lock.acquire()
                self._raw_history[self._incoming_thread_elapsed_time] = [name, message_type, sender, data]
                if message_type.decode('ascii').lower() == "statistics":
                    data_list = data.decode('ascii').split(';')
                    data_dict = {}
                    for entry in data_list:
                        entry_split = entry.split(':')
                        if len(entry_split) > 1:
                            data_dict[entry_split[0].lower()] = entry_split[1]
                    self._history[self._incoming_thread_elapsed_time] = data_dict
                    self._last_history_index = self._incoming_thread_elapsed_time
                self._history_lock.release()

                #self._history_lock.acquire()
                #self._raw_history[self._incoming_thread_elapsed_time] = [name, message_type, sender, data]
                #if name == b'casu-001':
                #    robot_index = 0 # XXX Only two robots
                #elif name = b'casu-002':
                #    robot_index = 1 # XXX Only two robots
                #data_pair = data.decode('ascii').split(',')
                #fish_current_behaviour_type = data_pair[0].split(' ')[1]
                #robot_current_behaviour_type = data_pair[1].split(' ')[1]
                #self._fish_history[self._incoming_thread_elapsed_time] = fish_current_behaviour_type
                #if self._robot_history.get(self._incoming_thread_elapsed_time):
                #    self._robot_history[self._incoming_thread_elapsed_time][robot_index] = robot_current_behaviour_type
                #else:
                #    behaviour_list = ['unk'] * 2 # XXX Only two robots
                #    behaviour_list[robot_index] = robot_current_behaviour_type
                #    self._robot_history[self._incoming_thread_elapsed_time][robot_index] = behaviour_list
                #self._history_lock.release()

                #if (sender == 'update'):
                #    #_print('Received statistics from cats: ' + name + ';' + message_type + ';' + sender + ';' + data)
                #    statistics = data.split(';')
                #    self.lock.acquire()
                #    for statistics_pair in statistics:
                #        id_value = statistics_pair.split(':')
                #        if len(id_value) == 2:
                #            self.value_by_path[id_value[0]] = id_value[1]
                #    if not self.initial_time_stamp:
                #        self.initial_time_stamp = float(self.value_by_path['timestamp'])
                #    t = (float(self.value_by_path['timestamp']) - self.initial_time_stamp) / 1000.
                #    self.value_by_path['time'] = t
                #    #_print('Received statistics from cats: ', self.value_by_path)
                #    self.hist_values_by_path.append(copy.deepcopy(self.value_by_path))
                #    self.lock.release()
                #elif sender == 'statistics':
                #    _print('Received statistics list from cats: ' + data)
                #    self.lock.acquire()
                #    self.available_statistics = data
                #    self.lock.release()
            except zmq.ZMQError as e:
                time.sleep(0.1)
                continue


    def _send_data(self):
        """ Forward data to a CATS instance """
        while not self._stop:
            try:
                self._robot_behaviour_lock.acquire()
                robot_behaviour = self._robot_behaviour
                self._robot_behaviour_lock.release()

                enableMsgPublishing = True
                name = bytes(self.instance_name, 'ascii')
                message_type = b'behaviour'
                sender = b'FishManager'
                if robot_behaviour == "CW":
                    data = b'CW'
                elif robot_behaviour == "CCW":
                    data = b'CCW'
                elif robot_behaviour == "SI":
                    data = b'Follow'
                else:
                    data = b''
                    enableMsgPublishing = False
                if enableMsgPublishing:
                    self.publisher.send_multipart([name, message_type, sender, data])
            except zmq.ZMQError as e:
                continue
            time.sleep(self.publishing_period)


    def stop_all(self):
        """Stops all threads."""
        self._stop = True
        while self.outgoing_thread.isAlive():
            time.sleep(0.1)
        _print('Intersetup CATS instance subscriber thread finished')
        while self.incoming_thread.isAlive():
            time.sleep(0.1)
        _print('Intersetup CATS instance receiving thread finished')



if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument('--intersetupSubscriberAddr1', type=str, default='tcp://fishtrack:5555',
                        help="address to listen for messages of CATS in setup-1.")
    parser.add_argument('--intersetupPublisherAddr1', type=str, default='tcp://fishtrack:5556',
                        help="address to send messages to CATS in setup-1.")
    parser.add_argument('--intersetupSubscriberAddr2', type=str, default='tcp://fishtrack2:5555',
                        help="address to listen for messages of CATS in setup-2.")
    parser.add_argument('--intersetupPublisherAddr2', type=str, default='tcp://fishtrack2:5556',
                        help="address to send messages to CATS in setup-2.")
    parser.add_argument('--interspeciesInterfaceSubscriberAddr', type=str, default='tcp://143.50.158.98:5555',
                        help="address to listen for messages of the Interspecies interface.")
    parser.add_argument('--interspeciesInterfacePublisherAddr', type=str, default='tcp://127.0.0.1:5556',
                        help="address to send messages to the Interspecies interface.")
    parser.add_argument('--trialDuration', type=int, default=30,
                        help="Duration of each trial")
#    parser.add_argument('--logFile', type=str, default="logs/log.log",
#                        help="Logging file")
    args = parser.parse_args()


    # TODO specify parameters through command line options
    # Connect to CATS instances
    cats_interface1 = CatsIntersetupInterface("setup-2", args.intersetupSubscriberAddr1, args.intersetupPublisherAddr1, 1.0, "CW")
    #cats_interface2 = CatsIntersetupInterface("setup-2", args.intersetupSubscriberAddr2, args.intersetupPublisherAddr2, 1.0, "CW")

    # Launch manager thread
    #manager = InterspeciesManager([cats_interface1, cats_interface2], args.interspeciesInterfaceSubscriberAddr, args.interspeciesInterfacePublisherAddr, 30.0)
    #manager = InterspeciesManager([cats_interface1], args.interspeciesInterfaceSubscriberAddr, args.interspeciesInterfacePublisherAddr, args.trialDuration)
    manager = InterspeciesManager({"setup-2": cats_interface1}, args.interspeciesInterfaceSubscriberAddr, args.interspeciesInterfacePublisherAddr, args.trialDuration)

    # Launch statistics threads
    # TODO


    cmd = 'a'
    while cmd != 'q':
        cmd = input('To stop the program press q<Enter>')

    manager.stop_all()
    cats_interface1.stop_all()
    #cats_interface2.stop_all()


# MODELINE	"{{{1
# vim:expandtab:softtabstop=4:shiftwidth=4:fileencoding=utf-8
# vim:foldmethod=marker

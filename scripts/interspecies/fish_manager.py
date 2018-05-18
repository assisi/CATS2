#!/usr/bin/env python3

import zmq
import time
import threading
import numpy as np
#from subprocess import Popen
#import os
#import shutil


_referenceClockwiseFrequencies = [0.54221289054743016, 0.52835117979620538, 0.52208400620158846, 0.57521062264859135, 0.50908453160904876, 0.50638694999787903, 0.49528456227357132]


## TODO
#class Behaviour(object):
#    """ TODO """
#    def __init__(self):
#        pass # TODO
#    def infer_next_behaviour(self, current_state):
#        pass # TODO
#
#
## TODO
#class ConstantBehaviour(Behaviour):
#    """ TODO """
#    def __init__(self, robot_behaviour_type):
#        super().__init__()
#        self.robot_behaviour_type = robot_behaviour_type
#    def infer_next_behaviour(self, current_state):
#        return self.robot_behaviour_type


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

        # Create and connect subscriber
        self.context = zmq.Context(2)
        self.subscriber = self.context.socket(zmq.SUB)
        #self.subscriber.setsockopt(zmq.RCVTIMEO, 1000)
        self.subscriber.setsockopt(zmq.SUBSCRIBE, b'')
        self.subscriber.connect(self.subscriber_addr)
        print("Successfully connected Interspecies subscriber to address: '%s'" % self.subscriber_addr)

        # Create and connect publisher
        self.publisher = self.context.socket(zmq.PUB)
        self.publisher.connect(self.publisher_addr)
        print("Successfully connected Interspecies publisher to address: '%s'" % self.publisher_addr)

        # Create and start threads
        self.incoming_thread = threading.Thread(target = self._receive_data)
        #self.outgoing_thread = threading.Thread(target = self._send_data)
        self._stop = False
        self.incoming_thread.start()
        #self.outgoing_thread.start()


    def stop_all(self):
        """Stops all threads."""
        self._stop = True
        while self.outgoing_thread.isAlive():
            time.sleep(0.1)
        print('Interspecies subscriber thread finished')
        while self.incoming_thread.isAlive():
            time.sleep(0.1)
        print('Interspecies receiving thread finished')

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
        while not self.stop:
            try:
                [name, message_type, sender, data] = self.subscriber.recv_multipart(flags=zmq.NOBLOCK)
                self._incoming_thread_elapsed_time = int(time.time() - self._incoming_thread_starting_time)
                print("#%d\tfrom:%s\tname:%s device:%s command:%s data:%s" % (self._incoming_thread_elapsed_time, self.subscriber_addr, name, message_type, sender, data))

                # Save packet in history
                self._history_lock.acquire()
                self._raw_history[self._incoming_thread_elapsed_time] = [name, message_type, sender, data]
                if message_type.decode('ascii').lower() == "proberq":
                    data_list = data.decode('ascii').split(';')
                    data_dict = {}
                    for entry in data_list:
                        entry_split = entry.split(':')
                        data_dict[entry_split[0].lower()] = entry_split[1]
                self._history[self._incoming_thread_elapsed_time] = data_dict
                self._last_history_index = self._incoming_thread_elapsed_time
                self._history_lock.release()

                # Handle probe Requests
                if message_type.decode('ascii').lower() == "proberq":
                    setup_name = name.decode('ascii').lower()
                    setup_id = int(setup_name.split('-')[1]) - 1
                    self.initiate_probing_trial(setup_id, confidence)
            except zmq.ZMQError as e:
                time.sleep(0.1)
                continue


    # TODO put in a separate ProbingRequest class
    def initiate_probing_trial(self, setup_id, confidence):
        """ Initiate a probing trial using the setup_id CATS instance """
        assert (setup_id >= 0 and setup_id < len(self.cats_interfaces))
        confidence = float(data_dict['confidence'])
        assert (confidence >= 0. and confidence <= 1.)

        # Determine trial duration from the confidence level
        trial_duration = 4 * 60 # XXX Force trials to last 4 minutes

        print("Initiating a new probing trial on setup-%i for %i sec with a confidence of $f" % (setup_id + 1, trial_duration, confidence))

        # Send new behaviour to the CATS instance
        self.cats_interfaces[setup_id].set_robot_behaviour("CW")

        # Wait for the trial to be completed
        time.sleep(trial_duration)

        # Reset robot behaviour to Social Integration through the CATS instance
        self.cats_interfaces[setup_id].set_robot_behaviour("SI")

        # Compute a modulation score
        statistics = self.cats_interfaces[setup_id].get_last_history()
        fishClockWiseFrequency = statistics['fishClockWisePercent']
        modulation_score = abs(fishClockWiseFrequency - np.mean(_referenceClockwiseFrequencies)) # TODO more adequate scheme
        if modulation_score > 0.02: # TODO more adequate scheme
            surprise = 1
        else:
            surprise = 0

        # Send a response to the Interspecies interface
        response_name = "FishManager"
        response_message_type = "ProbeDone"
        response_sender = "setup-%i" % (setup_id + 1)
        response_data = "Score:%f;Modulated:%i;Duration:%i" % (modulation_score, surprise, trial_duration)
        self.publisher.send_multipart([response_name, response_message_type, response_sender, response_data])
        except zmq.ZMQError as e:
            pass # TODO




#    def _send_data(self):
#        """ Forward data to the Interspecies interface """
#        while not self.stop:
#            try:
#                pass # TODO
#            except zmq.ZMQError as e:
#                continue
#            time.sleep(self.publishing_period)




class CatsIntersetupInterface:
    """ Interface to CATS intersetup system through ZMQ sockets """

    def __init__(self, instance_name, subscriber_addr = 'tcp://fishtrack:5555', publisher_addr = 'tcp://fishtrack:5556', publishing_period = 1.0, robot_behaviour = "CW"):
        self.instance_name = instance_name
        self.subscriber_addr = subscriber_addr
        self.publisher_addr = publisher_addr
        self.publishing_period = publishing_period
        self._robot_behaviour = robot_behaviour
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
        print("Successfully connected CATS instance subscriber to address: '%s'" % self.subscriber_addr)

        # Create and connect publisher
        self.publisher = self.context.socket(zmq.PUB)
        self.publisher.connect(self.publisher_addr)
        print("Successfully connected CATS instance publisher to address: '%s'" % self.publisher_addr)

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
        result = self._fish_history[self._last_history_index]
        self._history_lock.release()
        return result


    def set_robot_behaviour(self, robot_behaviour):
        self._robot_behaviour_lock.acquire()
        self._robot_behaviour = robot_behaviour
        self._robot_behaviour_lock.release()


    def _receive_data(self):
        """ Handle incoming data streams from a CATS instance """
        self._incoming_thread_starting_time = time.time()
        while not self.stop:
            try:
                [name, message_type, sender, data] = self.subscriber.recv_multipart(flags=zmq.NOBLOCK)
                self._incoming_thread_elapsed_time = int(time.time() - self._incoming_thread_starting_time)
                print("#%d\tfrom:%s\tname:%s device:%s command:%s data:%s" % (self._incoming_thread_elapsed_time, self.subscriber_addr, name, message_type, sender, data))

                # Save packet in history
                self._history_lock.acquire()
                self._raw_history[self._incoming_thread_elapsed_time] = [name, message_type, sender, data]
                if message_type.decode('ascii').lower() == "statistics":
                    data_list = data.decode('ascii').split(';')
                    data_dict = {}
                    for entry in data_list:
                        entry_split = entry.split(':')
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
                #    #print('Received statistics from cats: ' + name + ';' + message_type + ';' + sender + ';' + data)
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
                #    #print('Received statistics from cats: ', self.value_by_path)
                #    self.hist_values_by_path.append(copy.deepcopy(self.value_by_path))
                #    self.lock.release()
                #elif sender == 'statistics':
                #    print('Received statistics list from cats: ' + data)
                #    self.lock.acquire()
                #    self.available_statistics = data
                #    self.lock.release()
            except zmq.ZMQError as e:
                time.sleep(0.1)
                continue


    def _send_data(self):
        """ Forward data to a CATS instance """
        while not self.stop:
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
        print('Intersetup CATS instance subscriber thread finished')
        while self.incoming_thread.isAlive():
            time.sleep(0.1)
        print('Intersetup CATS instance receiving thread finished')



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
    args = parser.parse_args()


    # TODO specify parameters through command line options
    # Connect to CATS instances
    cats_interface1 = CatsIntersetupInterface("setup-1", args.intersetupSubscriberAddr1, args.intersetupPublisherAddr1, 1.0, "CW")
    #cats_interface2 = CatsIntersetupInterface("setup-2", args.intersetupSubscriberAddr2, args.intersetupPublisherAddr2, 1.0, "CW")

    # Launch manager thread
    #manager = InterspeciesManager([cats_interface1, cats_interface2], args.interspeciesInterfaceSubscriberAddr, args.interspeciesInterfacePublisherAddr, 30.0)
    manager = InterspeciesManager([cats_interface1], args.interspeciesInterfaceSubscriberAddr, args.interspeciesInterfacePublisherAddr, 30.0)

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

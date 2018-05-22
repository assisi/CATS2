#!/usr/bin/env python3

import zmq
import time
import threading
import numpy as np



class IntersetupMSManager(object):
    """ Intersetup manager, to manage several CATS instances in Master-Slave mode """

    def __init__(self, master_cats_interface, slave_cats_interface, period = 0.1):
        self.master_cats_interface = master_cats_interface
        self.slave_cats_interface = slave_cats_interface
        self.period = period

        # Create and start threads
        self._managing_thread = threading.Thread(target = self._manage_interspecies)
        self._stop = False
        self._managing_thread.start()


    def stop_all(self):
        """Stops all threads."""
        self._stop = True
        while self._managing_thread.isAlive():
            time.sleep(0.1)
        print('Managing thread finished')


    def _manage_interspecies(self):
        while not self._stop_threads:
            try:
                last_behaviour = self.master_cats_interface.get_last_history()
                if last_behaviour != None:
                    x = last_behaviour['x']
                    y = last_behaviour['y']
                    self.slave_cats_interface.set_robot_target_position([x, y])
            except zmq.ZMQError as e:
                time.sleep(0.1)
                continue
            for elapsed_time in range(int(self.period / 0.10)):
                if not self._stop_threads:
                    time.sleep(0.1)



class RobotTargetCatsInterface:
    """ Interface to CATS system through ZMQ sockets, to specify robot targets """

    def __init__(self, instance_name, subscriber_addr = 'tcp://fishtrack:5555', publisher_addr = 'tcp://fishtrack:5556', robot_target_position = [0.0, 0.0]):
        self.instance_name = instance_name
        self.subscriber_addr = subscriber_addr
        self.publisher_addr = publisher_addr
        self._robot_target_position = robot_target_position
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
        self.publisher.bind(self.publisher_addr)
        print("Successfully connected CATS instance publisher to address: '%s'" % self.publisher_addr)

        # Create and start threads
        self.incoming_thread = threading.Thread(target = self._receive_data)
        self._stop = False
        self.incoming_thread.start()

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


    def set_robot_target_position(self, robot_target_position):
        """ Forward Robot target position to a CATS instance """
        self._robot_target_position = robot_target_position
        try:
            name = b''
            message_type = b'RobotTargetPositionChanged'
            sender = b'FishManager'
            data = bytes("x:%d;y:%d;" % (robot_target_position[0], robot_target_position[1]), "ascii")
            self.publisher.send_multipart([name, message_type, sender, data])
        except zmq.ZMQError as e:
            pass # TODO


    def _receive_data(self):
        """ Handle incoming data streams from a CATS instance """
        self._incoming_thread_starting_time = time.time()
        while not self._stop:
            try:
                [name, message_type, sender, data] = self.subscriber.recv_multipart(flags=zmq.NOBLOCK)
                self._incoming_thread_elapsed_time = int(time.time() - self._incoming_thread_starting_time)
                print("#%d\tfrom:%s\tname:%s device:%s command:%s data:%s" % (self._incoming_thread_elapsed_time, self.subscriber_addr, name, message_type, sender, data))

                # Save packet in history
                self._history_lock.acquire()
                self._raw_history[self._incoming_thread_elapsed_time] = [name, message_type, sender, data]
                if message_type.decode('ascii').lower() == "robottargetpositionchanged":
                    data_list = data.decode('ascii').split(';')
                    data_dict = {}
                    for entry in data_list:
                        entry_split = entry.split(':')
                        data_dict[entry_split[0].lower()] = entry_split[1]
                self._history[self._incoming_thread_elapsed_time] = data_dict
                self._last_history_index = self._incoming_thread_elapsed_time
                self._history_lock.release()
            except zmq.ZMQError as e:
                time.sleep(0.1)
                continue


    def stop_all(self):
        """Stops all threads."""
        self._stop = True
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
    args = parser.parse_args()


    # Connect to CATS instances
    cats_interface1 = RobotTargetCatsInterface("setup-1", args.intersetupSubscriberAddr1, args.intersetupPublisherAddr1, [0.0, 0.0])
    cats_interface2 = RobotTargetCatsInterface("setup-2", args.intersetupSubscriberAddr2, args.intersetupPublisherAddr2, [0.0, 0.0])

    # Launch manager thread
    manager = IntersetupMSManager(cats_interface1, cats_interface2)


    cmd = 'a'
    while cmd != 'q':
        cmd = input('To stop the program press q<Enter>')

    manager.stop_all()
    cats_interface1.stop_all()
    cats_interface2.stop_all()


# MODELINE	"{{{1
# vim:expandtab:softtabstop=4:shiftwidth=4:fileencoding=utf-8
# vim:foldmethod=marker

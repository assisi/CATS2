#!/usr/bin/env python3

import zmq
import time

import threading
import time
import numpy as np
from subprocess import Popen
import os
import shutil



class Behaviour(object):
    """ TODO """
    def __init__(self):
        pass # TODO
    def inferNextBehaviour(self, currentState):
        pass # TODO


class ConstantBehaviour(Behaviour):
    """ TODO """
    def __init__(self, robotBehaviourType):
        super().__init__()
        self.robotBehaviourType = robotBehaviourType
    def inferNextBehaviour(self, currentState):
        return self.robotBehaviourType


class InterspeciesManager(object):
    """ TODO """

    def __init__(self, catsInterfaces, period = 30.0):
        self.catsInterfaces = catsInterfaces
        self.period = period
        self._stopThreads = False
        self._managingThread = threading.Thread(target = self._manageInterspecies)
        self._currentBehaviour = Behaviour()
        self._currentBehaviourLock = threading.Lock()

    def start(self):
        self._managingThread.start()

    def stop(self):
        self._stopThreads = True
        while self._managingThread.isAlive():
            time.sleep(0.1)

    def setBehaviour(self, behaviour):
        self._currentBehaviourLock.acquire()
        self._currentBehaviourLock = behaviour
        self._currentBehaviourLock.release()

    def _manageInterspecies(self):
        while not self._stopThreads:
            try:
                pass # TODO
            except zmq.ZMQError as e:
                time.sleep(0.1)
                continue
            for elapsedTime in range(int(self.period)):
                if not self._stopThreads:
                    time.sleep(1.0)




class CatsIntersetupInterface:
    """ Interface to CATS intersetup system through ZMQ sockets """

    def __init__(self, subscriberAddr = 'tcp://fishtrack:5555', publisherAddr = 'tcp://fishtrack:5556', publishingPeriod = 1.0, robotBehaviour = "CW"):
        self.subscriberAddr = subscriberAddr
        self.publisherAddr = publisherAddr
        self.publishingPeriod = publishingPeriod
        self._robotBehaviour = robotBehaviour
        self._robotBehaviourLock = threading.Lock()
        self._history = {}
        self._fishHistory = {}
        self._robotHistory = {}
        self._historyLock = threading.Lock()

        # Create and connect subscriber
        self.context = zmq.Context(2)
        self.subscriber = self.context.socket(zmq.SUB)
        #self.subscriber.setsockopt(zmq.RCVTIMEO, 1000)
        self.subscriber.setsockopt(zmq.SUBSCRIBE, b'')
        self.subscriber.connect(self.subscriberAddr)
        print("Successfully connected subscriber to address: '%s'" % self.subscriberAddr)

        # Create and connect publisher
        self.publisher = self.context.socket(zmq.PUB)
        self.publisher.connect(self.publisherAddr)
        print("Successfully connected publisher to address: '%s'" % self.publisherAddr)

        # Create and start threads
        self.incoming_thread = threading.Thread(target = self._receive_data)
        self.outgoing_thread = threading.Thread(target = self._send_data)
        self.stop = False
        self.incoming_thread.start()
        self.outgoing_thread.start()

    def getHistory(self, index = None):
        if index:
            self._historyLock.acquire()
            result = self._history[index]
            self._historyLock.release()
        else:
            self._historyLock.acquire()
            result = self._history
            self._historyLock.release()
        return result

    def getFishHistory(self, index = None):
        if index:
            self._historyLock.acquire()
            result = self._fishHistory[index]
            self._historyLock.release()
        else:
            self._historyLock.acquire()
            result = self._fishHistory
            self._historyLock.release()
        return result

    def getRobotHistory(self, index = None, robotIndex = None):
        if index:
            self._historyLock.acquire()
            if robotIndex:
                result = self._robotHistory[index][robotIndex]
            else:
                result = self._robotHistory[index]
            self._historyLock.release()
        else:
            self._historyLock.acquire()
            result = self._robotHistory
            self._historyLock.release()
        return result


    def setRobotBehaviour(self, robotBehaviour):
        self._robotBehaviourLock.acquire()
        self._robotBehaviour = robotBehaviour
        self._robotBehaviourLock.release()

    def _receive_data(self):
        """ Handle incoming data streams from a CATS instance """
        self._incoming_thread_starting_time = time.time()
        while not self.stop:
            try:
                [name, device, command, data] = self.subscriber.recv_multipart(flags=zmq.NOBLOCK)
                self._incoming_thread_elapsed_time = int(time.time() - self._incoming_thread_starting_time)
                print("#%d\tfrom:%s\tname:%s device:%s command:%s data:%s" % (self._incoming_thread_elapsed_time, self.subscriberAddr, name, device, command, data))

                self._historyLock.acquire()
                self._history[self._incoming_thread_elapsed_time] = [name, device, command, data]
                if name == b'casu-001':
                    robotIndex = 0 # XXX Only two robots
                elif name = b'casu-002':
                    robotIndex = 1 # XXX Only two robots
                data_pair = data.decode('ascii').split(',')
                fishCurrentBehaviourType = data_pair[0].split(' ')[1]
                robotCurrentBehaviourType = data_pair[1].split(' ')[1]
                self._fishHistory[self._incoming_thread_elapsed_time] = fishCurrentBehaviourType
                if self._robotHistory.get(self._incoming_thread_elapsed_time):
                    self._robotHistory[self._incoming_thread_elapsed_time][robotIndex] = robotCurrentBehaviourType
                else:
                    behaviourList = ['unk'] * 2 # XXX Only two robots
                    behaviourList[robotIndex] = robotCurrentBehaviourType
                    self._robotHistory[self._incoming_thread_elapsed_time][robotIndex] = behaviourList
                self._historyLock.release()

                #if (command == 'update'):
                #    #print('Received statistics from cats: ' + name + ';' + device + ';' + command + ';' + data)
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
                #elif command == 'statistics':
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
                self._robotBehaviourLock.acquire()
                robotBehaviour = self._robotBehaviour
                data = b''
                name = b'cats'
                device = b''
                if robotBehaviour == "CW":
                    command = b'CW'
                elif robotBehaviour == "CCW":
                    command = b'CCW'
                elif robotBehaviour == "SI":
                    command = b'Follow'
                else:
                    command = b''
                self.publisher.send_multipart([name, device, command, data])
                self._robotBehaviourLock.release()
            except zmq.ZMQError as e:
                continue
            time.sleep(self.publishingPeriod)


    def stop_all(self):
        """Stops all threads."""
        self.stop = True
        while self.outgoing_thread.isAlive():
            time.sleep(0.1)
        print('Interspecies subscriber thread finished')
        while self.incoming_thread.isAlive():
            time.sleep(0.1)
        print('Interspecies receiving thread finished')



if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument('--intersetupSubscriberAddr1', type=str, default='tcp://fishtrack:5555',
                        help="address to listen for messages.")
    parser.add_argument('--intersetupPublisherAddr1', type=str, default='tcp://fishtrack:5556',
                        help="address to send messages to.")
    parser.add_argument('--intersetupSubscriberAddr2', type=str, default='tcp://fishtrack2:5555',
                        help="address to listen for messages.")
    parser.add_argument('--intersetupPublisherAddr2', type=str, default='tcp://fishtrack2:5556',
                        help="address to send messages to.")
    args = parser.parse_args()


    # Connect to CATS instances
    catsInterface1 = CatsIntersetupInterface(args.intersetupSubscriberAddr1, args.intersetupPublisherAddr1, 1.0, "CW")
    catsInterface2 = CatsIntersetupInterface(args.intersetupSubscriberAddr2, args.intersetupPublisherAddr2, 1.0, "CW")

    # Launch manager thread
    manager = InterspeciesManager([catsInterface1, catsInterface2], 30.0)
    manager.start()

    # Launch statistics threads
    # TODO


    cmd = 'a'
    while cmd != 'q':
        cmd = input('To stop the program press q<Enter>')

    manager.stop()
    catsInterface1.stop_all()
    catsInterface2.stop_all()


# MODELINE	"{{{1
# vim:expandtab:softtabstop=4:shiftwidth=4:fileencoding=utf-8
# vim:foldmethod=marker

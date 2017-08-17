import zmq
import threading
import time

from settings_interface import Request

class CatsStatisticsInterface:
    """Provides the read access to CATS2 statistics."""

    def __init__(self):
        self.posted_requests = set()
        self.available_statistics = ""
        self.lock = threading.Lock()

        self.value_by_path = dict()

        # Create and connect sockets 
        self.context = zmq.Context(2)

        self.subscriber = self.context.socket(zmq.SUB)
        self.subscriber.setsockopt(zmq.SUBSCRIBE, '')
        # Connects to the address to listen to CATS
        self.subscriber.connect('tcp://127.0.0.1:5560')
        print('Statistics subscriber connected!')

        self.publisher = self.context.socket(zmq.PUB)
        # Connects to the address to publish to CATS
        self.publisher.connect('tcp://127.0.0.1:5559')
        print('Statistics publisher connected!')

        self.incoming_thread = threading.Thread(target = self.recieve_statistics)
        self.outgoing_thread = threading.Thread(target = self.request_statistics)

        self.stop = False

        self.incoming_thread.start()
        self.outgoing_thread.start()

    def recieve_statistics(self):
        """Manages the incoming data stream from CATS.

        The message is the statistics update. 
        """
        while not self.stop:
            try: 
                [name, device, command, data] = self.subscriber.recv_multipart(flags=zmq.NOBLOCK)
                if (command == 'update'):
                    print('Received statistics from cats: ' + name + ';' + device + ';' + command + ';' + data)
                    statistics = data.split(';')
                    self.lock.acquire()
                    for statistics_pair in statistics:
                        id_value = statistics_pair.split(':')
                        if len(id_value) == 2:
                            self.value_by_path[id_value[0]] = id_value[1]
                    self.lock.release()    
                elif command == 'statistics':
                    print('Received statistics list from cats: ' + data)   
                    self.lock.acquire()
                    self.available_statistics = data            
                    self.lock.release()    
            except zmq.ZMQError, e:     
                time.sleep(0.1)         
                continue

    def request_statistics(self):
        """Subscribes to the CATS2 statistics.
        
        """
        while not self.stop:
            self.lock.acquire()
            if (len(self.posted_requests) > 0):
                request = self.posted_requests.pop()
                self.lock.release()
                if (request.requestType == 'subscribe'):
                    data = request.requestData
                    name = 'cats'
                    device = ''
                    command = 'subscribe'  
                   # print('Sent request: ' + name + ';' + device + ';' + command + ';' + data)
                    self.lock.acquire()
                    statistics = data.split(';')                    
                    for id in statistics:
                        if id not in self.value_by_path:
                            self.value_by_path[id] = None
                    self.lock.release()                    
                    self.publisher.send_multipart([name, device, command, data])                    
                elif (request.requestType == 'get-statistics'):
                    data = ''
                    name = 'cats'
                    device = ''
                    command = 'get-statistics'  
                    self.publisher.send_multipart([name, device, command, data])
                    #print('Sent request: ' + name + ';' + device + ';' + command + ';' + data)
            else:
                self.lock.release()
                time.sleep(0.1)

    def subscribe(self, ids):
        """Requests to get regularly the statistics values from CATS.
        
        """
        request = Request('subscribe', ids)
        self.lock.acquire()
        self.posted_requests.add(request)
        # print('Post request: ' + request.requestType + ';' + request.requestData)
        self.lock.release()

    def get_value(self, id):
        """Returns the statistics value."""
        self.lock.acquire()
        if id in self.value_by_path:
            value = self.value_by_path[id]
        else: 
            value = 0
        self.lock.release()
        return value

    def get_statistics_list(self):
        """Request all available statistics."""
        request = Request('get-statistics', '')
        self.available_statistics = ''
        self.lock.acquire()
        self.posted_requests.add(request)
        # print('Post request: ' + request.requestType + ';' + request.requestData)
        self.lock.release()
        timeout = 0.1
        time.sleep(timeout)
        counter = timeout
        waiting_answer = True
        while waiting_answer:
            counter += timeout
            if (counter >= 1.):
                counter = 0
                request = Request('get-statistics', '')
                self.lock.acquire()
                self.posted_requests.add(request)
                self.lock.release()
            time.sleep(timeout)
            self.lock.acquire()
            waiting_answer = len(self.available_statistics) == 0
            #print(self.available_statistics + "->" + waiting_answer)
            self.lock.release()
        return self.available_statistics

    def stop_all(self):
        """Stops all threads."""
        self.stop = True
        while self.outgoing_thread.isAlive():
            time.sleep(0.1)
        print('Statistics subscriber thread finished')
        while self.incoming_thread.isAlive():
            time.sleep(0.1)
        print('Statistics receiving thread finished')
import zmq
import threading
import time

class Request:
    """Set/get request for CATS2 settings variable."""

    def __init__(self, requestType = '', requestData = ''):
        self.requestType = requestType        
        self.requestData = requestData


class CatsSettingsInterface:
    """Provides the read/write access to CATS2 settings."""

    def __init__(self):
        self.posted_requests = set()
        self.sent_requests = set()
        self.lock = threading.Lock()

        self.value_by_path = dict()

        # Create and connect sockets 
        self.context = zmq.Context(1)

        self.subscriber = self.context.socket(zmq.SUB)
        self.subscriber.setsockopt(zmq.SUBSCRIBE, '')
        # Connects to the address to listen to CATS
        self.subscriber.connect('tcp://127.0.0.1:5556')
        print('Internet subscriber connected!')

        self.publisher = self.context.socket(zmq.PUB)
        # Connects to the address to publish to CATS
        self.publisher.connect('tcp://127.0.0.1:5555')
        print('Internet publisher connected!')

        self.incoming_thread = threading.Thread(target = self.recieve_settings)
        self.outgoing_thread = threading.Thread(target = self.send_settings)

        self.stop = False

        self.incoming_thread.start()
        self.outgoing_thread.start()

    def recieve_settings(self):
        """Manages the incoming data stream from CATS.

        Only set requests that are replies on our get requests are managed at 
        the moment. 
        """
        while not self.stop:
            try: 
                [name, device, command, data] = self.subscriber.recv_multipart(flags=zmq.NOBLOCK)
                print('Received from cats: ' + name + ';' + device + ';' + command + ';' + data)
                if (command == 'set'):
                    self.lock.acquire()
                    self.value_by_path[device] = float(data) 
                    self.sent_requests.discard(device)
                    self.lock.release()             
            except zmq.ZMQError, e:     
                time.sleep(0.1)         
                continue

    def send_settings(self):
        """Manages the outcoming data stream from CATS.
        
        The requests to send are taken from posted_requests set, the values for 
        set requests are taken from value_by_path dictionary. Once sent the 
        request's path (it's kind of id) is moved to the sent_requests set. 
        """
        while not self.stop:
            self.lock.acquire()
            if (len(self.posted_requests) > 0):
                request = self.posted_requests.pop()
                self.lock.release()
                if (request.requestType == 'get'):
                    data = ''
                    name = 'cats'
                    device = request.requestData
                    command = 'get'  
                   # print('Sent request: ' + name + ';' + device + ';' + command + ';' + data)
                    self.lock.acquire()
                    self.sent_requests.add(request.requestData)
                    if request.requestData not in self.value_by_path:
                        self.value_by_path[request.requestData] = None
                    self.lock.release()                    
                    self.publisher.send_multipart([name, device, command, data])                    
                elif (request.requestType == 'set'):
                    self.lock.acquire()
                    data = str(self.value_by_path[request.requestData])
                    self.lock.release()                    
                    name = 'cats'
                    device = request.requestData
                    command = 'set'  
                    self.publisher.send_multipart([name, device, command, data])
                    #print('Sent request: ' + name + ';' + device + ';' + command + ';' + data)
            else:
                self.lock.release()
                time.sleep(0.1)

    def set_value(self, path):
        """Requests the settings value from CATS until it received.
        
        The resulted settings value is read from value_by_path dictionary.
        """
        request = Request('get', path)
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
                request = Request('get', path)
                self.lock.acquire()
                self.posted_requests.add(request)
        #        print('Re-post request: ' + request.requestType + ';' + request.requestData)
                self.lock.release()
            time.sleep(timeout)
            self.lock.acquire()
            waiting_answer = (path in self.sent_requests) or (request in self.posted_requests)
            self.lock.release()
        return self.value_by_path[path]

    def set_value(self, path, value):
        """Updates the settings value in CATS2."""
        request = Request('set', path)
        self.lock.acquire()
        self.value_by_path[path] = value # we assume that this value will not \
                                         # be rewritten from the CATS settings\
                                         # before sent
        self.posted_requests.add(request)
       # print('Post request: ' + request.requestType + ';' + request.requestData)
        self.lock.release()

    def stop_all(self):
        """Stops all threads."""
        self.stop = True
        while self.outgoing_thread.isAlive():
            time.sleep(0.1)
        print('Sending thread finished')
        while self.incoming_thread.isAlive():
            time.sleep(0.1)
        print('Receiving thread finished')
import zmq
import time
import argparse


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-a', '--addr', type=str, default='tcp://localhost:10111',
                        help="address to listen for messages."
                        "NOTE: recv side cannot have a * to replace the host")
    args = parser.parse_args()

    # connect
    context = zmq.Context(1)
    listener = context.socket(zmq.SUB)
    listener.setsockopt(zmq.RCVTIMEO, 1000)
    listener.setsockopt(zmq.SUBSCRIBE, '') #  need to filter on nothing to accept incoming...
    #listener.connect("tcp://127.0.0.1:10111") # equiv to localhost
    #listener.connect("tcp://localhost:10111") # recv this port when tx to *:port

    listener.connect(args.addr)

    t0 = time.time()

    i = 0
    j = 0
    try:
        while True:
            try:
                [name, msg, sender, data] = listener.recv_multipart()
                print "[recv]", msg
                i+= 1

            except zmq.ZMQError as e:
                if e.errno == zmq.EAGAIN:
                    print "passing. {:.1f}".format(time.time() - t0)
                    j +=1
                    continue

            print name, msg, sender, data

    except KeyboardInterrupt:
        print "[I] done. bye"

    tend= time.time()
    dur = tend-t0
    print "Recv: {} timeouts: {}. {:.1f}sec".format(i,j, dur)




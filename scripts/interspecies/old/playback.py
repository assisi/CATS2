'''

read a simple fish-casu log file, transmit it as if it were coming from
an active fish-casu (CATS) system. Transmits the message in row i when time(i)
has elapsed.

Input data
==========

This script assumes an input file with two columns, time and direction.
The data is loaded with np.loadtxt so it is straightforward to update the
arguments if data arrives in a different form (e.g. csv rather than space
delimited)


Messages
========
The form of the messages that are transmitted around the bee casu system are
multipart, with 4 parts:
    name, message_type, sender, msg


In the simple binary-choice interspecies system, the parts were constant.

for fish --> bee messages:

    sender = 'cats'
    msg_type = 'Message'

    sender initially used "fishCasu-01" and then "fishCasu"

    data contains the payload, which is structured into elements such as
        fish:CCW,fishCasu: CW

    the direction could be {CW, CCW, Undef}


    name is the message target. This could be either casu-001 or casu-002.
    The relay code took these psuedo-logical names and translated into physical
    hardware names, e.g. casu-002 -> tcp://bbg-001:10104


for bee --> fish messages
    name is the target, which is always "cats"
    msg_type type is either Message or CommEth (the field is actually ignored but the
    agreed value is Message)

    sender casu-001
    data contains a single floating point value in [0,1].


'''

import zmq
import time
import numpy as np
import argparse


#{{{ handling the CASU and messaging
def setup_messenger(addr="tcp://*:10987"):
    context = zmq.Context(1)
    pub = context.socket(zmq.PUB)
    pub.bind(addr)
    print "[I] bound to {}, socket in publisher mode".format(addr)
    time.sleep(0.5)
    return pub
#}}}

#{{{ disp_and_send
def disp_and_send(pub, name, sender, msg, ty="Message", verb=False):
    '''
    given a publisher that is already opened, send messages to casus

    '''
    if verb:
        #print('Sending: ' + name + ';' + ty + ';' + sender + ';' + msg)
        print('---> name: {} ; typ: {} ; sender : {}; msg: {}. '.format(
            name, ty, sender, msg))
    pub.send_multipart([name, ty, sender, msg])

def send_density_value_ts(pub, target_casu, val, loc, ts=-1,
        sender="vidsensor-001", verb=False):
    '''
    emit an info message in form
    "vid_sensor, casu-xxx, 0.45, <utc_stamp>"
    '''
    msg = "vid_sensor_t, {}, {:.3f}, {}".format(loc, val, ts)
    disp_and_send(pub, name=target_casu, sender=sender, msg=msg, verb=verb)

def send_density_value(pub, target_casu, val, loc,
        sender="vidsensor-001", verb=False):
    '''# emit an info message in form "vid_sensor, casu-xxx, 0.45" '''
    msg = "vid_sensor, {}, {:.3f}".format(loc, val)
    disp_and_send(pub, name=target_casu, sender=sender, msg=msg, verb=verb)

def _send_density_value(pub, linklabel, val, loc,
        sender="vidsensor-001", verb=False):
    # emit an info message in form "vid_sensor, casu-xxx, 0.45"
    msg = "vid_sensor, {}, {:.3f}".format(loc, val)
    disp_and_send(pub, name=linklabel, sender=sender, msg=msg, verb=verb)
#}}}



if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-d', '--datafile', type=str, default=None)
    parser.add_argument('-a', '--addr', type=str, default='tcp://*:10111')
    args = parser.parse_args()

    # open port
    msg_emitter = setup_messenger(args.addr)
    #casu_linknames = load_casu_linknames(args.configfile )
    casu_linknames = ['casu-001', 'casu-002' ] # not sure which direciton?
    #casu_linknames = ['casu-022', 'casu-023' ] # not sure which direciton?

    # read data to replay
    if args.datafile is not None:
        D = np.loadtxt(args.datafile)


    sender = 'cats'
    msg_type = 'Message'
    origin = "fishCasu-01"


    #disp_and_send(msg_emitter, casu_linknames[0], sender=sender,
    #        msg="fishCasu-01:left", ty="Message", verb=True)

    t_start = time.time()

    try:
        i = 0
        while True:
            if i >= len(D):
                print "reached end of file after {}s".format(D[i,0])
                break

            now = time.time()
            elap = now - t_start
            t_next_msg = D[i, 0]
            if elap > t_next_msg:
                drn = D[i, 1]
                if drn == 1.0:
                    msg = "{}:{}".format(origin, "CW")
                elif drn == 0.0:
                    msg = "{}:{}".format(origin, "CCW")
                else:
                    # not sure what to send?
                    msg = "{}:{}".format(origin, "Undef")
                    continue

                # emit the mssage
                print "{}s elapsed. \t".format(t_next_msg),
                disp_and_send(msg_emitter, casu_linknames[0], sender=sender,
                              msg=msg, ty=msg_type, verb=True)
                i += 1


            time.sleep(0.1)
    except KeyboardInterrupt:
        print "giving up"




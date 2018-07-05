#!/bin/bash


# June 2018 Workshop, with ISI on graz workstation
setup1SubAddr=tcp://172.27.34.7:5535
setup1PubAddr=tcp://172.27.34.3:5536
setup2SubAddr=tcp://127.0.0.1:5545
setup2PubAddr=tcp://127.0.0.1:5546
ISISubAddr=tcp://143.50.158.98:5555 # GRATZ Workstation
#ISISubAddr=tcp://10.42.1.157:5555    # Rob's laptop

ISIPubAddr=tcp://127.0.0.1:5556
#ISIPubAddr=tcp://172.27.34.3:5556
trialDuration=120 #120

# June 2018 Workshop, with ISI on Rob's computer on ip 10.42.1.157:5565
#setup1SubAddr=tcp://127.0.0.1:5545
#setup1PubAddr=tcp://127.0.0.1:5546
#ISISubAddr=tcp://10.42.1.157:5565
#ISIPubAddr=tcp://172.27.34.3:5566
#trialDuration=120


logDirectory=logs/
currentDate=`date "+%FT%H%M%S"`

mkdir -p $logDirectory


pidSSH=""
#pidFM=""
function cleanup {
	echo "Cleaning up..."
	if [ ! -z "$pidSSH" ]; then
		kill $pidSSH
	fi
#	if [ ! -z "$pidFM" ]; then
#		kill $pidFM
#	fi
}
trap cleanup EXIT TERM INT

# Launch SSH Tunnel
autossh -M 0 -N -R 5556:localhost:5556 -o "GatewayPorts yes" -o "ServerAliveInterval 20" -o "ServerAliveCountMax 3" -o "StrictHostKeyChecking=no" -o "BatchMode=yes" -i /home/assisi/.ssh/id_rsa assisi@vps 2>&1 > $logDirectory/ssh-$currentDate.log &
pidSSH=$!

# Launch Fish Manager
./fish_manager_2Rooms.py --intersetupSubscriberAddr1 $setup1SubAddr --intersetupPublisherAddr1 $setup1PubAddr --intersetupSubscriberAddr2 $setup2SubAddr --intersetupPublisherAddr2 $setup2PubAddr --interspeciesInterfaceSubscriberAddr $ISISubAddr --interspeciesInterfacePublisherAddr $ISIPubAddr --trialDuration $trialDuration  2>&1 | tee $logDirectory/fish_manager-$currentDate.log #&
#pidFM=$!
#
#wait $pidSSH
#wait $pidFM


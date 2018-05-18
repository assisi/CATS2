#!/bin/sh

autossh -M 0 -N -R 5556:localhost:5556 -o "GatewayPorts yes" -o "ServerAliveInterval 20" -o "ServerAliveCountMax 3" -o "StrictHostKeyChecking=no" -o "BatchMode=yes" -i /home/assisi/.ssh/id_rsa assisi@vps

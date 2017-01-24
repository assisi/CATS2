#!/bin/bash

args="$1"

cmdLine=''

killall asebamedulla
#killall /usr/bin/asebamedulla
sleep 1

if [[ $args == *"A"* ]]; then
	cmdLine="$cmdLine ser:device=/dev/rfcomm0"
fi

if [[ $args == *"B"* ]]; then
	cmdLine="$cmdLine ser:device=/dev/rfcomm1"
fi

if [[ $args == *"C"* ]]; then
	cmdLine="$cmdLine ser:device=/dev/rfcomm2"
fi

if [[ $args == *"D"* ]]; then
	cmdLine="$cmdLine ser:device=/dev/rfcomm3"
fi

if [[ $args == *"E"* ]]; then
	cmdLine="$cmdLine ser:device=/dev/rfcomm4"
fi

if [[ $args == *"F"* ]]; then
	cmdLine="$cmdLine ser:device=/dev/rfcomm5"
fi

if [[ $args == *"G"* ]]; then
	cmdLine="$cmdLine ser:device=/dev/rfcomm6"
fi

if [[ $args == *"H"* ]]; then
	cmdLine="$cmdLine ser:device=/dev/rfcomm7"
fi

if [[ $args == *"I"* ]]; then
	cmdLine="$cmdLine ser:device=/dev/rfcomm8"
fi

if [[ $args == *"J"* ]]; then
	cmdLine="$cmdLine ser:device=/dev/rfcomm9"
fi

if [[ $cmdLine != "" ]]; then
	asebamedulla -v -d $cmdLine | tee
	#/usr/bin/asebamedulla -v -d $cmdLine | tee
fi


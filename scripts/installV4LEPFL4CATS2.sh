#!/bin/bash
# TODO Description

set -e

pid1=""

function cleanup {
	echo "Quitting camera-related gstreamer processus..."
	if [ ! -z "$pid1" ]; then
		kill $pid1
	fi
}
trap cleanup EXIT TERM INT

# Verify that PylonViewerApp is not launched
isPylonLaunched=`ps ax | grep PylonViewerApp | grep -v grep | sed 's/[[:space:]]*$//'`
if [ ! -z "$isPylonLaunched" ]; then
	echo PylonViewerApp already launched. Please quit PylonViewerApp before attempting to run this script.
	exit 1
fi

sudo modprobe v4l2loopback devices=4

sleep 2
gst-launch-0.10 --gst-plugin-path=/usr/local/lib/gstreamer-0.10/ --gst-plugin-load=libgstaravis-0.4.so aravissrc  \
		! video/x-raw-gray,bpp=8,depth=8,width=2040,height=2040,framerate=15/1 ! tee name=t \
		! queue ! ffmpegcolorspace ! videoscale ! videorate ! video/x-raw-yuv,width=2040,height=2040 ! v4l2sink device=/dev/video0 sync=false \
	t.	! queue ! ffmpegcolorspace ! videoscale ! video/x-raw-yuv,width=512,height=512 ! v4l2sink device=/dev/video1 sync=false &
pid1=$!



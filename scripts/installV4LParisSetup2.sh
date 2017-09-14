#!/bin/bash
# TODO Description

set -e

pid1=""
pid2=""
pid3=""

function cleanup {
	echo "Quitting camera-related gstreamer processus..."
	if [ ! -z "$pid1" ]; then
		kill $pid1
	fi
	if [ ! -z "$pid2" ]; then
		kill $pid2
	fi
	if [ ! -z "$pid3" ]; then
		kill $pid3
	fi
}
trap cleanup EXIT TERM INT

# Verify that PylonViewerApp is not launched
isPylonLaunched=`ps ax | grep PylonViewerApp | grep -v grep | sed 's/[[:space:]]*$//'`
if [ ! -z "$isPylonLaunched" ]; then
	echo PylonViewerApp already launched. Please quit PylonViewerApp before attempting to run this script.
	exit 1
fi

sudo modprobe -r uvcvideo

sudo modprobe -r v4l2loopback
sleep 2
sudo modprobe v4l2loopback devices=4 exclusive_caps=0,0,0,0

sleep 4
gst-launch-0.10 --gst-plugin-path=/usr/local/lib/gstreamer-0.10/ --gst-plugin-load=libgstaravis-0.4.so aravissrc  \
		! video/x-raw-gray,bpp=8,depth=8,width=2040,height=2040,framerate=15/1 ! tee name=t \
		! queue ! ffmpegcolorspace ! videoscale ! videorate ! video/x-raw-yuv,width=2040,height=2040 ! v4l2sink device=/dev/video0 sync=false \
	t.	! queue ! ffmpegcolorspace ! videoscale ! video/x-raw-yuv,width=512,height=512 ! v4l2sink device=/dev/video1 sync=false &
pid1=$!
		#! queue ! ffmpegcolorspace ! videoscale ! videorate ! video/x-raw-yuv,width=1024,height=1024 ! v4l2sink device=/dev/video0 sync=false & #\
#pid1=$!
	#t.	! queue ! ffmpegcolorspace ! videoscale ! videorate ! video/x-raw-yuv,width=500,height=500,framerate=2/1 ! ffenc_mpeg4 ! rtpmp4vpay ! udpsink host=128.178.145.80 port=5200

sleep 5
gst-launch-0.10 v4l2src device=/dev/video0  ! queue ! ffmpegcolorspace ! videorate ! videoscale ! video/x-raw-yuv,width=500,height=500,framerate=5/1 ! jpegenc ! multipartmux ! tcpserversink port=5202 host=127.0.0.1 sync=false &
pid2=$!

#sleep 4
##gst-launch-1.0 -v v4l2src device=/dev/video1  ! queue ! videoconvert ! videorate ! videoscale ! video/x-raw,width=500,height=500,framerate=5/1 !  x264enc ! flvmux !  rtmpsink location='rtmp://streamyfish.com/rtmp/paris2 live=1' &
gst-launch-1.0 -v v4l2src device=/dev/video1  ! queue ! videoconvert ! videorate ! videoscale !  x264enc ! flvmux !  rtmpsink location='rtmp://streamyfish.com/rtmp/paris2 live=1' &
pid3=$!

sleep 1
sudo modprobe uvcvideo

wait $pid2
wait $pid3


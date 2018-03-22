#!/bin/bash

defaultBasePath="~/Dropbox/HEAF/data"
plotBasePath="~/Dropbox/HEAF/plots"
scriptsBasePath="~/Dropbox/HEAF/scripts"
videoBasePath="/data/videos"
numBuffers="$1"
#device="$3"
config="$2"

pid1=""
pid2=""
pid3=""
pid4=""


defaultBasePath="${defaultBasePath/#\~/$HOME}"
plotBasePath="${plotBasePath/#\~/$HOME}"
scriptsBasePath="${scriptsBasePath/#\~/$HOME}"
videoBasePath="${videoBasePath/#\~/$HOME}"

if [ -z "$defaultBasePath" ]; then
	if [ -e "/data/videos" ]; then
		defaultBasePath="/data/videos"
	else
		defaultBasePath="."
	fi
fi

if [ -z "$numBuffers" ]; then
	numBuffers=60000
fi

function cleanup {
	echo "Quitting camera-related gstreamer processus..."
	if [ ! -z "$pid1" ]; then
		kill $pid1
	fi
}
trap cleanup EXIT TERM INT


#if [ -z "$device" ]; then
#	device=/dev/video0
#fi

# Use filename based on current date
currentDate=`date "+%FT%H%M%S"`
expeDir=`./scripts/configInfos.py -l ${config} trajectoriesPath -d unknown`
orientation=`./scripts/configInfos.py -l ${config} orientation -d 0`
echo "Experiment directories (data,video,plots): '${expeDir}' with orientation: '${orientation}'"
mkdir -p "${videoBasePath}/${expeDir}"
mkdir -p "${defaultBasePath}/${expeDir}"
echo mkdir -p "${defaultBasePath}/${expeDir}"
#videoFilename="${videoBasePath}/${expeDir}/video-${currentDate}-orientation${orientation}.mkv"
videoFilename="${videoBasePath}/video-${currentDate}.mkv"
trajectoryFilename="positions-${currentDate}-orientation${orientation}.txt"
trajectoryPath="${defaultBasePath}/${expeDir}/positions-${currentDate}-orientation${orientation}.txt"

#commandToLaunchAfterExpe="'${scriptsBasePath}/performAnalysis.py -l ${config} -b -d ${defaultBasePath} -p ${plotBasePath} > outputCommandToLaunchAfterExpe &'"
commandToLaunchAfterExpe="./scripts/launchHEAF.sh ${defaultBasePath} ${plotBasePath} ${scriptsBasePath} ${config}"

echo "Saving video to '${videoFilename}'..."

#gst-launch -e v4l2src num-buffers=${numBuffers} device=${device}  ! ffmpegcolorspace ! videoscale ! video/x-raw-yuv,width=2040,height=2040,framerate=15/1 ! ffenc_huffyuv ! matroskamux ! filesink location=${videoFilename} &
gst-launch -e v4l2src num-buffers=${numBuffers} device=/dev/video0 ! queue ! ffmpegcolorspace ! videoscale ! video/x-raw-yuv,width=2040,height=2040,framerate=15/1 ! queue ! ffenc_huffyuv ! matroskamux ! filesink location=${videoFilename} &
pid1=$!

#gdb --args ./CATS -i v4l 1 -l $config -o ${trajectoryPath} -C "${commandToLaunchAfterExpe}"
#gdb --args ./CATS -i v4l 1 -l $config -o ${trajectoryPath} -C "${commandToLaunchAfterExpe}" &
#pid2=$!
../build/source/cats/cats -mc v4l 1 -bc v4l 4 -sri 1 -c $config # TODO

#sleep 5
#watch -n 120 "./scripts/launchStreamingHEAF.sh ${defaultBasePath} ${plotBasePath} ${scriptsBasePath} ${config} ${trajectoryPath} ${trajectoryFilename} > logStreamingHEAF" > /dev/null &
#pid3=$!

wait $pid1
echo "Finished video '${videoFilename}' !"
#kill $pid3


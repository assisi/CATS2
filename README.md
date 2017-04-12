# CATS2

Released under LGPL3, see [license.txt](license.txt) for details.

CATS2 goes for **C**ontrol **A**nd **T**racking **S**oftware.
It serves to monitor animal-robot experiments and to control robots in closed-loop.
CATS2 is built within the frame of ASSISIbf project (http://assisi-project.eu).

## Prerequisites

CATS2 depends on the following packages
* CMake 3.2 or newer
* Qt5.6 or newer
* gstreamer-0.10
* qt-gstreamer-0.10.3
* OpenCV 3.x (with dependecies)
* Boost
* protobuf
* [aseba](https://github.com/aseba-community/aseba)

## Run CATS2

This line will launch CATS2 with the video4linux drivers and input device "1":

  ./cats -mc v4l 1 -c <path-to-configuration-file>

Other than **v4l** you can use **vf** (to use a video file) or **if** (to use a still image),
in this case you need to provide the path to the corresponding file.

## Configuration files

Configuration files store parameters used by submodules of CATS2, such as the
camera calibration files, parameters of tracking routines, or parameters of the
robots' controllers. Examples of the configuration files can be found in
[config](config).

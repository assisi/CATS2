#!/bin/bash

export DISPLAY=:0.0

cd /home/assisi/CATS2/CATS2
mkdir -p logs
cd scripts
./mkVideoAndLaunchCATS2.sh 19000 ../config/cats2-p7setup1-circular.xml > ../logs/setup1.log 2>&1

#!/bin/bash

export DISPLAY=:0.0

cd /home/assisi/CATS2/CATS2
mkdir -p logs
cd scripts
./mkVideoAndLaunchCATS2.sh 32000 ../config/cats2-p7setup2-circular.xml > ../logs/setup2.log 2>&1


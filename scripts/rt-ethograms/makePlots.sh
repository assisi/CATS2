#!/bin/bash

export DISPLAY=:0.0


declare -a pids

arenaFilename=SetupLargeModelzoneBlack70x70v6A.png
#dataDir=data/simulation/20050-2017-03-05T210529/
#plotDir=plots/simulation
dataDir=$1
plotDir=$2


mkdir -p $plotDir


echo Launching analysis of data in "$dataDir" and export plots to "$plotDir" ...

./analyse_presences.py -d $dataDir -o $plotDir -x 1 -y 1 -a 4 -r 1 -S ".pdf" -C 3 -M 'Greys' &
pids+=" $!"

./plotsFeatures.py -i $dataDir -o $plotDir -a $arenaFilename -z 4 -k 0 &
pids+=" $!"

#./plotMC.py -i $dataDir -a $arenaFilename -z 6 -C "Void,Corridor,Center of\nRoom 1,Center of\nRoom 2,Walls of\nRoom 1,Walls of\nRoom 2" -s 42 -S 1000 -n 2 -k 0 -o $plotDir/mc.png &
./plotMC.py -i $dataDir -a $arenaFilename -z 4 -C "Void,Corridor,Near room walls,Center of Room" -s 42 -S 1000 -n 2 -k 0 -o $plotDir/mc.png &
pids+=" $!"

./analyse_traces.py -d $dataDir -o $plotDir -x 1 -y 1 -a 4 -r 1 -S ".pdf" -C 3 &
pids+=" $!"



#./computeScores.py -r $referenceDataDir -i $biohybridDataDir -o $biohybridPlotDir -a $arenaFilename -z 6 -k 5 &
pids+=" $!"


# Wait for plots to finish
#for i in $(seq 1 4); do
echo DEBUG1 "${pids[@]}"
for i in "${pids[@]}"; do
	#wait ${pids[$i]}
	wait $i
done

echo Successfully finished all plots !!
echo Sending plots to website

rsync -avz $plotDir assisi@vps:~/public_html/plots/paris2/


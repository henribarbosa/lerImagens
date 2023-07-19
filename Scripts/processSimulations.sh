#!/bin/bash
cd ..
while read line; do

	#IFS=' '
	#read -a list <<< ${line}
	#folder=${list[0]}
	#number=${list[1]}
	
	echo "${line}/DEM/post/dump_liggghts_run.*" #threshold: ${number}

	#rm thresholds.txt
	#cp Thresholds_base.txt thresholds.txt
	#sed -i "s/A1A/${number}/" thresholds.txt

	./lerImagem -m simulation -r all -p "${line}/DEM/post/dump_liggghts_run.*"
	
	cp -r PostFiles/ ${line}

done < Scripts/simulationFolders.txt

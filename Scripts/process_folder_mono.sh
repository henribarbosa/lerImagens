#!/bin/bash
while read line; do

	#IFS=' '
	#read -a list <<< ${line}
	#folder=${list[0]}
	#lower=${list[1]}
	#upper=${list[2]}
	
	echo \"${line}/image_*.tif\"

	#rm thresholds.txt
	#cp thresholds_base.txt thresholds.txt
	#sed -i "s/A1A/${lower}/" thresholds.txt
	#sed -i "s/A2A/${upper}/" thresholds.txt

	cd ..
	./lerImagem -m process -r all -t 0 -p "${line}/image_*.tif"
	
	cp -r Files/ ${line}

done < ToDoMonodisperse.txt


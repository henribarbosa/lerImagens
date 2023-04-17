#!/bin/bash
while read line; do

	IFS=' '
	read -a list <<< ${line}
	folder=${list[0]}
	number=${list[1]}
	
	echo \"${folder}/image_*.tif\" threshold: ${number}

	rm thresholds.txt
	cp Thresholds_base.txt thresholds.txt
	sed -i "s/A1A/${number}/" thresholds.txt

	./lerImagem -m process -r all -t 1 -p "${folder}/image_*.tif"
	
	cp -r Files/ ${folder}

done < ToDoBidisperse.txt

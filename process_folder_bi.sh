#!/bin/bash
while read line; do

	IFS=' '
	read -a list <<< ${line}
	folder=${list[0]}
	lower=${list[1]}
	upper=${list[2]}
	
	echo \"${folder}/image_*.tif\"

	rm thresholds.txt
	cp thresholds_base.txt thresholds.txt
	sed -i "s/A1A/${lower}/" thresholds.txt
	sed -i "s/A2A/${upper}/" thresholds.txt

	./lerImagem -m process -r all -t 1 -p "${folder}/image_*.tif"
	
	cp -r Files/ ${folder}

done < ToDoBidisperse.txt


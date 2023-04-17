#!/bin/bash
while read line; do

	#IFS=' '
	#read -a list <<< ${line}
	#folder=${list[0]}
	#number=${list[1]}
	
	echo \"${line}/image_*.tif\"

	#rm thresholds.txt
	#cp Thresholds_base.txt thresholds.txt
	#sed -i "s/A1A/${number}/" thresholds.txt

	./lerImagem -m process -r all -p "${line}/image_*.tif"
	
	cp -r Files/ ${line}

done < FoldersSimple.txt


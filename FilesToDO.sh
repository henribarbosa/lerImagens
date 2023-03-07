while read folder; do
	
	echo \"${folder}/image_*.tif\"

	./lerImagem -m process -r all -p "${folder}/image_*.tif"
	
	cp -r Files/ ${folder}

done < Folders.txt

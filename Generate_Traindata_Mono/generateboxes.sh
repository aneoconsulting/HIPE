
declare -a images=()
nb=$#
nb_arg=$(($nb-1))
for var in $@ ; do
	images+=($var)
done


#creer repertoir langdata dans repertoire courant s'il n'existe pas
	if [ ! -d langdata ];
	then mkdir langdata 
	fi	
# verifier que lang.font_properties exist sinon sortir du script
	if [ ! -f font_properties ];
	then echo "font properties file does not exist, you must create it"; exit -1; 
	fi

#creer la box
image=""
name=""
counter=0
while [ $counter -lt $nb ] ; do
	image="${images[$counter]}"
	name="${image::-4}"
	
	tesseract $image $name -psm 10 batch.nochop makebox

#verifier que .box exists et qu'il n'est pas nul sinon arreter le scirpt

	let counter=counter+1

done

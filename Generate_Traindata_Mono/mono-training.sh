
#training des caracteres seuls.
#arguments: noms images array


declare -a images=()
nb=$#
nb_arg=$(($nb-1))
for var in $@ ; do
	images+=($var)
done
counter=0
while [ $counter -lt $nb ] ; do
	image="${images[$counter]}"
	name="${image::-4}"
	boxes+=("${name}.box")
	trainfiles+=("${name}.tr")
	let counter=counter+1
done
echo $boxes
echo  $trainfiles

#creer unicharset
	unicharset_extractor ${boxes[*]}
#verifier que unicharset exists et non nul sinon arreter script

#setter l'unicharset
	set_unicharset_properties -U unicharset -O eng.unicharset --script_dir=langdata

#renommer unicharset de tel sorte à ce qu'on puisse l'utiliser dans la suite du training

	mv unicharset unicharset.bkp
	mv eng.unicharset unicharset
#generer int pffmtable shape
	mftraining -F font_properties -U unicharset -O eng.unicharset ${trainfiles[*]}

#verifier l'existencedes 3 fichiers

#generer normproto
	cntraining ${trainfiles[*]}

#verifier l'existence de normproto

#renommer tous les fichiers générés avec prefix eng.

mv normproto eng.normproto
mv inttemp eng.inttemp
mv pffmtable eng.pffmtable
mv shapetable eng.shapetable

combine_tessdata eng.



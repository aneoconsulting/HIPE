
declare -a images=()
nb=$#
nb_arg=$(($nb-1))
for var in $@ ; do
	images+=($var)
done

#creer la box
image=""
name=""
counter=0
while [ $counter -lt $nb ] ; do
	image="${images[$counter]}"
	name="${image::-4}"
	
#creer le tr
	tesseract $image $name -psm 10 nobatch box.train

#verifier le .tr non null sinon arreter script
	let counter=counter+1

done

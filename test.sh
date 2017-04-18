#!/bin/bash

m1=mat1;
m2=mat2;
	
# Ověření existence prvního souboru
if [ ! -f $m1 ]; then
    echo "Soubor mat1 neexistuje!"
	exit
fi

# Ověření existence druhého souboru
if [ ! -f $m2 ]; then
    echo "Soubor mat2 neexistuje!"
	exit
fi

#TODO - ověřit správnost matic v bashi!!!
	
mat1=$(head -n1 mat1)
mat2=$(head -n1 mat2)
	
mat1r=$(cat $m1 | wc -l)
mat1r=$((mat1r-1))
mat2r=$(cat $m2 | wc -l)
mat2r=$((mat2r-1))
	
if [ $mat1r -ne $mat1 ]; then
	n=$mat1r;
else
	n=$mat2r;
fi;
 
cpus=$((mat1*mat2))

mpic++ --prefix /usr/local/share/OpenMPI -o mm mm.cpp -std=c++0x
mpirun --prefix /usr/local/share/OpenMPI -np $(($cpus+1)) mm $n
rm -f mm

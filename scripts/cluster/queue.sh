#!/bin/bash

instances=$1
instances_escaped=$(sed 's;/;\\/;g' <<< "$instances")
mkdir results/"$instances"

for filename in $(ls "$instances")
do
	gunzip ./"$instances"/"$filename"
done

for filename in $(ls "$instances")
do
    sed -i "s/FILENAME/$filename/g" single.pbs
    sed -i "s/INSTANCES/$instances_escaped/g" single.pbs
    sbatch single.pbs
    sed -i "s/$filename/FILENAME/g" single.pbs
    sed -i "s/$instances_escaped/INSTANCES/g" single.pbs
done

#!/bin/bash

#SBATCH --job-name=CMS
#SBATCH --time=120:05:00
#SBATCH --ntasks=20
#SBATCH --partition=skylake_mpi
#SBATCH --mem-per-cpu=41960

module load CMake/3.20.1-GCCcore-10.3.0
module load Python/3.9.5-GCCcore-10.3.0

instances=$VSC_SCRATCH/problems2010
instances_escaped=$(sed 's;/;\\/;g' <<< "$instances")
mkdir running_scripts2010

for filename in $(ls "$instances")
do
	gunzip "$instances"/"$filename"
done

for filename in $(ls "$instances")
do
    sed "s/FILENAME/$filename/g" single.sh > running_scripts2010/${filename}.sh
    sed -i "s/INSTANCES/$instances_escaped/g" running_scripts2010/${filename}.sh
    chmod +x running_scripts2010/${filename}.sh
    srun -n1 --exclusive ./running_scripts2010/${filename}.sh
done
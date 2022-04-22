#!/bin/bash

wget https://www.cs.helsinki.fi/group/coreo/mse2021/mse2021_benchmarks/mse21_complete_unwt.zip
unzip mse21_complete_unwt.zip
mkdir problems2021
mv mse21_complete_unwt/* problems2021
rm -rf mse21_complete_unwt 

for family in problems2021/ ; do
   find "$family" -type f -exec sh -c 'new=$(echo "{}" | tr "/" "-" | tr " " "_"); mv "{}" problems2021/"$new"' \;
done
rm -rf problems2021/*/

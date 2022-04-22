#!/bin/bash

wget http://www.maxsat.udl.cat/10/benchs/pms_random.tgz
wget http://www.maxsat.udl.cat/10/benchs/pms_crafted.tgz
wget http://www.maxsat.udl.cat/10/benchs/pms_industrial.tgz

tar zxvf pms_random.tgz
tar zxvf pms_crafted.tgz
tar zxvf pms_industrial.tgz

mkdir problems2010
mv pms_random problems2010
mv pms_crafted problems2010
mv pms_industrial problems2010

rm pms_random.tgz
rm pms_crafted.tgz
rm pms_industrial.tgz

for family in problems2010/ ; do
   find "$family" -type f -exec sh -c 'new=$(echo "{}" | tr "/" "-" | tr " " "_"); mv "{}" problems2010/"$new"' \;
done
rm -rf problems2010/*/

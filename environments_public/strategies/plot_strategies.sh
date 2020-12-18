#!/bin/bash

prefix="tmp_"

rscript_path="plot_strategy.r"

if [[ $# -gt 1 ]]
then
    prefix=$1
fi

suffixes[0]="with_grass"
suffixes[1]="against_grass"

for suffix in ${suffixes[@]}
do
    Rscript ${rscript_path} ${prefix}${suffix}.csv
    mv test.png ${prefix}${suffix}.png
done

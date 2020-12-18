#!/bin/bash

BIN="${HOME}/workspace/devel_release/lib/kid_size/KickStrategy"

if [ $# -lt 1 ]; then
    echo "Usage: $0 [strategy.json]"
    exit
fi

$BIN -j ../common/kicks/sigmaban_plus_kicks.json -l $1 -g > /tmp/o
mkfifo /tmp/f
gnuplot < /tmp/f &
echo -ne "plot '/tmp/o' u 1:2:3 palette w l\nexit" > /tmp/p
cat /tmp/p - > /tmp/f


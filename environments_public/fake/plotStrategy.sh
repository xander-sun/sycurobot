#!/bin/bash

if [ $# -lt 1 ]; then
    echo "Usage: $0 [strategy.json]"
    exit
fi

./KickStrategy -l $1 -g > /tmp/o
mkfifo /tmp/f
gnuplot < /tmp/f &
echo -ne "plot '/tmp/o' u 1:2:3 palette w l\nexit" > /tmp/p
cat /tmp/p - > /tmp/f


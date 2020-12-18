#!/bin/bash

if [ "$#" -ne 1 ] && [ "$#" -ne 2 ]; then
    echo "Usage: prepare.sh robotName"
    echo "Or"
    echo "Usage: prepare.sh robotName pathToLogs"
    exit
fi
robot=$1
echo "Preparing the fake env for the robot '$robot'"
env=$(dirname `pwd`)
echo "env path is $env"

declare -a elements=("VCM.json" "sigmaban.urdf" "camera_calib.yml")

for i in "${elements[@]}"
do
    if ln -sf "$env/$robot/$i" .
    then
        echo "Success."
    else
        echo "Warning : $i ln -sf failed !!!!!!!!!!!!"
        exit
    fi
done

if [ "$robot" == "django" -o "$robot" == "mowgly" -o "$robot" == "chewbacca" ]; then
    echo "Linking to Sigmaban strategies"
    ln -sf ../common/kickStrategy_v1_with_grass.json kickStrategy_with_grass.json
    ln -sf ../common/kickStrategy_v1_counter_grass.json kickStrategy_counter_grass.json
#    ln -sf ../common/kicks/SigmabanKicks.xml KickModelCollection.xml #DEPRECATED
fi
if [ "$robot" == "tom" -o "$robot" == "olive" -o "$robot" == "arya" ]; then
    echo "Linking to Sigmaban V2 strategies"
    ln -sf ../common/kickStrategy_v2_with_grass.json kickStrategy_with_grass.json
    ln -sf ../common/kickStrategy_v2_counter_grass.json kickStrategy_counter_grass.json
    ln -sf ../common/kicks/sigmaban_plus_kicks.json KickModelCollection.json
fi

if [ "$#" -eq 2 ]; then
    echo "Setting the log path to '$2'"
    rm -rf workingLog
    ln -sf $2 workingLog
fi

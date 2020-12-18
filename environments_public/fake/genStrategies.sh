
echo "* Generating strategy for Sigmaban with grass"
./KickStrategy -j -o 180 -x ../common/kicks/SigmabanKicks.xml > ../common/kickStrategy_v1_with_grass.json

echo "* Generating strategy for Sigmaban against grass"
./KickStrategy -j -o 0 -x ../common/kicks/SigmabanKicks.xml > ../common/kickStrategy_v1_counter_grass.json

echo "* Generating strategy for Sigmaban V2 with grass"
./KickStrategy -j -o 180 -x ../common/kicks/SigmabanV2Kicks.xml > ../common/kickStrategy_v2_with_grass.json

echo "* Generating strategy for Sigmaban V2 against grass"
./KickStrategy -j -o 0 -x ../common/kicks/SigmabanV2Kicks.xml > ../common/kickStrategy_v2_counter_grass.json


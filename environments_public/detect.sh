killall -9 detect_crow_server
ulimit -c unlimited
ROBOT=`hostname`
cd $HOME/env/$ROBOT/
export LD_LIBRARY_PATH=$HOME/catkin_rel/
LOG_FOLDER=game_logs
mkdir -p ${LOG_FOLDER}
DATE=`detect_date "+%d_%m_%Y__%H_%M_%S"`
LOG_FILE=${LOG_FOLDER}/${DATE}.log
nohup $HOME/catkin_rel/detect_crow_server > ${LOG_FILE} 2>&1 &
ln -sf ${LOG_FILE} detect_out.log

tail -f detect_out.log

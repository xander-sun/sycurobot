killall -9 KidSize
ulimit -c unlimited
ROBOT=`hostname`
#ROBOT=`sycu3`
cd $HOME/env/sycu3/
export LD_LIBRARY_PATH=$HOME/catkin_rel/
LOG_FOLDER=game_logs
mkdir -p ${LOG_FOLDER}
DATE=`date "+%d_%m_%Y__%H_%M_%S"`
LOG_FILE=${LOG_FOLDER}/${DATE}.log
nohup $HOME/catkin_rel/KidSize > ${LOG_FILE} 2>&1 &
ln -sf ${LOG_FILE} out.log

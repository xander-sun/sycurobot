ROBOT=`hostname`
cd $HOME/env/$ROBOT/
## XXX: Temporary removed this because it batches the output
tail -f out.log #| grep -v ballStatusEntry

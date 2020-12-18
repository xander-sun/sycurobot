#!/bin/bash

remote="10.0.0.1"
if [ $# -gt 0 ]
then
    remote=$1
fi 

echo "Retrieving robot environment"
rsync --exclude core -l -r rhoban@${remote}:env/* .

echo "Checking git status"
git status


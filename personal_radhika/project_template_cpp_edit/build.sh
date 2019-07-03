#!/bin/bash

TARGET_BUILD_FOLDER=build

mkdir $TARGET_BUILD_FOLDER
cd $TARGET_BUILD_FOLDER

if cmake ..; then
	if make; then
 		./myProg
	else
		echo "error during make"
		exit 1
	fi 
else
	echo "error during cmake"
	exit 1
fi 
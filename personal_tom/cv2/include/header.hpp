#ifndef _HEADER_HPP_
#define _HEADER_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <pthread.h>
#include <mutex>
#include <unistd.h>
#include <cstdlib>
#include <cmath>
#include <math.h>

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

#define IMAGEWIDTH 	320
#define IMAGEHEIGHT 200

#define PIXELTHRESH 300
#define SOLID 1
#define DOTTED 2
#define STOP 3
#define CROSSING 4

#define STRAIGHT 0
#define LEFTTURN 1
#define RIGHTTURN 2

#define FONTTHICKNESS 1
#define FONTSIZE .5

#define STOPWAITTIME 1 // in seconds
#define VMIN 0.5
#define VMAX 1
#define VMAXCROSSING 0.7

void MyImshow(std::string name, cv::Mat* img,int wait);
void SetRealtimePriority();
void SetRRPriority();
void SuspendThread();
void ResumeThread();
void CheckSuspendThread();
void LockImshow();
void UnlockImshow();
void SetControlMode(bool new_mode);
bool GetControlMode();
void SetMotorRun(bool new_motor_run);
bool GetMotorRun();
#endif
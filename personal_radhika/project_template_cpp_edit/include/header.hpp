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

// void MyImshow(std::string name, cv::Mat* img,int wait);
// void SetRealtimePriority();
// void SetRRPriority();
void SuspendThread();
void ResumeThread();
void CheckSuspendThread();
// void LockImshow();
// void UnlockImshow();
#endif
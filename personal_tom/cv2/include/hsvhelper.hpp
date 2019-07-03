#ifndef _HSVHELPER_HPP_
#define _HSVHELPER_HPP_

#include "header.hpp"

static void OnMouse(int event, int x, int y, int f, void* );
void FindHSVValues(cv::Mat*, cv::Mat*);
void SetupHSVWindows();

#endif
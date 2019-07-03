#ifndef _CLIENT_HPP_
#define _CLIENT_HPP_

#include "header.hpp"
#include "base64.hpp"

void* InterfaceRun(void* arg);
void ResizeImage(cv::Mat* src);
void SendImage(cv::Mat* new_image);

#endif
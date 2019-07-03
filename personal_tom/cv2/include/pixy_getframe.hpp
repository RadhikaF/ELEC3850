#ifndef _GET_FRAME_HPP_
#define _GET_FRAME_HPP_

#define FRAME_WIDTH 320
#define FRAME_HEIGHT 200

#include "header.hpp"
#include "drawmarker.hpp"

class PixyFrame{
public:
	// unsigned char frame[70000]; //largest possible given hardware
	uint8_t gain;
	uint16_t compensation, width, height;
	uint32_t  numPixels;
	int return_value;
	unsigned char* internal_buffer;
	unsigned char frame_check[FRAME_HEIGHT];
	bool new_frame = false;
	cv::Mat img_rgb;

	void pixy_get_ECV();
	void get_frame();
};

int pixy_stop(int32_t* response);
int pixy_setup();
void pixy_quit();
void handle_SIGINT(int unused);
void* PixyRun(void* arg);
void set_new_frame(bool new_frame);
bool get_new_frame();
void set_img_rgb(cv::Mat* img_rgb_new);
cv::Mat get_img_rgb();
void* SetupPixycam();
void DisplayProcessedImage(cv::Mat* src);
void AddDrawMarker(DrawMarker new_marker);
void ClearDrawMarkers();

#endif
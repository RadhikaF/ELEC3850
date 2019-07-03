#ifndef _DRAWMARKER_HPP_
#define _DRAWMARKER_HPP_

#include "header.hpp"

#define DRAWMARKERS 1

#define CIRCLEMARKER 0
#define LINEMARKER 1
#define RECTMARKER 2
#define TEXTMARKER 3

class DrawMarker{
public:
	int type;
	bool show_text = false;
	std::string text;
	cv::Point p0;
	cv::Point p1;
	cv::Scalar colour;

	DrawMarker(int type, std::string, cv::Scalar,cv::Point,bool);					// DrawMarker contructor with name
	void SetP1(cv::Point);
};

#endif
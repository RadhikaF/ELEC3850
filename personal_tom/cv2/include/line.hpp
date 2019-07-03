#ifndef _LINE_HPP_
#define _LINE_HPP_

#include "header.hpp"

class Line{
public:
	std::string name;
	bool exists = false;
	int type = SOLID;
	double angle = 0;
	double length = 0;
	int x;
	int y;

	cv::Point min_point;
	cv::Point max_point;

	std::vector <cv::Vec4i> coords;
	std::vector <double> a;
	std::vector <double> l;

	Line(std:: string);					// line contructor with name
	void CheckExists();					// checks wether there are any lines in coords
	void AddLine(cv::Vec4i* line);		// add a set of line coords to the line
	void CalcAngle();					// calculate average gradient and individual gradients
	void CalcLength();					// calculate average length and individual lengths
	void CalcX();						// calculates average x position
	void CalcY();						// calculates average y position
	bool Cluster();						// does all above calculations together
	void PrintAngles();					// print vector of gradients
	void PrintLengths();				// print vector of line lengths
	void PrintCoords();					// print vector of coords
	void CalcMinPoint();			// calculates minimum point on line
	void CalcMaxPoint();			// calculates maximum point on line
};

#endif
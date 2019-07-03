#ifndef _IMAGEFUNCTIONS_HPP_
#define _IMAGEFUNCTIONS_HPP_

#include "header.hpp"
#include "line.hpp"

#define VERTLINEA 60	// Pi/4
#define HORZLINEA 45	// Pi/6

#define DOTTEDLINELENTHRESH 20		// used to determine dotted or solid lines
#define STOPLINELENTHRESH 55 	// used to determine stop lines or crossings

void ResizeImage(cv::Mat*);
void MaskHSV(cv::Mat* src, cv::Mat* dst, const std::vector <cv::Scalar>);
void CreateWindow(char* window_name, int x, int y);
void MaskRegion(cv::Mat* src, cv::Mat* dst, const std::vector <cv::Point> region);
void DrawLines(cv::Mat* dst, std::vector <cv::Vec4i>* lines);
void FindLines(cv::Mat* src,std::vector <cv::Vec4i>* lines, std::vector <int>* line_params);
void Blur(cv::Mat* src, cv::Mat* dst);
bool FindColour(cv::Mat*, cv::Mat*, std::vector <cv::Scalar>, std::vector <cv::Point>, bool inverse);
int FindLanes(cv::Mat*, Line*, Line*, Line*);
void SplitLane(Line*, Line*, Line*);
void ExamineMarking(cv::Mat* canny_img, Line* marking_line);
int CheckIntersection(cv::Mat* src, std::vector <cv::Scalar> colour, std::vector <bool>* options);
void IncrementLeftHist(int new_line_hist);
int CheckLeftHist();
void IncrementRightHist(int new_line_hist);
int CheckRightHist();
void MaskInvRegion(cv::Mat* src, cv::Mat* dst, const std::vector <cv::Point> region);
#endif
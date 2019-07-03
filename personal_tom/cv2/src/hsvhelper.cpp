/*
    File name:          hsvhelper.cpp
    Version:            1.1
    Author:             Tom Bailey
    Date created:       2018-08-19
    Date last modified: 2018-08-22

    Description:
    Displays the HSV and RGB values of a given image at the mouse location

Code Reference: http://answers.opencv.org/question/30547/need-to-know-the-hsv-value/
*/

#include "header.hpp"
#include "hsvhelper.hpp"
#include "imagefunctions.hpp"
// const vector <Scalar> yellow = {Scalar(75,10,135),Scalar(105,163,255)};        // track

using namespace std;
using namespace cv;

const string window_original = "Original";
const string window_masked = "HSV Helper";
const string window_lines = "Lines found";
const string trackbar_window = "Trackbar";


// static int hmin=0,hmax=255,smin=0,smax=255,vmin=0,vmax=255,threshold_val=50, line_len=50, line_gap=50;
// static int hmin=90,hmax=105,smin=200,smax=255,vmin=180,vmax=255;		// yellow test
// static int hmin=110,hmax=130,smin=150,smax=255,vmin=100,vmax=255;	// red test

// const vector <Scalar> red = {Scalar(110,113,185),Scalar(120,185,255)};          // track

// static int hmin=75,hmax=105,smin=10,smax=163,vmin=135,vmax=255;		// yellow track
static int hmin=75,hmax=105,smin=10,smax=163,vmin=135,vmax=255;	// red track

static int windows_setup = 0;

// mask constants
// const vector <Point> vision_mask = {Point(0,200),Point(0,50),Point(140,190),Point(180,190),Point(320,50),Point(320,200)};
// const vector <Point> vision_mask = {Point(0,0),Point(0,50),Point(140,190),Point(180,190),Point(320,50),Point(320,0)};
// const vector <Point> vision_mask = {Point(0,200),Point(20,180),Point(60,150),Point(260,150),Point(300,180),Point(320,200)};
// const vector <Point> vision_mask = {Point(320,50),Point(180,190),Point(140,190),Point(0,50),Point(0,200),Point(320,200),Point(320,50)};
const vector <Point> vision_mask = {Point(0,0),Point(0,130),Point(140,190),Point(180,190),Point(320,100),Point(320,0)};

vector <Scalar> colour;

vector <int> line_params = {22,34,14};

Mat img_masked, img_canny;
Mat* img_srcP;

void SetupHSVWindows(){
	// window creation and setup
	LockImshow();
	namedWindow(window_masked,WINDOW_NORMAL);
	resizeWindow(window_masked,IMAGEWIDTH,IMAGEHEIGHT);
	moveWindow(window_masked,100,350);

	namedWindow(trackbar_window,WINDOW_NORMAL);
	resizeWindow(trackbar_window,500,600);
	moveWindow(trackbar_window,450,350);

	namedWindow(window_original,WINDOW_NORMAL);
	resizeWindow(window_original,IMAGEWIDTH,IMAGEHEIGHT);
	moveWindow(window_original,100,50);

	namedWindow(window_lines,WINDOW_NORMAL);
	resizeWindow(window_lines,IMAGEWIDTH,IMAGEHEIGHT);
	moveWindow(window_lines,100,650);

	// trackbars to change HSV values
	createTrackbar("hmin", trackbar_window, &hmin, 255);
	createTrackbar("hmax", trackbar_window, &hmax, 255);
	createTrackbar("smin", trackbar_window, &smin, 255);
	createTrackbar("smax", trackbar_window, &smax, 255);
	createTrackbar("vmin", trackbar_window, &vmin, 255);
	createTrackbar("vmax", trackbar_window, &vmax, 255);
	createTrackbar("line len", trackbar_window, &(line_params[0]), 50);
	createTrackbar("line gap", trackbar_window, &(line_params[1]), 50);
	createTrackbar("threshold", trackbar_window, &(line_params[2]), 50);
	windows_setup=1;
	UnlockImshow();
}

void FindHSVValues(Mat* img_src, Mat* img_hsv){
	img_srcP = img_src;
	if(!windows_setup) SetupHSVWindows();
	clock_t start = clock();

 	// -------------- Find edges of lines -------------
	MaskHSV(img_hsv, &img_masked,{Scalar(hmin,smin,vmin), Scalar(hmax,smax,vmax)});
    GaussianBlur(img_masked, img_canny, Size(5,5), 0, 0);      						// Gaussian blur to limit noise    								
 	Canny(img_canny, img_canny, 200, 300);
 	MaskInvRegion(&img_canny, &img_canny, vision_mask);



 	// -------------- Find Yellow Lines ---------------
 	vector <Vec4i> lines;
 	FindLines(&img_canny, &lines, &line_params);

 	// -------------- Draw lines on image --------------
 	DrawLines(&img_canny, &lines); 

 	// --------------- Calculate FPS -------------------
    double fps = double(CLOCKS_PER_SEC)/(clock()-start);
    char name[30];
	sprintf(name,"FPS: %.1lf",fps);
	putText(img_canny,name, Point(5,15) , FONT_HERSHEY_SIMPLEX, FONTSIZE, Scalar(255), FONTTHICKNESS,8,false );

	// putText(*img_src,name, Point(5,15) , FONT_HERSHEY_SIMPLEX, FONTSIZE, Scalar(255), FONTTHICKNESS,8,false );

	// --------------- Display images -----------------
	MyImshow(window_original, img_src, 1);									// show the original image
	MyImshow(window_masked, &img_masked, 1);								// show the masked image
    MyImshow(window_lines,&img_canny, 1);									// show the image with lines

	
	setMouseCallback( window_original, OnMouse, 0 );						// click left mouse to pring HSV value
}

static void OnMouse(int event, int x, int y, int f, void*){
	if(event == CV_EVENT_LBUTTONDOWN){					// when left mouse is clicked
		if((x<(*img_srcP).cols)&(x>0)&(y<(*img_srcP).rows)&(y>0)){
			Mat RGB=(*img_srcP)(Rect(x,y,1,1));				// take the pixel where the mouse is
			Mat HSV;
			cvtColor(RGB, HSV,COLOR_RGB2HSV);			// convert to HSV

			Vec3b hsv=HSV.at<Vec3b>(0,0);				// separate layers and display HSV values
			int H=hsv.val[0];
			int S=hsv.val[1];
			int V=hsv.val[2];

			cout << "H: " << H << "    S: " << S << "    V: " << V << endl;
		}
	}
}
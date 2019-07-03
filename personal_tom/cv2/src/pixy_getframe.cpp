#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string.h>

#include "pixy.h"
#include "pixy_getframe.hpp"
#include "motorcontrol.hpp"
#include "drawmarker.hpp"
#include "ELEC3850_client.hpp"

using namespace std;
using namespace cv;

// camera mutex
static std::mutex img_rgb_mutex;
static std::mutex new_frame_mutex;

static Mat img_raw, img_temp;

const string win_name = "Processed Img";

static PixyFrame frame;

static bool pixy_is_setup = false;

void set_new_frame(bool new_frame){
	lock_guard <mutex> lock(new_frame_mutex);
	frame.new_frame = new_frame;
	return;
}

bool get_new_frame(){
	lock_guard <mutex> lock(new_frame_mutex);
	return frame.new_frame;
}

void set_img_rgb(cv::Mat* img_rgb_new){
	lock_guard <mutex> lock(img_rgb_mutex);
	frame.img_rgb = *img_rgb_new;
	set_new_frame(true);
	return;
}

Mat get_img_rgb(){
	lock_guard <mutex> lock(img_rgb_mutex);
	return frame.img_rgb;
}

void PixyFrame::pixy_get_ECV(){
	// get exposure
    return_value = pixy_cam_get_exposure_compensation(&gain, &compensation);
    printf("getECV returned %d values: 0, 0x%02x, 0x%04x\n",return_value,gain, compensation);
}

void PixyFrame::get_frame(){
	// setup variables
	int32_t response, fourcc;
	int8_t renderflags;

	// printf("Getting frame from pixy...\n");
    return_value = pixy_command("cam_getFrame",  	// String id for remote procedure
    							0x01, 	0x21,      	// mode 0 = 1280x800
                                0x02,   0,        	// xoffset
                                0x02,   0,         	// yoffset
                                0x02,  	FRAME_WIDTH,      	// width
                                0x02,  	FRAME_HEIGHT,       	// height (56 max @ 1280 w)
                                0,              	// separator
                                &response,      	// pointer to mem address for return value
		                    	&fourcc,
		                    	&renderflags,
		                     	&width,
		                     	&height,
		                     	&numPixels,
                                &internal_buffer,   // pointer to mem address for returned frame
                                0);
   	
}
int pixy_stop(int32_t* response){
	int return_value;
	// stop the pixycam
	return_value = pixy_command("stop", END_OUT_ARGS, response, END_IN_ARGS);
    printf("pixy_stop returned %d response %d\n", return_value, *response);
    return return_value;
}

int pixy_setup(){
	// setup variables
	int return_value;

	// initialize pixycam
	return_value = pixy_init();
	// printf("initialized Pixy response %d\n", return_value);

	if(return_value != 0){
	    // Error initializing Pixy //
	    printf("pixy_init(): ");
	    pixy_error(return_value);
	}

    return return_value;
}

void pixy_quit(){

	pixy_close();
}

void handle_SIGINT(int unused)
{
  // On CTRL+C - abort //
	SetMotorRun(0);
  StopMotors();
  printf("\nBye!\n");
  exit(0);
}

void* SetupPixycam(){
	if(!pixy_is_setup){
		cout << "setting up pixycam" << endl;
		int32_t response;
	    double duration;

	    // ================ Setup Pixycam ======================
	    pixy_setup();
	    pixy_stop(&response);
	    frame.pixy_get_ECV();
	    frame.get_frame();
	    img_raw = Mat(frame.height,frame.width,CV_8UC1);  
    	img_temp = Mat(frame.height,frame.width,CV_8UC3);
	    memcpy(img_raw.data, frame.internal_buffer, frame.numPixels);
        cvtColor(img_raw,img_temp,COLOR_BayerBG2RGB,3);
        set_img_rgb(&img_temp);

        LockImshow();
        namedWindow(win_name,WINDOW_NORMAL);
		resizeWindow(win_name,frame.width,frame.height);
		moveWindow(win_name,450,50);
		UnlockImshow();

	    pixy_is_setup = true;
	}
}

void* PixyRun(void* arg){
	// SetRealtimePriority();
	if(!pixy_is_setup){
		cout << "pixycam is not setup" << endl;
		exit(0);
	}
	cout << "hello from PixyRun" << endl;

    set_img_rgb(&img_temp);

    clock_t start = clock();
    double fps = 30;
    int sleep_time = 15000;
    static int count = 0;

    while(1){
    	SuspendThread();
    	frame.get_frame();
        memcpy(img_raw.data, frame.internal_buffer, frame.numPixels);
        cvtColor(img_raw,img_temp,COLOR_BayerBG2RGB,3);
        set_img_rgb(&img_temp);
        // --------------- Calculate FPS -------------------
        double fps = double(CLOCKS_PER_SEC)/(clock()-start);
        start = clock();
        char fps_array[30];
        sprintf(fps_array,"FPS: %.1lf",fps);
        putText(img_temp,fps_array, Point(5,15) , FONT_HERSHEY_SIMPLEX, FONTSIZE, Scalar(255,255,255), FONTTHICKNESS,8,false );
        DisplayProcessedImage(&img_temp);
        SendImage(&img_temp);

    	if(fps<40){
    		if(sleep_time>0){sleep_time--;}
    	} else {
    		sleep_time++;
    	}

    	ResumeThread();
        usleep(sleep_time);

    }
    pthread_exit(NULL);
}

pthread_mutex_t marker_mutex = PTHREAD_MUTEX_INITIALIZER;
const double font_size = .5;
const Scalar yellow = Scalar(0,255,0);
static vector <DrawMarker> markers;

void DisplayProcessedImage(Mat* src){
	if(DRAWMARKERS){
		pthread_mutex_lock(&marker_mutex);
		for(int i=0; i<markers.size(); i++){
			if(markers[i].show_text){
				if(markers[i].p0.x>(IMAGEWIDTH/2.0)){
					putText(*src,markers[i].text, Point(markers[i].p0.x-60,markers[i].p0.y) , FONT_HERSHEY_SIMPLEX, font_size, markers[i].colour, FONTTHICKNESS,8,false );
				} else {
					putText(*src,markers[i].text, Point(markers[i].p0.x+10,markers[i].p0.y) , FONT_HERSHEY_SIMPLEX, font_size, markers[i].colour, FONTTHICKNESS,8,false );
				}
			}
			if(markers[i].type==CIRCLEMARKER){
				circle(*src,markers[i].p0,5,markers[i].colour,1,8,0);	
			} else if(markers[i].type==LINEMARKER){
				line(*src,markers[i].p0, markers[i].p1, markers[i].colour,1,8,0);	
			} else if(markers[i].type==RECTMARKER){
				rectangle(*src,markers[i].p0, markers[i].p1,markers[i].colour,1,8,0);	
			}
		}
		pthread_mutex_unlock(&marker_mutex);
	}
	MyImshow(win_name, src, 30);
}

void AddDrawMarker(DrawMarker new_marker){
	if(DRAWMARKERS){
		pthread_mutex_lock(&marker_mutex);
		markers.push_back(new_marker);
		pthread_mutex_unlock(&marker_mutex);
	}
}

void ClearDrawMarkers(){
	if(DRAWMARKERS){
		pthread_mutex_lock(&marker_mutex);
		markers.clear();
		pthread_mutex_unlock(&marker_mutex);
	}
}
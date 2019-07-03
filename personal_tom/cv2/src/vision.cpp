/*
    File name:          vision.cpp
    Version:            0.1
    Author:             Tom Bailey
    Date created:       2018-08-21
    Date last modified: 2018-08-21

    Description:

*/
#include <numeric>

#include "header.hpp"
#include "pixy_getframe.hpp"
#include "vision.hpp"
#include "hsvhelper.hpp"
#include "imagefunctions.hpp"
#include "motorcontrol.hpp"
#include "line.hpp"
#include "control.hpp"
#include "drawmarker.hpp"

using namespace std;
using namespace cv;

bool using_pixy = true;
bool using_hsvhelper = false;

int state = DRIVING;
int lane_state = LEFTLANE;

vector <bool> intersection_options(3);

const string resources_dir = "../resources/";
vector <string> file_list;

static double last_stop_angle;
static bool stop_hold = false;
 
// colour constants
const vector <Scalar> yellow = {Scalar(75,10,176),Scalar(105,190,255)};        // track
const vector <Scalar> red = {Scalar(110,113,185),Scalar(120,185,255)};          // track

// const vector <Scalar> yellow = {Scalar(90,200,180),Scalar(105,255,255)};        // test image
// const vector <Scalar> red = {Scalar(110,150,100),Scalar(130,255,255)};          // test image

// const vector <Scalar> yellow = {Scalar(73,79,48),Scalar(84,147,140)};        // @home
// const vector <Scalar> red = {Scalar(89,40,53),Scalar(142,163,128)};          // @home 

// mask constants
// const vector <Point> vision_mask = {Point(0,200),Point(0,160),Point(120,100),Point(200,120),Point(300,160),Point(300,200)}; // old mask
const vector <Point> vision_mask_reg = {Point(0,200),Point(10,170),Point(40,100),Point(280,100),Point(310,170),Point(320,200)};
const vector <Point> vision_mask = {Point(0,0),Point(0,130),Point(140,190),Point(180,190),Point(320,100),Point(320,0)};

void* Vision(void* arg){
    // SetRRPriority();
    cout << "hello from vision" << endl;
    Mat img_raw, img_rgb, img_hsv, img_yellow_canny, img_red_canny, img_temp;

    if (!using_pixy){
        // ---------------- From File -----------------
        img_rgb = imread((resources_dir+"saved_img/close_left_line.jpg").c_str());              // read test image
        ResizeImage(&img_rgb);                                                      // resize to 300 x 240
    }                                 

    clock_t start = clock();

    while(1){
        if(DRAWMARKERS){usleep(100000);}  // just so you can see image markers
        CheckSuspendThread();   // -- suspend the active thread if needed -- //
              
        // =======================================================================================//
        // ====================================== Get Image ======================================//
        // ---------------- From Pixycam -----------------
        if (using_pixy && get_new_frame()){
            ClearDrawMarkers();
            img_rgb = get_img_rgb();
            // imwrite( (resources_dir+"saved_img/1.jpg"), img_rgb );
            // exit(0);
        } else {
            img_temp = img_rgb.clone();
            DisplayProcessedImage(&img_temp);
            ClearDrawMarkers();
        }

        // --------------- Calculate FPS -------------------
        double fps = double(CLOCKS_PER_SEC)/(clock()-start);
        start = clock();
        char fps_array[30];
        sprintf(fps_array,"PFPS: %.1lf",fps);
        DrawMarker fpsMarker(TEXTMARKER, fps_array, Scalar(255,255,255), Point(290,15), true);
        AddDrawMarker(fpsMarker);

        // ============== Image Processing ================
        // --------------- Convert to HSV -----------------
        cvtColor(img_rgb,img_hsv,COLOR_RGB2HSV,CV_8UC3);            // convert image to HSV from RGB

        // =============== HSV Helper ======================
        if (using_hsvhelper){ 
            FindHSVValues(&img_rgb, &img_hsv);                      // find HSV values
            continue;
        }

        // =======================================================================================//
        // ======================================= Driving =======================================//
        if(state==DRIVING){
            Line lane("lane lines"), right_line("right line"), left_line("left line"), marking_line("marking_line");
            SetVmax(VMAX);  // max speed allowed before any lines found
            // cout << "red lines:" << endl;
            if (FindColour(&img_hsv, &img_red_canny, red, vision_mask_reg, false)){ // look for a red line
                ExamineMarking(&img_red_canny, &marking_line);
                if((marking_line.type == STOP)&&(GetControlMode())&&(!stop_hold)){
                    cout << "you are coming to a stop" << endl;
                    last_stop_angle = marking_line.angle;
                    // do stop things
                    stop_hold = true;
                    SetMotorCtrl({0,0});    // stop
                    usleep(1000000*STOPWAITTIME);        // wait for at least 1 second
                    state=STOPPED;
                    continue;
                } else if(marking_line.type == CROSSING){
                    // do crossing things
                    SetVmax(VMAXCROSSING);              // slow down if there is a crossing
                }
            } else {
                if(stop_hold){
                    stop_hold=false;
                }
            }
            // cout << "yellow lines:" << endl;
            if (FindColour(&img_hsv, &img_yellow_canny, yellow, vision_mask, true)){              
                if(FindLanes(&img_yellow_canny, &lane, &right_line, &left_line)==0){
                   // cout<<"left angle: " << left_line.angle << endl;
                   // cout <<"right angle: "<< right_line.angle<<endl;

                    if(left_line.exists){
                        string line_type = "solid";
                        if(left_line.type==DOTTED){
                            line_type = "dotted";
                        }
                        DrawMarker leftLaneMarker(CIRCLEMARKER, line_type, Scalar(255,255,255), Point(left_line.x,left_line.y), true);
                        AddDrawMarker(leftLaneMarker);
                    }
                    if(right_line.exists){
                        string line_type = "solid";
                        if(right_line.type==DOTTED){
                            line_type = "dotted";
                        }
                        DrawMarker rightLaneMarker(CIRCLEMARKER, line_type, Scalar(255,255,255), Point(right_line.x,right_line.y), true);
                        AddDrawMarker(rightLaneMarker);
                    }
                    if(GetControlMode()){
                        if(CalcErrorSignal(&left_line, &right_line, lane_state)==0){
                            PID();
                            continue;
                        } else { 
                            continue; 
                        }    // the lines presented to control were not logical
                    }
                } else {
                    // ------ test ------ if no lines are found, don't use the previous lines for error calc
                        // NewError(0);
                    // ------------------
                    if(GetControlMode()){
                        NewError(0);
                        PID();
                        continue;
                    }
                } // there were no lane lines found in the image
            }

        }
        // =======================================================================================//
        // ======================================= Stopped =======================================//
        if(state==STOPPED){
            if(CheckIntersection(&img_hsv, yellow, &intersection_options)==0){
                int random_option_loc = ChooseIntersectionDirection(&intersection_options);
                if(random_option_loc==-1){
                    cout << "there are no valid intersection options" << endl;
                    continue;
                } else if(random_option_loc==STRAIGHT){
                    cout << "you have chosen to go straight" << endl;
                } else if(random_option_loc==LEFTTURN){
                    cout << "you have chosen to go left" << endl;
                } else if(random_option_loc==RIGHTTURN){
                    cout << "you have chosen to go right" << endl;
                }
                usleep(1000000*STOPWAITTIME);
                // StraightenUp(last_stop_angle);
                // CheckTrafficLights();
                state=DRIVING;
            } else {
                usleep(1000000*STOPWAITTIME);
                state=DRIVING;  // remove this eventually, just leave in while it can't move
                continue; // no valid intersection options were found, scan again
            }
        }    
	}
	pthread_exit(NULL);
}
/*
    File name:          imagefunctions.cpp
    Version:            0.1
    Author:             Tom Bailey
    Date created:       2018-08-19
    Date last modified: 2018-08-20

    Description:

Code Reference: http://www.pieter-jan.com/node/5
Code Reference: https://docs.opencv.org/2.4/modules/imgproc/doc/feature_detection.html?highlight=houghlines#houghlines
*/

#include "header.hpp"
#include "imagefunctions.hpp"
#include "line.hpp"
#include "drawmarker.hpp"
#include "pixy_getframe.hpp"

using namespace std;
using namespace cv;

#define CENTREGAP 10
#define HISTSIZE 3

// vector <int> solid = {25,4,20};
// vector <int> dotted = {25,40,20};
// vector <int> dotted = {5,10,20};            // test image
// vector <int> dotted = {7,5,20};            // test track
vector <int> dotted = {20,60,10};            // test track
vector <int> stop = {30,8,20};
// vector <int> crossing = {25,40,20};
// vector <int> crossing = {10,10,20};         // test image
vector <int> crossing = {5,22,15};         // test track

int left_line_hist [HISTSIZE];
int right_line_hist [HISTSIZE];

int line_len = 30;
int line_gap = 35;
int threshold_val = 20;

const int solid_pixel_thresh = 1000;

static int left_pixels;
static int right_pixels;

// mask constants
const vector <Point> top_mask = {Point(0,0),Point(0,60),Point(120,100),Point(200,100),Point(320,60),Point(320,0)};
const vector <Point> left_mask = {Point(0,0),Point(60,0),Point(120,75),Point(120,125),Point(60,200),Point(0,200)};
const vector <Point> right_mask = {Point(320,0),Point(290,0),Point(250,50),Point(260,135),Point(280,200),Point(320,200)};

void IncrementLeftHist(int new_line_hist){
    for(int i=HISTSIZE;i>0;i--){
        if(i==1){
            left_line_hist[i-1] = new_line_hist;
        } else {
            left_line_hist[i-1] = left_line_hist[i-2];
        }        
    }
}
int CheckLeftHist(){
    int solid_count=0;
    int dotted_count=0;
    for(int i=0;i<HISTSIZE;i++){
        if(left_line_hist[i] == SOLID){
            solid_count++;
        } else if(left_line_hist[i]==DOTTED){
            dotted_count++;
        }
    }
    if(solid_count>dotted_count){
        return SOLID;
    } else if(dotted_count>solid_count){
        return DOTTED;
    } else {
        return left_line_hist[0];
    }
}
void IncrementRightHist(int new_line_hist){
    for(int i=HISTSIZE;i>0;i--){
        if(i==1){
            right_line_hist[i-1] = new_line_hist;
        } else {
            right_line_hist[i-1] = right_line_hist[i-2];
        }        
    }
}
int CheckRightHist(){
    int solid_count=0;
    int dotted_count=0;
    for(int i=0;i<HISTSIZE;i++){
        if(right_line_hist[i] == SOLID){
            solid_count++;
        } else if(right_line_hist[i]==DOTTED){
            dotted_count++;
        }
    }
    if(solid_count>dotted_count){
        return SOLID;
    } else if(dotted_count>solid_count){
        return DOTTED;
    } else {
        return right_line_hist[0];
    }
}

void ResizeImage(Mat* src){
	resize(*src, *src, Size(320,200), 0, 0, INTER_NEAREST); // resize an image to standard size
}

void MaskHSV(Mat* src, Mat* dst, const vector <Scalar> colour){
    CheckSuspendThread();   // -- suspend the active thread if needed -- //
	Mat mask((*src).size(), CV_8U);                    // 8bit 1 channel mask matrix of zeros
    // Mat temp;
	// cvtColor(*src,temp,COLOR_RGB2HSV,CV_8UC3);         // convert to HSV from BGR
	inRange(*src,colour[0],colour[1],mask);            // mask contains 255 where colour is in range
	*dst = mask.clone();                               // copy the result to the destination
}

void CreateWindow(char* window_name, int x, int y){
    namedWindow(window_name,cv::WINDOW_NORMAL);         // create a new, resizeable window
    resizeWindow(window_name,IMAGEWIDTH,IMAGEHEIGHT);   // resize the window to the standard image size
    moveWindow(window_name,x,y);                        // position the window on the screen
}

void MaskRegion(Mat* src, Mat* dst, const vector <Point> region){
     CheckSuspendThread();   // -- suspend the active thread if needed -- //
    Mat mask = Mat::zeros((*src).size(), CV_8U);    // create empy matrix for mask
    Mat temp = *src;                                // store a temporary copy of the original
    *dst = mask.clone();                            // ensure dst starts of empty

    fillConvexPoly(mask,&region[0],region.size(),255,8,0);  // fill polygon with 255's
    temp.copyTo(*dst,mask);                         // copy the masked image to dst
}

void MaskInvRegion(Mat* src, Mat* dst, const vector <Point> region){
     CheckSuspendThread();   // -- suspend the active thread if needed -- //
    Mat mask_empty = Mat::zeros((*src).size(), CV_8U);    // create empy matrix for mask
    Mat mask((*src).size(),CV_8U, Scalar(255));   // create matrix of ones
    Mat temp = *src;                                // store a temporary copy of the original
    *dst = mask_empty.clone();                            // ensure dst starts of empty

    fillConvexPoly(mask,&region[0],region.size(),0,8,0);  // fill polygon with 0's
    temp.copyTo(*dst,mask);                         // copy the masked image to dst
}

void DrawLines(Mat* dst, vector <Vec4i>* lines){
    CheckSuspendThread();   // -- suspend the active thread if needed -- //
    *dst = Scalar(0);
    for(size_t i = 0; i < (*lines).size(); i++ )
    {
        line(*dst, Point((*lines)[i][0], (*lines)[i][1]),         // draw lines on the image
            Point((*lines)[i][2], (*lines)[i][3]), Scalar(255), 3, 8 );
    }
}

void FindLines(Mat* src, vector <Vec4i>* lines, std::vector <int>* line_params){
    CheckSuspendThread();   // -- suspend the active thread if needed -- //    
    HoughLinesP(*src, *lines, 1, CV_PI/180,(*line_params)[2],(*line_params)[0],(*line_params)[1]);        	// find lines
    
}

void Blur(Mat* src, Mat* dst){
    GaussianBlur(*src, *dst, Size(3,3), 0, 0);      // Gaussian blur
}

bool FindColour(Mat* src, Mat* dst, vector <Scalar> colour, vector <Point> vision_mask, bool inverse){
    Mat left, right;

    CheckSuspendThread();   // -- suspend the active thread if needed -- //
    // ---------- Find edges of yellow lines ----------
    MaskHSV(src, dst, colour);                      // create a mask from hsv based on colour

    int pixel_count = countNonZero(*dst);           // count how many pixels remain in the mask
    // cout << "pixels: " << pixel_count <<endl;
    if(pixel_count<PIXELTHRESH){
        left_pixels = 0;
        right_pixels = 0;
        return false;
    }      // skip if none of that colour found
    MaskRegion(dst, &left, left_mask);
    MaskRegion(dst, &right, right_mask);
    left_pixels = countNonZero(left);
    right_pixels = countNonZero(right);

    // cout << "left pixels: " << left_pixels << endl;
    // cout << "right pixels: " << right_pixels << endl;


    GaussianBlur(*dst, *dst, Size(5,5), 0, 0);      // Gaussian blur to limit noise                                 
    CheckSuspendThread();   // -- suspend the active thread if needed -- //
    Canny(*dst, *dst, 200, 300);
    if(inverse){
        MaskInvRegion(dst, dst, vision_mask);
    } else {
        MaskRegion(dst, dst, vision_mask);
    }
    return true;
}

int FindLanes(Mat* canny_img, Line* lane, Line* right_lane, Line* left_lane){
    CheckSuspendThread();   // -- suspend the active thread if needed -- //
    Line lane_temp("lane_temp"), left_temp("left_temp"), right_temp("right_temp");
    FindLines(canny_img, &((*lane).coords), &dotted);    // find solid and dotted lines in the image
    (*lane).CheckExists();



//     Mat dst, top, left, right;
//  MaskRegion(&dst, &left, left_mask);
// MaskRegion(&dst, &right, right_mask);
//     int pixel_count = countNonZero(dst);           // count how many pixels remain in the mask


    if((*lane).exists){
        SplitLane(lane, right_lane, left_lane);
        // ------------------- PIXEL THRESH TESTING NEW ------------------------- //
        // cout << "\nleft " << (*left_lane).length << endl;
        // cout << "right " << (*right_lane).length << "\n" << endl;
        // usleep(3000000);

        if((*left_lane).exists && (*right_lane).exists){         // both lines found
            // cout << "left l: " << (*left_lane).length << endl;
            // cout << "right l: " << (*right_lane).length << endl;
            if(left_pixels>=right_pixels){          // dotted line on the left
                (*left_lane).type = SOLID;
                (*right_lane).type = DOTTED;
                // cout << "left line is the solid line\n";
                // cout << "right line is the dotted line\n";
                // return 0;
            } else{
                (*left_lane).type = DOTTED;
                (*right_lane).type = SOLID;
                // cout << "right line is the solid line\n";
                // cout << "left line is the dotted line\n";
                // return 0;
            }
            IncrementLeftHist((*left_lane).type);
            if(CheckLeftHist()!=(*left_lane).type){
                return 1;
            }
            IncrementRightHist((*right_lane).type);
            if(CheckRightHist()!=(*right_lane).type){
                return 1;
            }
            if(CheckLeftHist()==CheckRightHist()){
                return 1;
            }
            return 0;
        } else if((*left_lane).exists){
             if(left_pixels>=solid_pixel_thresh){
                (*left_lane).type = SOLID;
                // cout << "left line l: "<<(*left_lane).length<<endl;
                // cout << "left line is the solid line\n";
                // return 0;
             }else{
                (*left_lane).type = DOTTED;
                // cout << "left line l: "<<(*left_lane).length<<endl;
                // cout << "left line is the dotted line\n";
                // return 0;
             }
            IncrementLeftHist((*left_lane).type);
            if(CheckLeftHist()!=(*left_lane).type){
                return 1;
            }
             return 0;
        } else if((*right_lane).exists){
             if(right_pixels>=solid_pixel_thresh){
                (*right_lane).type = SOLID;
                // cout << "right line l: "<<(*right_lane).length<<endl;
                // cout << "right line is the solid line\n";
                // return 0;
             } else {
                (*right_lane).type = DOTTED;
                // cout << "right line l: "<<(*right_lane).length<<endl;
                // cout << "right line is the dotted line\n";
                // return 0;
             }
            IncrementRightHist((*right_lane).type);
            if(CheckRightHist()!=(*right_lane).type){
                return 1;
            }
             return 0;
        }
        // ------------------- PIXEL THRESH TESTING NEW ------------------------- //
        // ------------------------ PIXEL THRESH TESTING ------------------------ //
        // cout << "\nleft " << (*left_lane).length << endl;
        // cout << "right " << (*right_lane).length << "\n" << endl;
        // usleep(3000000);

        // if((*left_lane).exists && (*right_lane).exists){         // both lines found
        //     cout << "left l: " << (*left_lane).length << endl;
        //     cout << "right l: " << (*right_lane).length << endl;
        //     if((*left_lane).length>=(*right_lane).length){          // dotted line on the left
        //         (*left_lane).type = SOLID;
        //         (*right_lane).type = DOTTED;
        //         cout << "left line is the solid line\n";
        //         cout << "right line is the dotted line\n";
        //         // return 0;
        //     } else{
        //         (*left_lane).type = DOTTED;
        //         (*right_lane).type = SOLID;
        //         cout << "right line is the solid line\n";
        //         cout << "left line is the dotted line\n";
        //         // return 0;
        //     }
        //     IncrementLeftHist((*left_lane).type);
        //     if(CheckLeftHist()!=(*left_lane).type){
        //         return 1;
        //     }
        //     IncrementRightHist((*right_lane).type);
        //     if(CheckRightHist()!=(*right_lane).type){
        //         return 1;
        //     }
        //     if(CheckLeftHist()==CheckRightHist()){
        //         return 1;
        //     }
        //     return 0;
        // } else if((*left_lane).exists){
        //      if((*left_lane).length>=DOTTEDLINELENTHRESH){
        //         (*left_lane).type = SOLID;
        //         cout << "left line l: "<<(*left_lane).length<<endl;
        //         cout << "left line is the solid line\n";
        //         // return 0;
        //      }else{
        //         (*left_lane).type = DOTTED;
        //         cout << "left line l: "<<(*left_lane).length<<endl;
        //         cout << "left line is the dotted line\n";
        //         // return 0;
        //      }
        //     IncrementLeftHist((*left_lane).type);
        //     if(CheckLeftHist()!=(*left_lane).type){
        //         return 1;
        //     }
        //      return 0;
        // } else if((*right_lane).exists){
        //      if((*right_lane).length>=DOTTEDLINELENTHRESH){
        //         (*right_lane).type = SOLID;
        //         cout << "right line l: "<<(*right_lane).length<<endl;
        //         cout << "right line is the solid line\n";
        //         // return 0;
        //      } else {
        //         (*right_lane).type = DOTTED;
        //         cout << "right line l: "<<(*right_lane).length<<endl;
        //         cout << "right line is the dotted line\n";
        //         // return 0;
        //      }
        //     IncrementRightHist((*right_lane).type);
        //     if(CheckRightHist()!=(*right_lane).type){
        //         return 1;
        //     }
        //      return 0;
        // }
        // ------------------------ PIXEL THRESH TESTING ------------------------ //
        return 1;   // there were no distinguishable lines found


        // ========================= OLD METHOD FOR FINDING DOTTED ==========================/
        // FindLines(canny_img, &(lane_temp.coords), &solid); // find only solid lines in the image
        // lane_temp.CheckExists();
        // if(lane_temp.exists){
            // SplitLane(&lane_temp, &right_temp, &left_temp);
            // -- figure out which line is the dotted one -- //
            // if((*left_lane).exists & left_temp.exists){
            //     cout << "left line is the solid line\n";
            //     (*left_lane).type = SOLID;
            // } else {
            //     cout << "left line is the dotted line\n";
            //     (*left_lane).type = DOTTED;
            // }
            // if((*right_lane).exists & right_temp.exists){
            //     cout << "right line is the solid line\n";
            //     (*right_lane).type = SOLID;
            // } else {
            //     cout << "right line is the dotted line\n";
            //     (*right_lane).type = DOTTED;
            // }
        // }
        // ===================================================================================/                        
    }
}

void ExamineMarking(Mat* canny_img, Line* marking_line){
    CheckSuspendThread();   // -- suspend the active thread if needed -- //
    FindLines(canny_img, &((*marking_line).coords), &crossing);    // find red lines in the image
    (*marking_line).CheckExists();
    (*marking_line).Cluster();

    if((*marking_line).exists){
        cout<<"red line length: "<<(*marking_line).length<<endl;
        if(((*marking_line).length>=STOPLINELENTHRESH)&&(((*marking_line).angle>120)||(*marking_line).angle<60)){
            (*marking_line).type = STOP;
            cout << "there is a stop line" << endl;
            DrawMarker redMarker(CIRCLEMARKER, "stop line", Scalar(255,255,255), Point((*marking_line).x,(*marking_line).y), true);
            AddDrawMarker(redMarker);
        } else {
            (*marking_line).type = CROSSING;
            cout << "there is a crossing" << endl;
            DrawMarker redMarker(CIRCLEMARKER, "crossing", Scalar(255,255,255), Point((*marking_line).x,(*marking_line).y), true);
            AddDrawMarker(redMarker);
        }
    }

    // ============================== OLD METHOD FOR CROSSINGS ==============================//
    // FindLines(canny_img, &((*marking_line).coords), &stop);    // find stop line in the image
    // if((*marking_line).exists){
    //     cout << "there is a stop line" << endl;
    //     (*marking_line).type = STOP;
    //     return;
    // } else {
    //     // ========================================== // 
    //     // if this check isn't enough, use pixelcount too
    //     // ========================================== //
    //     FindLines(canny_img, &((*marking_line).coords), &crossing);    // find crossing in image
    //     (*marking_line).CheckExists();
    //     if((*marking_line).exists){
    //          cout << "there is a crossing" << endl;
    //          (*marking_line).type = CROSSING;
    //     }
    // }
    // ======================================================================================//
}

void SplitLane(Line* lane, Line* right_lane, Line* left_lane){
    CheckSuspendThread();   // -- suspend the active thread if needed -- //
    if((*lane).exists){
        //Split into left and right lines
            for(int i=0; i<(*lane).coords.size();i++){
                if((abs((*lane).coords[i][0]+(*lane).coords[i][2])/2)<(IMAGEWIDTH/2)-CENTREGAP){
                    // line in left half of image
                    (*left_lane).AddLine(&((*lane).coords[i]));
                } else if((abs((*lane).coords[i][0]+(*lane).coords[i][2])/2)>(IMAGEWIDTH/2)+CENTREGAP) {
                    // line in right half of image
                    (*right_lane).AddLine(&((*lane).coords[i]));
                }
            }
            // cluster lane lines
            (*right_lane).Cluster();
            (*left_lane).Cluster();
            (*lane).Cluster();

            // average gradient of left and right lines scaled by length of each side
            // (*lane).gradient = (right_line.gradient*right_line.length+left_line.gradient*left_line.length)/2;
        // }
    }
}

int CheckIntersection(Mat* src, vector <Scalar> colour, vector <bool>* options){
    Mat dst, top, left, right;
    *options = {false, false, false};
    CheckSuspendThread();   // -- suspend the active thread if needed -- //
    Line top_lines("top lines"), left_lines("left lines"), right_lines("right lines");
    // ---------- Find edges of yellow lines ----------
    MaskHSV(src, &dst, colour);                      // create a mask from hsv based on colour

    int pixel_count = countNonZero(dst);           // count how many pixels remain in the mask
    // cout << "pixels: " << pixel_count <<endl;
    if(pixel_count<PIXELTHRESH){return 1;}      // skip if none of that colour found

    GaussianBlur(dst, dst, Size(5,5), 0, 0);      // Gaussian blur to limit noise                                 
    CheckSuspendThread();   // -- suspend the active thread if needed -- //
    Canny(dst, dst, 200, 300);
    
    // mask each side of the intersection image
    MaskRegion(&dst, &top, top_mask);
    MaskRegion(&dst, &left, left_mask);
    MaskRegion(&dst, &right, right_mask);

    // find lines in each image
    FindLines(&top, &(top_lines.coords), &dotted);    // find solid and dotted lines in the image
    FindLines(&left, &(left_lines.coords), &dotted);    // find solid and dotted lines in the image
    FindLines(&right, &(right_lines.coords), &dotted);    // find solid and dotted lines in the image

    top_lines.CheckExists();
    left_lines.CheckExists();
    right_lines.CheckExists();

    // cluster lines in each image
    top_lines.Cluster();
    left_lines.Cluster();
    right_lines.Cluster();

    // check which regions are valid.
    bool options_found = false;
    // top
    if((abs(top_lines.angle-90)<VERTLINEA)&&(top_lines.exists)){
        cout << "straight is an option" << endl;
        (*options)[STRAIGHT] = true;
        options_found = true;
        DrawMarker topMarker(CIRCLEMARKER, "straight", Scalar(0,255,0), Point(top_lines.x,top_lines.y), true);
        AddDrawMarker(topMarker);
    }
    // left
    if((abs(left_lines.angle)<HORZLINEA)&&(left_lines.exists)){
        cout << "left is an option" << endl;
        (*options)[LEFTTURN] = true;
        options_found = true;
        DrawMarker leftMarker(CIRCLEMARKER, "left", Scalar(0,255,0), Point(left_lines.x,left_lines.y), true);
        AddDrawMarker(leftMarker);
    }
    // right
    if((abs(right_lines.angle)<HORZLINEA)&&(right_lines.exists)){
        cout << "right is an option" << endl;
        (*options)[RIGHTTURN] = true;
        options_found = true;
        DrawMarker rightMarker(CIRCLEMARKER, "right", Scalar(0,255,0), Point(right_lines.x,right_lines.y), true);
        AddDrawMarker(rightMarker);
    }
    if(options_found){
        return 0;   // if there were lines found return 0
    } else {
        cout << "no valid options found, scan again" << endl;
        return 1;   // if there were no valid lines found return an error
    }
}
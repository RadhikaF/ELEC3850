#include <numeric>
#include "line.hpp"

using namespace std;
using namespace cv;

Line::Line(string line_name){
    name = line_name;
}

void Line::CheckExists(){
    if(coords.size()>0){
        exists=true;
    } else {
        exists=false;
    }
}

void Line::AddLine(Vec4i* line){
    CheckSuspendThread();   // -- suspend the active thread if needed -- //
    if (exists==false) {exists=true;}
    coords.push_back(*line);
}

void Line::CalcX(){
    CheckSuspendThread();   // -- suspend the active thread if needed -- //
    if(exists==true){
        vector <int> x_vals;
        for(int i=0; i<coords.size();i++){
            // calculate average x positions
            x_vals.push_back(double(coords[i][2]+coords[i][0])/2);
        }
        // sum up all the average x positions
        double x_sum = accumulate(x_vals.begin(),x_vals.end(),0);
        x = x_sum/x_vals.size();
    }
}

void Line::CalcY(){
    CheckSuspendThread();   // -- suspend the active thread if needed -- //
    if(exists==true){
        vector <int> y_vals;
        for(int i=0; i<coords.size();i++){
            // calculate average x positions
            y_vals.push_back(double(coords[i][1]+coords[i][3])/2);
        }
        // sum up all the average y positions
        double y_sum = accumulate(y_vals.begin(),y_vals.end(),0);
        y = y_sum/y_vals.size();
    }
}

void Line::CalcAngle(){
    CheckSuspendThread();   // -- suspend the active thread if needed -- //
    if(exists==true){
        if(l.size()==0){
            CalcLength();
        }
        a.clear();  // clears the vector to ensure it's empty
        double deltax;
        double deltay;
        double temp;
        for(int i=0; i<coords.size();i++){
            // calculate gradients and add them to vector
            deltax = coords[i][2]-coords[i][0];
            deltay = coords[i][1]-coords[i][3];
            temp = asin(deltay/l[i])*180/M_PI;
            if(temp<0){ temp+=180; }   // rotate line if it is negative to centre around y axis
            a.push_back(temp);
        }
        // calculate the average gradient
        double a_sum = accumulate(a.begin(),a.end(),0.0);
        angle = a_sum/a.size();
    }
}

void Line::CalcLength(){
    CheckSuspendThread();   // -- suspend the active thread if needed -- //
    if(exists==true){
    l.clear();  // clears the vector to ensure it's empty
        for(int i=0; i<coords.size();i++){
            // calculate lengths and add them to vector
            l.push_back(sqrt(pow(coords[i][1]-coords[i][3],2)+pow(coords[i][2]-coords[i][0],2)));
        }
    // calculate the average length
    double l_sum = accumulate(l.begin(),l.end(),0.0);
    length = l_sum/l.size();
    }
}

void Line::PrintCoords(){
    CheckSuspendThread();   // -- suspend the active thread if needed -- //
    if(exists==true){
        cout << name << " coords" << endl;        
        for(int i=0; i<coords.size();i++){
                cout<< i<<": "<< coords[i] << endl;
        }
    }
}

void Line::PrintAngles(){
    CheckSuspendThread();   // -- suspend the active thread if needed -- //
    if(exists==true){
        cout << name << " angles" << endl;        
        for(int i=0; i<a.size();i++){
                cout<< i<<": "<< a[i] << endl;
        }
    }
}

void Line::PrintLengths(){
    CheckSuspendThread();   // -- suspend the active thread if needed -- //
    if(exists==true){
        cout << name << " lengths" << endl;        
        for(int i=0; i<l.size();i++){
                cout<< i<<": "<< l[i] << endl;
        }
    }
}

//=============================== NOT USED ===============================//
void Line::CalcMinPoint(){
    CheckSuspendThread();   // -- suspend the active thread if needed -- //
    double xmin_sum=0;
    double ymin_sum=0;
    if(exists==true){    
        for(int i=0; i<coords.size();i++){
                xmin_sum += coords[i][0];
                ymin_sum += coords[i][1];
        }
        min_point=Point(int(xmin_sum/coords.size()),int(ymin_sum/coords.size()));
    }
}

void Line::CalcMaxPoint(){
    CheckSuspendThread();   // -- suspend the active thread if needed -- //
    double xmax_sum=0;
    double ymax_sum=0;
    if(exists==true){    
        for(int i=0; i<coords.size();i++){
                xmax_sum += coords[i][2];
                ymax_sum += coords[i][3];
        }
        max_point=Point(int(xmax_sum/coords.size()),int(ymax_sum/coords.size()));
    }
}
// ======================================================================= //

bool Line::Cluster(){
    CheckSuspendThread();   // -- suspend the active thread if needed -- //
    CheckExists();
    if (exists){
        CalcAngle();
        CalcLength();
        CalcX();
        CalcY();
        // CalcMinPoint();
        // CalcMaxPoint();

        // =========== Uncomment to show results ===============
        // PrintCoords();
        // PrintAngles();
        // PrintLengths();
        // cout << name << endl;
        // cout << "length: " << length << endl;
        // cout << "angle: " << angle << endl;


        // cout << "x: " << x << endl;
        // cout << "y: " << y << endl;
        
        return exists;
    } else {
        return exists;
    }
}
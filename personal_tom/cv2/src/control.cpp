#include "header.hpp"
#include "control.hpp"
#include "vision.hpp"
#include "line.hpp"
#include "motorcontrol.hpp"

using namespace std;

// const double kp = 0.05;
// const double ki = 0.01;
// const double kd = 0.1;
static double kp = 0.006;
static double ki = 0.0001;
static double kd = 0.004;

static double kpos = 2;

// static double error_mem[ERRORMEMSIZE] = {0}; 	// error memory array
static double error_prev = 0;
static double error_curr = 0;

static double clamp = false;

static double vmax = VMAX;

clock_t last = clock();

void SetPID(double P, double I, double D, double POS){
	kp = P;
	ki = I;
	kd = D;
	kpos = POS;

	cout << "kp: " << kp << endl;
	cout << "ki: " << ki << endl;
	cout << "kd: " << kd << endl;
	cout << "kpos: " << kpos << endl;
}

void PID(){
	// cout<<"vmax: "<<vmax<<endl;
	vector <double> local_motor_ctrl = GetMotorCtrl();			// get the current motor control values
	double old_velocity = CalcVelocity(local_motor_ctrl);
	double old_angular_velocity = CalcAngularVelocity(local_motor_ctrl);

	// calculate new angular velocity with PID
	// double new_angular_velocity = kp*error_curr + ki*IntError()*(clock()-last) + kd*DiffError()/(clock()-last);
	double new_angular_velocity = kp*error_curr + ki*IntError() + kd*DiffError();

	last = clock();
	// ======================= Integrator Clamping =======================//
	// check whether PID output is saturating
	if(new_angular_velocity>WMAX){
		new_angular_velocity=WMAX;
		clamp=true;
	} else if(new_angular_velocity<(-WMAX)){
		new_angular_velocity=(-WMAX);
		clamp=true;
	} else if((error_curr*new_angular_velocity)>0){
	// check sign of error & new_angular_velocity to see if they're the same
		clamp=true;
	} else { clamp = false;}

	// ========================= Speed Control =========================//
	// adjust the wheel speeds to acheive the max speed and new angular velocity
	// calculate max velocity with recommended angular momentum
	double new_r=0;
	double new_l=0;
	double vmax_temp=0;
	if(new_angular_velocity>0){
		new_r = 1;
		new_l = 1-new_angular_velocity;
		vmax_temp = 1-new_angular_velocity/2;
	} else if(new_angular_velocity<0){
		new_l = 1;
		new_r = 1+new_angular_velocity;
		vmax_temp = 1+new_angular_velocity/2;
	} else{
		new_l = 1;
		new_r = 1;
		vmax_temp = 1;
	}
	// if max posible velocity is greater than set velocity, slow it down
	if(vmax_temp>vmax){
		double scaler = vmax/vmax_temp;
		new_l*=scaler;
		new_r*=scaler;
	}
	double new_velocity = (new_l+new_r)/2;

	// cout << "old velocity:	" << old_velocity << endl;
	// cout << "old angular velocity: " << old_angular_velocity << endl;
	// cout << "new velocity: " << new_velocity << endl;
	// cout << "new_angular_velocity: " << new_angular_velocity << endl; 

	SetMotorCtrl({new_l,new_r});


	// change the velocity based on the error calculation.
	// double new_velocity = old_velocity + (ERRORVMARGIN-error_mem[0])*VADJ;
	// if (new_velocity<VMIN) {new_velocity=VMIN;}
	// if (new_velocity>vmax) {new_velocity=vmax;}

	// cout << "new_velocity: " << new_velocity << endl;

	// calculate the new angular velocity using PID
	// double new_angular_velocity = old_angular_velocity + kp*error_mem[0] + ki*IntError() + kd*DiffError();

	// if (abs(new_angular_velocity)>WMAX) {new_angular_velocity=WMAX;}
	// // calculate new motor control values from new angular velocity
	// double new_l = new_velocity - new_angular_velocity/2;
	// double new_r = new_velocity + new_angular_velocity/2;

	// // ensure resulting control vector is always positive and not > 1
	// if(new_l<0){
	// 	new_r += -new_l;
	// 	new_l = 0;
	// } else if(new_l>1){
	// 	new_r += 1-new_l;
	// 	new_l = 1;
	// }
	// if(new_r<0){
	// 	new_l += -new_r;
	// 	new_r = 0;
	// } else if(new_r>1){
	// 	new_l += 1-new_r;
	// 	new_r = 1;
	// }

	// SetMotorCtrl({new_l,new_r});

}

double CalcVelocity(vector <double> motor_control_vector){
	return (motor_control_vector[0]+motor_control_vector[1])/2;
}

double CalcAngularVelocity(vector <double> motor_control_vector){
	return motor_control_vector[1]-motor_control_vector[0];
}

double SmoothAngle(double angle, double smooth_angle){
	float snap_multiplier = 0.005;
	float Diff;
	float y;

	Diff = abs(angle - smooth_angle);		// Find the difference between the Object distance and the Average
														// 		value from the last calculation
	y = 1/(Diff*snap_multiplier + 1);					// y is a hyperbolic cuve offset by 1. Rapid changes lead to a rapid
														// 		change in the Smooth_Distance 0<y<1
	y = (1 - y)*2;										// Flips the curve so y --> 0 as x --> 0
	if (y > 1){
		y = 1;
	}
	return smooth_angle += (angle - smooth_angle)*y;
}

/*
- Calculates the error signal for the PID controller
- Returns 0 if success, 1 if fail
- will fail if there is a logical error, ie left lane is right type and right lane is wrong type
*/
int CalcErrorSignal(Line* left_line, Line* right_line, int lane){
	int left_line_desired_type;
	int right_line_desired_type;
	int lines_found;
	bool correct_lane;
	double left_opt_a = LEFTOPTA;
	double right_opt_a = RIGHTOPTA;
	double left_error = 0;
	double right_error = 0;
	int error_mult = 1;

	static double smooth_left_angle = left_opt_a;
	static double smooth_right_angle = right_opt_a;

	//====== What is the correct lines based on the lane you are in ====/
	if(lane==LEFTLANE){
		left_line_desired_type = SOLID;
		right_line_desired_type = DOTTED;
	} else if(lane==RIGHTLANE){
		left_line_desired_type = DOTTED;
		right_line_desired_type = SOLID;
	}

	// ==================== check which lines were found in vision ====================/
	if((*left_line).exists && (*right_line).exists){
		lines_found = BOTHLINES;
		smooth_left_angle = SmoothAngle((*left_line).angle, smooth_left_angle);
		smooth_right_angle = SmoothAngle((*right_line).angle, smooth_right_angle);
		error_mult = 1;
	} else if((*left_line).exists){
		lines_found = LEFTLINEONLY;
		smooth_left_angle = SmoothAngle((*left_line).angle, smooth_left_angle);
		error_mult = 2;
	} else if((*right_line).exists){
		lines_found = RIGHTLINEONLY;
		smooth_right_angle = SmoothAngle((*right_line).angle, smooth_right_angle);
		error_mult = 2;
	}

	// cout << "left smooth A: " << smooth_left_angle <<endl;
	// cout << "right smooth A: " << smooth_right_angle <<endl;


	// ======================= Lane changing is controlled here =======================/
	//====== Left line, right line or both lines type checking (dotted or solid) ======/
	// if(lines_found==BOTHLINES){
	// 	if((*left_line).type==left_line_desired_type){			// left line is correct type
	// 		if((*right_line).type==right_line_desired_type){		// both lines are correct type
	// 			correct_lane = true;
	// 		} else {
	// 			return 1;										// one line is incorrect type (logic error)
	// 		} 
	// 	} else if((*right_line).type==right_line_desired_type){
	// 		if((*left_line).type==left_line_desired_type){			// both lines are correct type
	// 			correct_lane = true;
	// 		} else {
	// 			return 1;										// one line is incorrect type (logic error)
	// 		} 
	// 	} else {
	// 		correct_lane = false;
	// 	}
	// } else if(lines_found==RIGHTLINEONLY){						// right line is correct type
	// 	if((*right_line).type==right_line_desired_type){
	// 		correct_lane = true;
	// 	} else {
	// 		correct_lane = false;
	// 	}
	// } else if(lines_found==LEFTLINEONLY){						// left line is correct type
	// 	if((*left_line).type==left_line_desired_type){
	// 		correct_lane = true;
	// 	} else {
	// 		correct_lane = false;
	// 	}
	// }
	correct_lane = true;

	// =================== Calculate error from angle of lines ================== /
	// ------ See onenote if you want to understand how this works -------/

	if(lines_found==BOTHLINES){
		// left_error = (*left_line).angle - (left_opt_a+((*left_line).x*(90.0-left_opt_a)/(IMAGEWIDTH/2.0)));
		// right_error = (*right_line).angle - right_opt_a-((IMAGEWIDTH-(*right_line).x)*(right_opt_a-90.0)/(IMAGEWIDTH/2.0));
		
		// TEST
		left_error = smooth_left_angle - left_opt_a - ((*left_line).x*kpos-10);
		right_error = smooth_right_angle - right_opt_a + (IMAGEWIDTH-(*right_line).x)*kpos;
		// left_error = smooth_left_angle - left_opt_a;
		// right_error = smooth_right_angle- right_opt_a;
		// TEST

		if(correct_lane==false){										// need to switch lanes
			if((*left_line).type==DOTTED){							
				left_error = right_opt_a-smooth_left_angle;			// right lane, turn left
			} else if((*right_line).type==DOTTED){
				right_error = left_opt_a-smooth_right_angle;			// left lane, turn right
			}
		}
	} else if (lines_found==RIGHTLINEONLY){
		// TEST
		// right_error = smooth_right_angle- right_opt_a;
		right_error = smooth_right_angle - right_opt_a + (IMAGEWIDTH-(*right_line).x)*kpos;
		// TEST
		// right_error = (*right_line).angle - right_opt_a-((IMAGEWIDTH-(*right_line).x)*(right_opt_a-90.0)/(IMAGEWIDTH/2.0));
		if((correct_lane==false)&&((*right_line).type==DOTTED)){		// need to switch lanes
			right_error = left_opt_a-smooth_right_angle;				// left lane turn right
		} else if(correct_lane==false){
			right_error = -(left_opt_a-smooth_right_angle);			// right lane turn left
		}
	} else if (lines_found==LEFTLINEONLY){	
		// TEST
		// left_error = smooth_left_angle - left_opt_a;
		left_error = smooth_left_angle - left_opt_a - ((*left_line).x*kpos-10);
		// TEST
		// left_error = (*left_line).angle - (left_opt_a+((*left_line).x*(90.0-left_opt_a)/(IMAGEWIDTH/2.0)));
		if((correct_lane==false)&&((*left_line).type==DOTTED)){			// need to switch lanes
			left_error = right_opt_a-smooth_left_angle;				// right lane turn left
		} else if(correct_lane=false){
			left_error = -(right_opt_a-smooth_left_angle);				// left lane turn right
		}
	}

	// =================== Calculate error from angle of lines ================== /
	// ================================== OLD =================================== /
	// if(lines_found==BOTHLINES){
	// 	left_error = left_opt_a - (*left_line).angle;
	// 	right_error = right_opt_a - (*right_line).angle;
	// 	if(correct_lane==false){										// need to switch lanes
	// 		if((*left_line).type==DOTTED){							
	// 			left_error = right_opt_a-(*left_line).angle;			// right lane, turn left
	// 		} else if((*right_line).type==DOTTED){
	// 			right_error = left_opt_a-(*right_line).angle;			// left lane, turn right
	// 		}
	// 	}
	// } else if (lines_found==RIGHTLINEONLY){
	// 	right_error = right_opt_a - (*right_line).angle;
	// 	if((correct_lane==false)&&((*right_line).type==DOTTED)){		// need to switch lanes
	// 		right_error = left_opt_a-(*right_line).angle;				// left lane turn right
	// 	} else if(correct_lane==false){
	// 		right_error = -(left_opt_a-(*right_line).angle);			// right lane turn left
	// 	}
	// } else if (lines_found==LEFTLINEONLY){	
	// 	left_error = left_opt_a - (*left_line).angle;
	// 	if((correct_lane==false)&&((*left_line).type==DOTTED)){			// need to switch lanes
	// 		left_error = right_opt_a-(*left_line).angle;				// right lane turn left
	// 	} else if(correct_lane=false){
	// 		left_error = -(right_opt_a-(*left_line).angle);				// left lane turn right
	// 	}
	// }
	// =========================================================================== /

	NewError((right_error + left_error)*error_mult);			// calculate overall error signal
	return 0;
}

/*
- Shift the error array along by one position then add the new error to the first position.
*/
void NewError(double new_error){
	// for(int i=0; i<ERRORMEMSIZE-1;i++){
	// 	error_mem[i+1] = error_mem[i];
	// }
	// error_mem[0] = new_error;
	error_prev = error_curr;
	error_curr = new_error;
	cout << "error: " << error_curr << endl;
}

/*
- Calculates the change in error since the last value
*/
double DiffError(){
	if(error_curr == 0){
		return 0;
	}
	return error_curr-error_prev;
	// return error_mem[0]-error_mem[1];
}

/*
- Calculates the integral of the error
*/
double IntError(){
	static double error_sum = 0;
	if(clamp) {return error_sum;}

	// for(int i=0; i<ERRORMEMSIZE; i++){
	// 	error_sum += error_mem[i];
	// }
	error_sum+=error_curr;
	return error_sum;
}

void SetVmax(double new_vmax){
	vmax = new_vmax;
}

double GetVmax(){
	return vmax;
}

int ChooseIntersectionDirection(vector <bool>* options){
	vector <int> option_list;
	for(int i=0; i<(*options).size(); i++){
		if((*options)[i]){
			option_list.push_back(i);
		}
	}
	if(option_list.size()==0){
		return -1;	// there are no valid options
	}
	int rand_option = rand()%option_list.size();
	return option_list[rand_option];
}

void StraightenUp(double stop_angle){
	NewError(0);
	double kstop = 30;
	double diff_angle;
	if(stop_angle>90){
		diff_angle = stop_angle-180;
		SetMotorCtrl({1,0});
	} else {
		diff_angle = stop_angle;
		SetMotorCtrl({0,1});
	}
	cout << "stop angle: "<<stop_angle<<endl;
	cout << "stop angle Diff: "<<diff_angle<<endl;
	usleep(abs(diff_angle*kstop*1000));
	SetMotorCtrl({0,0});    // stop
}

void GoStraight(){
	SetMotorCtrl({VMAX,VMAX});
	usleep(abs(1000000));
}
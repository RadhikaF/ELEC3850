#ifndef _CONTROL_HPP_
#define _CONTROL_HPP_

#include "line.hpp"

#define LEFTLINEONLY 0
#define RIGHTLINEONLY 1
#define BOTHLINES 2

#define LEFTOPTA 79
#define RIGHTOPTA 98

#define ERRORVMARGIN 5
#define VADJ 0.01
#define WMAX 1					// max angular velocity

#define CARWIDTH 10				// not currently used as constant and doesn't matter units
#define WHEELRADIUS 5			// not currently used ^^

#define ERRORMEMSIZE 10

void PID();
double CalcVelocity(std::vector <double> motor_control_vector);
double CalcAngularVelocity(std::vector <double> motor_control_vector);
int CalcErrorSignal(Line* left_line, Line* right_line, int lane);
void NewError(double new_error);
double DiffError();
double IntError();
void SetVmax(double new_vmax);
double GetVmax();
int ChooseIntersectionDirection(std::vector <bool>* options);
double SmoothAngle(double angle, double smooth_angle);
void SetPID(double P, double I, double D, double POS);
void StraightenUp(double stop_angle);
void GoStraight();

#endif
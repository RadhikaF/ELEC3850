/*
    File name:          motorcontrol.cpp
    Version:            0.1
    Author:             Tom Bailey
    Date created:       2018-08-21
    Date last modified: 2018-08-21

    Description:

*/
#include "header.hpp"
#include "motorcontrol.hpp"

#include <stdio.h>		// Needed to print to putty
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "pca9685.h"
#include <wiringPi.h>
#include <wiringPiI2C.h>

#include <SDL_joystick.h>
#include <unistd.h>
#include "SDL.h"

using namespace std;

#define MAX_CONTROLLERS 10

//Analog joystick dead zone
const int JOYSTICK_DEAD_ZONE = 4000;

static bool xbox_controller = false;

SDL_GameController *ControllerHandles[MAX_CONTROLLERS];

static double prev_ema[2] = {0, 0};		// previous exponential moving average stored here

int motor_input(double left_motor, double right_motor);
double exponential_moving_average (double new_point, int left_or_right);
int initialise(void);

// motor control mutex
pthread_mutex_t motor_ctrl_mutex = PTHREAD_MUTEX_INITIALIZER;

// motor control vector
static vector <double> motor_ctrl(2);

// sets the values in the motor_ctrl vector
void SetMotorCtrl(vector <double> new_motor_ctrl){
	pthread_mutex_lock(&motor_ctrl_mutex);
	motor_ctrl = new_motor_ctrl;
	// cout << "motor control set: " << motor_ctrl[0] << "," << motor_ctrl[1] << endl;
	pthread_mutex_unlock(&motor_ctrl_mutex);
}

// returns the values in the motor_ctrl vector
vector <double> GetMotorCtrl(){
	pthread_mutex_lock(&motor_ctrl_mutex);
	vector <double> temp = motor_ctrl;
	pthread_mutex_unlock(&motor_ctrl_mutex);
	return temp;
}

// std::vector<int> GetMotorCtrl(std:vector<int>);

void* MotorControl(void* arg){
	cout << "in motorcontrol" << endl;
	int init = initialise();
	printf("%d\n", wiringPiSetup());
	int test1 = xbox_init();

	while(1){
		if(xbox_controller&&!GetControlMode()){
			xbox_main();
		}
		if(GetMotorRun()){
			motor_input(motor_ctrl[0], motor_ctrl[1]);
			usleep(10000);
		} else {
			StopMotors();
		}
	}
}

//PCA9685 pwm(1,1);

// int main()
// {
// 	int init = initialise();
// 	printf("%d\n", wiringPiSetup());
// 	motor_input(0.5, 0.4);
// 	motor_input(0.3, 0.2);
// 	motor_input(0.99, 0.99);
// 	motor_input(0.99, 0.99);
// 	motor_input(0.99, 0.99);
// 	motor_input(0.99, 0.99);
// 	motor_input(0.99, 0.99);
// 	return 0;
// }

int initialise(void)
{
	printf("Lets go!\n");
	int fd = pca9685Setup(PIN_BASE, 0x40, HERTZ);
	if (fd < 0)
	{
		printf("Error in setup\n");
		return fd;
	}
	pca9685PWMReset(fd);
	return 0;
}

int motor_input(double left_motor, double right_motor)
{
	double left_ema = 0;
	double right_ema = 0;
	if ((left_motor > 1.0) | (left_motor < -1.0))
	{
		printf("left motor out of range\n");
		return 0;
	}
	else
	{
		// left_ema = exponential_moving_average(left_motor, 0);
		left_ema = left_motor;
	}
	if ((right_motor > 1.0) | (right_motor < -1))
	{
		printf("right motor out of range\n");
		return 0;
	}
	else
	{
		// right_ema = exponential_moving_average(right_motor, 1);
		right_ema = right_motor;
	}
	int left_direction = 0;
	int right_direction = 0;
	int left_pwm = 0;
	int right_pwm;
	if (left_ema >= 0)
	{
		left_direction = 0;
	}
	else
	{
		left_direction = 1;
	}
	if (right_ema >= 0)
	{
		right_direction = 0;
	}
	else
	{
		right_direction = 1;
	}
	left_pwm = int(abs(left_ema) * 4096);
	right_pwm = int(abs(right_ema) * 4096);
	left_direction = left_direction * 4096;
	right_direction = right_direction * 4096;

	// pwm.setPWM(0, left_direction);
	// pwm.setPWM(1, left_pwm);
	// pwm.setPWM(2, right_direction);
	// pwm.setPWM(3, right_pwm);

	// printf("%d, %d, %d, %d\n", left_direction, left_pwm, right_direction, right_pwm);

	pwmWrite(0 + PIN_BASE, left_direction);
	pwmWrite(1 + PIN_BASE, left_pwm);
	pwmWrite(2 + PIN_BASE, right_direction);
	pwmWrite(3 + PIN_BASE, right_pwm);

	/*ofstream file;
	file.open("pwm_data.txt");
	file << left_direction << " " << right_direction << " " << left_pwm << " " << right_pwm;
	file.close();
	// call motors function pwm_motors(left_direction, right_direction, left_pwm, right_pwm)
	printf("%d...%d...%d...%d...\n", left_direction, right_direction, left_pwm, right_pwm);
	*/
	return 1;
}

double exponential_moving_average (double new_point, int left_or_right)
{
	double multiplier = 2.0 / (NO_POINTS + 1.0); 	// for 10 period EMA, should be 0.1818
	double ema = (new_point - prev_ema[left_or_right]) * multiplier + prev_ema[left_or_right];
	// printf("%f %f\n", ema, new_point, multiplier);
	prev_ema[left_or_right] = ema;
	return ema;
}

void StopMotors(){
	prev_ema[0] = 0;
	prev_ema[1] = 0;
	// printf("%d, %d, %d, %d\n", 1, 0, 1, 0);

	pwmWrite(0 + PIN_BASE, 1);
	pwmWrite(1 + PIN_BASE, 0);
	pwmWrite(2 + PIN_BASE, 1);
	pwmWrite(3 + PIN_BASE, 0);
	usleep(100000);
}

int xbox_init()
{
    //Initialization flag
    int success = 1;

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) < 0 )
    {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        success = 0;
    }
	int ControllerIndex = 0;
    //Check for joysticks
    if( SDL_NumJoysticks() < 1 )
    {
        printf( "Warning: No joysticks connected!\n" );
    }
    else
    {
    	printf("Success! Number of joysticks: %d\n", SDL_NumJoysticks());
    	int MaxJoysticks = SDL_NumJoysticks();
		if (!SDL_IsGameController(ControllerIndex))
	    {
	        printf("Unexpected error\n");
	    }
	    ControllerHandles[ControllerIndex] = SDL_GameControllerOpen(ControllerIndex);
    }
    //int IsAPressed = SDL_GameControllerGetButton(Controller, SDL_CONTROLLER_BUTTON_A);
    //for (int ControllerIndex = 0;
    // ControllerIndex < MAX_CONTROLLERS;
    // ++ControllerIndex)
	int StickLX = SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_LEFTX);
	int StickLY = SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_LEFTY);
	//int StickRX = SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_RIGHTX);
	//int StickRY = SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_RIGHTY);
	printf("%d %d\n", StickLX, StickLY);

	return 0;
}

int xbox_main()
{
	//printf("Main\n");
    //Event handler
    SDL_Event e;
    static double x_value;
    static double y_value;

    const double max = 32000.0;

    //Handle events on queue
    while( SDL_PollEvent( &e ) != 0 )
    {
    	if (e.jaxis.axis == 0)
    	{
    		x_value = e.jaxis.value/max;
    	}
    	if (e.jaxis.axis == 1)
    	{
    		y_value = -e.jaxis.value/max;
    	}
    }
    if((abs(x_value)>0.15)||(abs(y_value)>0.15)){
    	CalcRL(x_value,y_value);
    } else {
    	CalcRL(0,0);
    }
}

void CalcRL(double x, double y){
	if(x>1){
		x=1;
	} else if(x<-1){
		x=-1;
	}
	if(y>1){
		y=1;
	} else if(y<-1){
		y=-1;
	}
	x = -x;
	double velocity = (1-abs(x))*y+y;
	double angular_velocity = (1-abs(y))*x+x;
	double r = (velocity+angular_velocity)/2.0;
	double l = (velocity-angular_velocity)/2.0;
	cout << "l: " << l << endl;
	cout << "r: " << r << endl;
	SetMotorCtrl({l,r});
}

void XboxControlEnable(bool set){
	xbox_controller = set;
	cout << "xbox mode" << set << endl;
}

bool GetXboxControl(){
	return xbox_controller;
}
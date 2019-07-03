#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <thread>
#include <signal.h>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <sched.h>

#include "header.hpp"
#include "filefunctions.hpp"
#include "vision.hpp"
#include "motorcontrol.hpp"
#include "pixy_getframe.hpp"
#include "ELEC3850_client.hpp"

using namespace std;
using namespace cv;

pthread_mutex_t my_imshow_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_t vision_th;
pthread_t motorcontrol_th;
pthread_t camera_th;
pthread_t interface_th;

pthread_mutex_t suspend_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t control_mode_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t motor_run_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t resume_cond;
static bool suspend_flag = false;
static bool control_mode = 0;	// 1 autonomous, 0 manual
static bool motor_run = 1; // 1 run, 0 stopped

// used for local storage of motor_ctrl variable, use with GetMotorCtrl
vector <int> local_motor_ctrl(2);

int main(){
	cout << "hello from main" << endl;
	signal(SIGINT, handle_SIGINT);	// handles ctrl+c

	// intialise motor_ctrl to 0
	SetMotorCtrl({0,0});

	// setup pixycam
	SetupPixycam();

	// set up threads
	if(pthread_create(&camera_th, NULL, &PixyRun, NULL)){
		cout << "error creating camera thread, abort" << endl;
		return 1;
	}

	if(pthread_create(&vision_th, NULL, &Vision, NULL)){
		cout << "error creating vision thread, abort" << endl;
		return 1;
	}
	if(pthread_create(&motorcontrol_th, NULL, &MotorControl, NULL)){
	 	cout << "error creating motor control thread, abort" << endl;
	 	return 1;
	}
	if(pthread_create(&interface_th, NULL, &InterfaceRun, NULL)){
		cout << "error creating interface thread, abort" << endl;
		return 1;
	}
	pthread_join(vision_th,NULL);
	pthread_join(camera_th,NULL);
	pthread_join(motorcontrol_th,NULL);
	pthread_join(interface_th,NULL);
	usleep(1000000);
	// while(1){
		// usleep(1000000);
	// }
	
	return 0;

}

void MyImshow(string name,Mat* img,int wait){
	pthread_mutex_lock(&my_imshow_mutex);
	imshow(name, *img);
	waitKey(wait);
	pthread_mutex_unlock(&my_imshow_mutex);
}

void SetRealtimePriority() {
	int ret;

	// We'll operate on the currently running thread.
	pthread_t this_thread = pthread_self();
	// struct sched_param is used to store the scheduling priority
	struct sched_param params;

	// We'll set the priority to the maximum.
	params.sched_priority = sched_get_priority_max(SCHED_FIFO);
	std::cout << "Trying to set thread realtime prio = " << params.sched_priority << std::endl;

	// Attempt to set thread real-time priority to the SCHED_FIFO policy
	ret = pthread_setschedparam(this_thread, SCHED_FIFO, &params);
	if (ret != 0) {
		// Print the error
		std::cout << "Unsuccessful in setting thread realtime prio, ERROR: " << ret <<std::endl;
	return;     
	}
	// Now verify the change in thread priority
	int policy = 0;
	ret = pthread_getschedparam(this_thread, &policy, &params);
	if (ret != 0) {
		std::cout << "Couldn't retrieve real-time scheduling paramers" << std::endl;
		return;
	}

	// Check the correct policy was applied
	if(policy != SCHED_FIFO) {
		std::cout << "Scheduling is NOT SCHED_FIFO!" << std::endl;
	} else {
		std::cout << "SCHED_FIFO OK" << std::endl;
	}

	// Print thread scheduling priority
	std::cout << "Thread priority is " << params.sched_priority << std::endl; 
}

void SetRRPriority() {
	int ret;

	// We'll operate on the currently running thread.
	pthread_t this_thread = pthread_self();
	// struct sched_param is used to store the scheduling priority
	struct sched_param params;

	// We'll set the priority to the maximum.
	params.sched_priority = sched_get_priority_min(SCHED_RR);
	std::cout << "Trying to set thread realtime prio = " << params.sched_priority << std::endl;

	// Attempt to set thread real-time priority to the SCHED_FIFO policy
	ret = pthread_setschedparam(this_thread, SCHED_RR, &params);
	if (ret != 0) {
		// Print the error
		std::cout << "Unsuccessful in setting thread realtime prio, ERROR: " << ret <<std::endl;
	return;     
	}
	// Now verify the change in thread priority
	int policy = 0;
	ret = pthread_getschedparam(this_thread, &policy, &params);
	if (ret != 0) {
		std::cout << "Couldn't retrieve real-time scheduling paramers" << std::endl;
		return;
	}

	// Check the correct policy was applied
	if(policy != SCHED_RR) {
		std::cout << "Scheduling is NOT SCHED_FIFO!" << std::endl;
	} else {
		std::cout << "SCHED_FIFO OK" << std::endl;
	}

	// Print thread scheduling priority
	std::cout << "Thread priority is " << params.sched_priority << std::endl; 
}

void LockImshow(){
	pthread_mutex_lock(&my_imshow_mutex);
}

void UnlockImshow(){
	pthread_mutex_unlock(&my_imshow_mutex);
}

void SuspendThread(){
	pthread_mutex_lock(&suspend_mutex);
	suspend_flag = true;
	pthread_mutex_unlock(&suspend_mutex);
}

void ResumeThread(){
	pthread_mutex_lock(&suspend_mutex);
	suspend_flag = false;
	pthread_cond_broadcast(&resume_cond);
	pthread_mutex_unlock(&suspend_mutex);
}

void CheckSuspendThread(){
	pthread_mutex_lock(&suspend_mutex);
	while(suspend_flag) pthread_cond_wait(&resume_cond, &suspend_mutex);
	pthread_mutex_unlock(&suspend_mutex);
}

void SetControlMode(bool new_mode){
	pthread_mutex_lock(&control_mode_mutex);
	control_mode = new_mode;
	cout << "control mode set: " << new_mode << endl;
	pthread_mutex_unlock(&control_mode_mutex);
}

bool GetControlMode(){
	pthread_mutex_lock(&control_mode_mutex);
	bool temp = control_mode;
	pthread_mutex_unlock(&control_mode_mutex);
	return temp;
}

void SetMotorRun(bool new_motor_run){
	pthread_mutex_lock(&motor_run_mutex);
	motor_run = new_motor_run;
	cout << "motor run set: " << new_motor_run << endl;
	pthread_mutex_unlock(&motor_run_mutex);
}

bool GetMotorRun(){
	pthread_mutex_lock(&motor_run_mutex);
	bool temp = motor_run;
	pthread_mutex_unlock(&motor_run_mutex);
	return temp;
}
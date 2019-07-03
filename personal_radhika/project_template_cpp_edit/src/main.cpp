#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <thread>
#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

#include "header.hpp"
#include "ELEC3850_client.hpp"
#include "base64.hpp"

using namespace std;

//pthread_t vision_th;
//pthread_t motorcontrol_th;
//pthread_t camera_th;
pthread_t interface_th;

pthread_mutex_t suspend_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t resume_cond;
bool suspend_flag = false;

int main(){
	cout << "hello from main" << endl;
	
	// set up threads
	if(pthread_create(&interface_th, NULL, &InterfaceRun, NULL)){
		cout << "error creating user interface thread, abort" << endl;
		return 1;
	}

	pthread_join(interface_th,NULL);
	// while(1){
		// usleep(1000000);
	// }
	
	return 0;

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
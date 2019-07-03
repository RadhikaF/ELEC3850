// Socket.IO C++ Client
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <thread>
#include <vector>
#include "/home/pi/Elec3850/personal_tom/cv2/lib/socket.io-client-cpp/src/sio_client.h"
#include "ELEC3850_client.hpp"
#include "motorcontrol.hpp"
#include "control.hpp"


using namespace std;
pthread_mutex_t sio_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t sio_cond;
bool connect_finish = false;
static cv::Mat image;
// motor control vector
vector <double> motor_ctrl(2);
int p=0;
int i=0;
int d=0;

static double x=0;
static double y=0;

static bool manual_enable = true;

class connection_client
{
	sio::client &handler;
	
	public:
	
	connection_client(sio::client& h):
    handler(h)
    {
    }
	
	void on_close()
	{
		std::cout << "Sio socket disconnect" << std::endl;
		exit(EXIT_FAILURE);
	}
		
	void on_fail()
	{
		std::cout << "Sio socket failed" << std::endl;
		exit(EXIT_FAILURE);
	}
	
	void on_connected()
	{
		std::cout<<"Sio socket connected"<<std::endl;
		pthread_mutex_lock(&sio_mutex);
		connect_finish = true;
		pthread_cond_broadcast(&sio_cond);
		pthread_mutex_unlock(&sio_mutex);
	}
};

void* InterfaceRun(void* arg){
	cout << "in interface" << endl;
	sio::client h;
	connection_client l(h);
    
    h.set_open_listener(std::bind(&connection_client::on_connected, &l));
    h.set_close_listener(std::bind(&connection_client::on_close, &l));
    h.set_fail_listener(std::bind(&connection_client::on_fail, &l));
	
	h.connect("http://192.168.43.143:3000");
	 
	pthread_mutex_lock(&sio_mutex);
    if(!connect_finish)
    {
		pthread_cond_wait(&sio_cond, &sio_mutex);
    }
	pthread_mutex_unlock(&sio_mutex);
		std::cout<<"After mutex"<<std::endl;

	sio::socket::ptr current_socket;
	current_socket = h.socket();
	std::cout<<"After current_socket"<<std::endl;

	
	
		h.socket()->on("c++ request", sio::socket::event_listener_aux([&](string const& name, sio::message::ptr const& data, bool isAck,sio::message::list &ack_resp)
	{
		motor_ctrl = GetMotorCtrl();
		// double x = (double) rand()/RAND_MAX;
		// double y = (double) rand()/RAND_MAX;
		
		// std::vector<double> vect = {x, y};

	   double left = motor_ctrl[0];
	   double right = motor_ctrl[1];
	   
	   std::string command = data->get_string();
	   std::string up ("up");
	   std::string left_str ("left");
	   std::string stop ("stop");
	   std::string right_str ("right");
	   std::string reverse ("reverse");
	   std::cout<<"Before: " << left << ", " << right << std::endl;

	   // toms changes -------------------------------------
	   // std::string p_up ("p_up");
	   // std::string p_up ("p_down");


	   // toms changes-----------------------------------
		   
	// if (up.compare(command)  == 0)
	// 	{
	// 	std::cout<<"Up"<<std::endl;
	// 	SetMotorRun(1);
	// 			left = left + 0.1;
	// 			if (left > 1)
	// 			{
	// 				left = 1;
	// 			}
	// 			else if (left < -1)
	// 			{
	// 				left = -1;
	// 			}
	// 			right = right + 0.1;
	// 			if (right > 1)
	// 			{
	// 				right = 1;
	// 			}
	// 			else if (right < -1)
	// 			{
	// 				right = -1;
	// 			}				
	// 	}
	// 	else if (left_str.compare(command)  == 0)
	// 	{
	// 	std::cout<<"Left"<<std::endl;
	// 	SetMotorRun(1);
	// 			right = right + 0.1;
	// 			if (right > 1)
	// 			{
	// 				right = 1;
	// 			}
	// 			else if (right < -1)
	// 			{
	// 				right = -1;
	// 			}
				
	// 			left = left - 0.1;
	// 			if (left > 1)
	// 			{
	// 				left = 1;
	// 			}
	// 			else if (left < -1)
	// 			{
	// 				left = -1;
	// 			}			
	// 	}
	// 	else if (stop.compare(command)  == 0)
	// 	{
	// 		left = 0;
	// 		right = 0;
	// 		SetMotorRun(0);
	// 	std::cout<<"Stop"<<std::endl;
			
	// 	}
	// 	else if (right_str.compare(command)  == 0)
	// 	{
	// 	std::cout<<"Right"<<std::endl;
	// 	SetMotorRun(1);
	// 			left = left + 0.1;
	// 			if (left > 1)
	// 			{
	// 				left = 1;
	// 			}
	// 			else if (left < -1)
	// 			{
	// 				left = -1;
	// 			}
				
	// 			right = right - 0.1;
	// 			if (right > 1)
	// 			{
	// 				right = 1;
	// 			}
	// 			else if (right < -1)
	// 			{
	// 				right = -1;
	// 			}
	// 	}
	// 	else if (reverse.compare(command)  == 0)
	// 	{
	// 	std::cout<<"Reverse"<<std::endl;
	// 	SetMotorRun(1);
	// 			left = left - 0.1;
	// 			if (left > 1)
	// 			{
	// 				left = 1;
	// 			}
	// 			else if (left < -1)
	// 			{
	// 				left = -1;
	// 			}
	// 			right = right - 0.1;				
	// 			if (right > 1)
	// 			{
	// 				right = 1;
	// 			}
	// 			else if (right < -1)
	// 			{
	// 				right = -1;
	// 			}			
	// 	}

	// --------------- TOMS CHANGES ----------------------


	   if (up.compare(command)  == 0)
		{
		std::cout<<"Up"<<std::endl;
		SetMotorRun(1);
				y += 0.1;		
		}
		else if (left_str.compare(command)  == 0)
		{
		std::cout<<"Left"<<std::endl;
		SetMotorRun(1);
				x -= 0.1;
		}
		else if (stop.compare(command)  == 0)
		{
			x = 0;
			y = 0;
			SetMotorRun(0);
		std::cout<<"Stop"<<std::endl;
			
		}
		else if (right_str.compare(command)  == 0)
		{
		std::cout<<"Right"<<std::endl;
		SetMotorRun(1);
				x += 0.1;
		}
		else if (reverse.compare(command)  == 0)
		{
		std::cout<<"Reverse"<<std::endl;
		SetMotorRun(1);
				y-=0.1;
		}

		if(!GetXboxControl()&&!GetControlMode()){
			CalcRL(x,y);
		}
	

	// std::cout<<"After: " << left << ", " << right << std::endl;
	}));

//Receive Xbox control mode
	bool xbox;
	
	h.socket()->on("XboxMode", sio::socket::event_listener_aux([&](string const& name, sio::message::ptr const& data, bool isAck,sio::message::list &ack_resp)
	{
		pthread_mutex_lock(&sio_mutex);
		xbox = data->get_bool();
		std::cout<<"> *** Xbox control: "<<std::endl;

		std::cout<<xbox<<std::endl;

		if (xbox == true)		//use xbox as controller and ignore any other inputs
		{
			XboxControlEnable(true);
			SetMotorRun(1);
			//SuspendThread();
//or set suspend_flag to true to let vision thread suspend itself?
		}
		else{
			XboxControlEnable(false);
			if(!GetControlMode()){
				StopMotors();
				x = 0;
				y = 0;
			}
		}

		pthread_mutex_unlock(&sio_mutex);
	}));

	
	//Receive new mode from server (mode has been toggled Autonomous<->Manual)
	h.socket()->on("ToggleMode", sio::socket::event_listener_aux([&](string const& name, sio::message::ptr const& data, bool isAck,sio::message::list &ack_resp)
	{
		pthread_mutex_lock(&sio_mutex);
		bool mode = data->get_bool();
		std::cout<<"> *** C++ mode: "<<std::endl;

		std::cout<<mode<<std::endl;
		if (mode == true)
		{
			std::cout<<mode<<endl;
			SetControlMode(1);
			SetMotorRun(1);
			// SuspendThread();
//or set suspend_flag to true to let vision thread suspend itself?
		}
		else{
			std::cout<<mode<<endl;
			SetControlMode(0);
			StopMotors();
			x = 0;
			y = 0;
			// ResumeThread();
		}

		pthread_mutex_unlock(&sio_mutex);
	}));
	
	bool camera=false;

	h.socket()->on("ToggleCamMode", sio::socket::event_listener_aux([&](string const& name, sio::message::ptr const& data, bool isAck,sio::message::list &ack_resp)
	{
		pthread_mutex_lock(&sio_mutex);
		camera = data->get_bool();
		std::cout<<"> *** C++ camera: "<<std::endl;

		std::cout<<camera<<std::endl;

		if (camera == true)
		{
			//SuspendThread();
//or set suspend_flag to true to let vision thread suspend itself?
		}
		else{
			//ResumeThread();
		}

		pthread_mutex_unlock(&sio_mutex);
	}));

	h.socket()->on("PID", sio::socket::event_listener_aux([&](string const& name, sio::message::ptr const& data, bool isAck,sio::message::list &ack_resp)
	{
		pthread_mutex_lock(&sio_mutex);
		string PID1_str = data->get_map()["PID1"]->get_string();
		string PID2_str = data->get_map()["PID2"]->get_string();
		string PID3_str = data->get_map()["PID3"]->get_string();
		string PID4_str = data->get_map()["PID4"]->get_string();

		std::cout<<"> *** C++ Received PID: "<<PID1_str<<std::endl;
		double PID1 = stof(PID1_str, NULL);
		double PID2 = stof(PID2_str, NULL);
		double PID3 = stof(PID3_str, NULL);
		double PID4 = stof(PID4_str, NULL);

		SetPID(PID1, PID2, PID3, PID4);
		std::cout<<PID1<<std::endl;
		std::cout<<PID2<<std::endl;
		std::cout<<PID3<<std::endl;
		std::cout<<PID4<<std::endl;

		pthread_mutex_unlock(&sio_mutex);
	}));

	std::vector<uchar> buffer;
	int x = 0;
	while(1){


		usleep(200000);
		// have flag indicating when Mat data is ready to be sent to socket: while(flag == false) {wait}
		//std::cout<<x<<std::endl;
		if (camera){
		// read test image
			if (image.data == NULL)
			{
				std::cout<<"Image not found"<<std::endl;
			}
			else{
				cv::imencode(".png",image,buffer);
				std::string s = base64_encode(buffer.data(),buffer.size());

				sio::message::ptr image_data(sio::object_message::create());
				std::map<std::string, sio::message::ptr>& map = image_data->get_map();
				  
				// Set the member, type and name of object
				map.insert(std::make_pair("image", sio::bool_message::create(true)));
				map.insert(std::make_pair("buffer", sio::string_message::create(s)));
				
				// Send the image command to the server
				h.socket()->emit("image", image_data);
			}
		}
	}

	h.sync_close();
	h.clear_con_listeners();

	return 0;
}


void SendImage(cv::Mat* new_image){
	image = *new_image;
}
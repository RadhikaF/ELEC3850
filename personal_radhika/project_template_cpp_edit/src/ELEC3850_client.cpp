// Socket.IO C++ Client
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <thread>
#include <vector>
#include "/home/pi/nodetest/project_template_cpp_edit/lib/socket.io-client-cpp/src/sio_client.h"
#include "ELEC3850_client.hpp"

using namespace std;
pthread_mutex_t sio_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t sio_cond;
bool connect_finish = false;

const string resources_dir = "/home/pi/nodetest/project_template_cpp_edit/resources/";

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
	sio::client h;
	connection_client l(h);
    
    h.set_open_listener(std::bind(&connection_client::on_connected, &l));
    h.set_close_listener(std::bind(&connection_client::on_close, &l));
    h.set_fail_listener(std::bind(&connection_client::on_fail, &l));
	
	h.connect("http://192.168.1.108 :3000");
	
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
		
		double x = (double) rand()/RAND_MAX;
		double y = (double) rand()/RAND_MAX;
		
		std::vector<double> vect = {x, y};

	   double left = vect[0];
	   double right = vect[1];
	   
	   std::string command = data->get_string();
	   std::string up ("up");
	   std::string left_str ("left");
	   std::string stop ("stop");
	   std::string right_str ("right");
	   std::string reverse ("reverse");
	   std::cout<<"Before: " << left << ", " << right << std::endl;
		   
	if (up.compare(command)  == 0)
		{
		std::cout<<"Up"<<std::endl;
			if (-1 < left && left < 1 && -1 < right && right < 1)
			{
				left = left + 0.1;
				if (left > 1)
				{
					left = 1;
				}
				else if (left < -1)
				{
					left = -1;
				}
				right = right + 0.1;
				if (right > 1)
				{
					right = 1;
				}
				else if (right < -1)
				{
					right = -1;
				}
			}				
		}
		else if (left_str.compare(command)  == 0)
		{
		std::cout<<"Left"<<std::endl;
			if (-1 < right && right < 1)
			{
				right = right + 0.1;
				if (right > 1)
				{
					right = 1;
				}
				else if (right < -1)
				{
					right = -1;
				}
			}			
		}
		else if (stop.compare(command)  == 0)
		{
			left = 0;
			right = 0;
		std::cout<<"Stop"<<std::endl;
			
		}
		else if (right_str.compare(command)  == 0)
		{
		std::cout<<"Right"<<std::endl;
			if (-1 < left && left < 1)
			{
				left = left + 0.1;
				if (left > 1)
				{
					left = 1;
				}
				else if (left < -1)
				{
					left = -1;
				}
			}
		}
		else if (reverse.compare(command)  == 0)
		{
		std::cout<<"Reverse"<<std::endl;
			if (-1 < left && left < 1)
			{
				left = left - 0.1;
				if (left > 1)
				{
					left = 1;
				}
				else if (left < -1)
				{
					left = -1;
				}
			}
			if (-1 < right && right < 1)
			{
				right = right - 0.1;				
				if (right > 1)
				{
					right = 1;
				}
				else if (right < -1)
				{
					right = -1;
				}		
			}	
		}	   
	   std::cout<<"After: " << left << ", " << right << std::endl;
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
			SuspendThread();
//or set suspend_flag to true to let vision thread suspend itself?
		}
		else{
			ResumeThread();
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

	cv::Mat image;
	std::vector<uchar> buffer;
	int x = 0;
	while(1){
		// have flag indicating when Mat data is ready to be sent to socket: while(flag == false) {wait}
		//std::cout<<x<<std::endl;
		if (camera){
		if (x==0){
			x=x+1;
			image = cv::imread((resources_dir+"1.jpg").c_str());              // read test image
		}
		else if (x==1){
			x++;
			image = cv::imread((resources_dir+"2.jpg").c_str());              // read test image
		}
		else if (x==2){
			x++;
			image = cv::imread((resources_dir+"3.jpg").c_str());              // read test image
		}
		else if (x==3){
			x++;
			image = cv::imread((resources_dir+"4.jpg").c_str());              // read test image
		}
		else if (x==4){
			x++;
			image = cv::imread((resources_dir+"5.jpg").c_str());              // read test image
		}
		else if (x==5){
			x++;
			image = cv::imread((resources_dir+"6.jpg").c_str());              // read test image
		}
		else if (x==6){
			x++;
			image = cv::imread((resources_dir+"7.jpg").c_str());              // read test image
		}
		else if (x==7){
			x=0;
			image = cv::imread((resources_dir+"8.jpg").c_str());              // read test image
		}
	        //image = cv::imread((resources_dir+"example.jpg").c_str());              // read test image
		if (image.data == NULL)
		{
			std::cout<<"Image not found"<<std::endl;
		}
		else{
			ResizeImage(&image);                                                      // resize to 300 x 240
			//cv::imwrite( (resources_dir+"resized.jpg"), image);
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

void ResizeImage(cv::Mat* src){
	resize(*src, *src, cv::Size(150,94), 0, 0, cv::INTER_NEAREST); // resize an image to standard size
}
cmake_minimum_required(VERSION 2.8)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake" )

# Add sources here... #
add_executable (cv main.cpp pixy_getframe.cpp hsvhelper.cpp imagefunctions.cpp filefunctions.cpp 
	vision.cpp motorcontrol.cpp line.cpp control.cpp drawmarker.cpp pca9685.c ELEC3850_client.cpp 
	base64.cpp)

# libpixyusb should always come before libboost and libusb #
target_link_libraries (cv pixyusb)

find_package ( libpixyusb REQUIRED )
find_package ( libusb-1.0 REQUIRED )
find_package ( Boost 1.49 COMPONENTS thread system chrono REQUIRED)
find_package ( OpenCV REQUIRED )
find_package(SDL2 REQUIRED)


find_library(wiringPi_LIB wiringPi)
target_link_libraries (cv ${Boost_LIBRARIES})
target_link_libraries (cv ${LIBUSB_1_LIBRARY})
target_link_libraries (cv ${OpenCV_LIBS})
target_link_libraries(cv ${wiringPi_LIB})
target_link_libraries (cv sioclient)
target_link_libraries(cv ${SDL2_LIBRARIES})


include_directories (src
                    include
		     		${OpenCV_INCLUDE_DIRS}
                    ../common
                    ${Boost_INCLUDE_DIR}
                    ${LIBUSB_1_INCLUDE_DIRS}
                    ${SDL2_INCLUDE_DIRS})
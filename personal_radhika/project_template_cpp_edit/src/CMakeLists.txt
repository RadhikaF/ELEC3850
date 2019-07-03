cmake_minimum_required(VERSION 2.8)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake" )

add_executable(myProg main.cpp ELEC3850_client.cpp base64.cpp)

target_link_libraries(myProg sioclient)
target_link_libraries(myProg pthread)

find_package ( Boost 1.49 COMPONENTS thread system chrono REQUIRED)
find_package ( OpenCV REQUIRED )

target_link_libraries (myProg ${Boost_LIBRARIES})
target_link_libraries (myProg ${OpenCV_LIBS})

include_directories (src
                    include
		     		${OpenCV_INCLUDE_DIRS}
                    ../common
                    ${Boost_INCLUDE_DIR})
# Install script for directory: /home/pi/nodetest/project_template_cpp_edit/lib/socket.io-client-cpp

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/pi/nodetest/project_template_cpp_edit/lib/socket.io-client-cpp/build/include/sio_client.h;/home/pi/nodetest/project_template_cpp_edit/lib/socket.io-client-cpp/build/include/sio_message.h;/home/pi/nodetest/project_template_cpp_edit/lib/socket.io-client-cpp/build/include/sio_socket.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/pi/nodetest/project_template_cpp_edit/lib/socket.io-client-cpp/build/include" TYPE FILE FILES
    "/home/pi/nodetest/project_template_cpp_edit/lib/socket.io-client-cpp/src/sio_client.h"
    "/home/pi/nodetest/project_template_cpp_edit/lib/socket.io-client-cpp/src/sio_message.h"
    "/home/pi/nodetest/project_template_cpp_edit/lib/socket.io-client-cpp/src/sio_socket.h"
    )
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/pi/nodetest/project_template_cpp_edit/lib/socket.io-client-cpp/build/lib/Release/libsioclient.a")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/pi/nodetest/project_template_cpp_edit/lib/socket.io-client-cpp/build/lib/Release" TYPE STATIC_LIBRARY FILES "/home/pi/nodetest/project_template_cpp_edit/lib/socket.io-client-cpp/libsioclient.a")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/pi/nodetest/project_template_cpp_edit/lib/socket.io-client-cpp/build/lib/Release/libboost_system.a;/home/pi/nodetest/project_template_cpp_edit/lib/socket.io-client-cpp/build/lib/Release/libboost_date_time.a;/home/pi/nodetest/project_template_cpp_edit/lib/socket.io-client-cpp/build/lib/Release/libboost_random.a")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/pi/nodetest/project_template_cpp_edit/lib/socket.io-client-cpp/build/lib/Release" TYPE FILE FILES
    "/usr/lib/arm-linux-gnueabihf/libboost_system.a"
    "/usr/lib/arm-linux-gnueabihf/libboost_date_time.a"
    "/usr/lib/arm-linux-gnueabihf/libboost_random.a"
    )
endif()


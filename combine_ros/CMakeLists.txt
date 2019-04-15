cmake_minimum_required(VERSION 2.8)
project(combine_ros)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  geometry_msgs
  darknet_ros_msgs
  sensor_msgs
  cv_bridge
  pcl_ros
  pcl_conversions
  image_transport
  gazebo_ros
  nav_msgs
)

catkin_package( 
CATKIN_DEPENDS
  geometry_msgs
  roscpp
  rospy
  sensor_msgs
  std_msgs
  darknet_ros_msgs 
  cv_bridge
  pcl_ros
  pcl_conversions
  image_transport
  gazebo_ros
  nav_msgs
)
include_directories(
  include
  devel/include/darknet_ros_msgs
  ${catkin_INCLUDE_DIRS}
  ${GAZEBO_INCLUDE_DIRS} ${SDFormat_INCLUDE_DIRS}
)

add_library(get_function SHARED src/get_function.cpp)

#pcl
find_package(PCL REQUIRED COMPONENT common io)
include_directories(include ${PCL_INCLUDE_DIRS})
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})

#eigen
include_directories("/usr/include/eigen3/")

#cv
set(CMAKE_CXX_FLAGS "-std=c++11")
find_package(OpenCV 3 REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

#gazebo
find_package(gazebo REQUIRED)


add_executable(${PROJECT_NAME}
  src/combine.cpp
  src/get_function.cpp
  src/get_function.h
  )
target_link_libraries(${PROJECT_NAME} 
  ${catkin_LIBRARIES}
  get_function
  ${PCL_LIBRARIES}
  ${OpenCV_LIBS}

)



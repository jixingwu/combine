#ifndef GET_FUNCTION_H
#define GET_FUNCTION_H

#include <iostream>
#include <string>

#include "ros/ros.h"
#include "std_msgs/String.h"
#include "std_msgs/Float32.h"
#include "geometry_msgs/Pose.h" 	//audio  "/sound_direction"
#include "darknet_ros_msgs/BoundingBoxes.h"//camera "/darknet_ros/bounding_boxes"
#include "darknet_ros_msgs/BoundingBox.h"
#include "sensor_msgs/PointCloud2.h"//TOF    "/smarttof/pointcloud"
#include "sensor_msgs/PointCloud.h"
#include "sensor_msgs/Image.h"
#include "geometry_msgs/Twist.h"	//robot  "/RosAria/cmd_vel"
#include "nav_msgs/Odometry.h"
#include <pcl/point_types.h>
//#include <pcl_conversions/pcl_conversions.h>
#include <pcl/visualization/cloud_viewer.h>
#include <pcl/point_cloud.h>
#include <pcl/filters/voxel_grid.h>
//Eigen 部分
#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Geometry>
//opencv
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
//#include <image_transport/image_transport.h>


using namespace std;

struct xyCoo
{
  int xmin, xmax, ymin, ymax;
};

class SubscribeAndPublish
{
public:
  SubscribeAndPublish();
  ~SubscribeAndPublish();
  void StartReadingLoop();

private:
  ros::NodeHandle RosNodeHandler;//private handler for GET_FUNCTION_H
 
  //subscribe
  ros::Subscriber sound_sub;
  ros::Subscriber camera_sub;
  ros::Subscriber pointcloud_sub;
  ros::Subscriber Imagedist_sub;
  ros::Subscriber odome_sub;
  //publish
  ros::Publisher  obsavoidPub;
  ros::Publisher  cmdVelPub;
  //handler
  ros::NodeHandle _np;

  const std::string Object = "person";//Object
  //相机内参
  double cx = 162.00374;
  double cy = 110.05493;
  double fx = 319.33169;
  double fy = 319.25846;
  double scale = 1.0;
   float D;
  float altha = 1.0;
  float beta  = 1.0;
	
  pcl::PointXYZRGB p;

  bool reP = false;//用于标定camera是否检测到人，检测到值为1，未检测到值为0
  bool T = false;// 动作并未执行
  bool sdT = false;
  bool hearT = false;
	
  xyCoo objectBox = {0, 0, 0, 0};
	
  //CallBack
  void soundCallback(const geometry_msgs::Pose& pose);
  void cameraCallback(const darknet_ros_msgs::BoundingBoxes& bounding_boxes);
  void pointcloudCallback(const sensor_msgs::PointCloud2& pointcloud);
  void imagedistCallback(const sensor_msgs::Image& image_dist);
  void odomCallback(const nav_msgs::OdometryConstPtr& odom);
  //Function
  double getDistance(float x, float y);
  void grayDistance(int u, int v, unsigned int d);
  float kinectDepth(int objectBoxX, int objectBoxY, int cols, uchar* depth);
  void rgbDistance(int u, int v, unsigned int d);
  void TOFDistance(int64 u, int64 v, unsigned int d);

  //function
  void ControlRotation(const float sPoseOrien[]);
  void BackControlRotation(const float sPoseOrien[]);
  void findObject(const darknet_ros_msgs::BoundingBoxes& bounding_boxes);
  void getObjectPosition(const sensor_msgs::Image& image_dist, xyCoo objectBox );	
};

#endif

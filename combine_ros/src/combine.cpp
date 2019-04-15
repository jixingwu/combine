 #include <iostream>
#include "ros/ros.h"
#include "std_msgs/String.h"
#include "std_msgs/Float32.h"
#include "geometry_msgs/Pose.h"//audio topic is "/sound_direction"
#include "darknet_ros_msgs/BoundingBoxes.h" //camera topic is "/darknet_ros/bounding_boxes".
#include "sensor_msgs/PointCloud2.h"//TOF topic is "/smarttof/pointcloud".
#include "geometry_msgs/Twist.h"//robot topic is "/RosAria/cmd_vel" which depend on gazebo.

#include "get_function.h"

using namespace std;



	//start
	/*ros::init(argc, argv, "combine_nodes");
	ros::NodeHandle RosNodeHandler;

	ros::Publisher sound = RosNodeHandler.advertise<std_msgs::Float32>("sound_topic",1);
	ros::Publisher camera   = RosNodeHandler.advertise<sensor_msgs::Image>("camera_topic",1);
	//ros::Publisher TOF   = RosNodeHandler.advertise<dmcam_ros_main>("TOF_topic",1);

	ros:Rate loop_rate(10);

	while(ros::ok())
	{

	}*/

	
	/*void soundCallback(const std_msgs::Float32& sound)
	{
		//publish sound node
		ros::init(argc, argv, "sound_node");
		ros::NodeHandle soundNodeHandle;

		ros::Publisher sound_node_pub = soundNodeHandle.advertise<std::Float32>("sound_node", 1);

		ros::Rate loop_rate(10);

		while(ros::ok())
		{
			std::Float32 msg;
			msg.data = sound.data;


		}
	}


	void cameraCallback(const sensor_msgs::Image& camera)
	{} 
	*/
	//void TOFCallback(const )



	int main(int argc, char** argv)
	{
		//overview
		ROS_INFO("[INFO]Starting Combine sound, camera and TOF nodes");//start
		ros::init(argc, argv, "combine");//combine is the node name
// 		ros::NodeHandle _np;//creat a handler
// 		p3atObstacleAvoid roshandler(_np);

		SubscribeAndPublish GetFunction_handler;

		GetFunction_handler.StartReadingLoop();
		
		//Finally
		//ROS_INFO("[INFO]Finshed!");
		ros::spin();//when Ctrl -C is pressed, will be shutdown.
		return 0;

	}




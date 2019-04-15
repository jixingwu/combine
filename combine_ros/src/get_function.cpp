#include "get_function.h"
#include <string>
#define Speed 1.0 //机器人的速度，单位：m/s（线速度） or rad/s(角速度)
#define rate  100 //ros自循环频率

float odomLinear[3]={0,0,0};
float sPoseOrien[4]={1,0,0,0};// {w, x, y, z}

SubscribeAndPublish::SubscribeAndPublish()
{}
SubscribeAndPublish::~SubscribeAndPublish()
{}

void SubscribeAndPublish::odomCallback(const nav_msgs::OdometryConstPtr& odom)
{
  odomLinear[2] = odom->twist.twist.angular.z;
}


//旋转控制函数，用于sound回调函数,若sysmbol为正，则逆时针旋转，反之顺时针。
void SubscribeAndPublish::ControlRotation(const float sPoseOrien[])
{
/*Pose
 * geometry_msgs/Point position 
 * [float64 x, float64 y, float64 z]
 * geometry_msgs/Quaternion orientation 
 * [float64 x, float64 y, float64 z, float64 w]  
*/
/*Twist
 * Vector3  linear
 * [float64 x, float64 y, float64 z]
 * Vector3  angular
 * [float64 x, float64 y, float64 z]
 */

  ros::Rate loopRate(rate);
  //计算出需要旋转的角度，单位：rad
  geometry_msgs::Twist rotation;
  //如果angular_speed为正即symbol=1，则逆时针旋转，反之顺时针。
  double angular_speed = 1.0; //rad/s
  cout<<"sPoseOrien[0]="<<sPoseOrien[0]<<endl;
  double goal_angle = 2*acos((double)sPoseOrien[0]);// rad
  double angular_duration = goal_angle/angular_speed;// s
  cout<<"goal_angle= "<<goal_angle<<"rad="<<goal_angle*180/M_PI<<endl;
  rotation.linear.x  = 0;
  rotation.angular.z = angular_speed;
	
  int ticks = int( abs(angular_duration) * rate) + 35;
  cout<<"ticks:"<<ticks<<endl;

  for(int i = 0; i < ticks; i++)
  {
    cmdVelPub.publish(rotation);//// notice!!!
    loopRate.sleep(); 
  }
	
  rotation.angular.z = 0;
  cmdVelPub.publish(geometry_msgs::Twist());
  sdT = true;	
}

void SubscribeAndPublish::BackControlRotation(const float sPoseOrien[])
{
	ros::Rate loopRate(rate);
	//计算出需要旋转的角度，单位：rad
	geometry_msgs::Twist rotation;
	//如果angular_speed为正即symbol=1，则逆时针旋转，反之顺时针。
	double angular_speed =  -1; //rad/s
	
	cout<<"Back: sPoseOrien[0]="<<sPoseOrien[0]<<endl;
	
	double goal_angle = 2*acos((double)sPoseOrien[0]);// rad
	
	double angular_duration = goal_angle/angular_speed;// s
	rotation.linear.x  = 0;
	rotation.angular.z = angular_speed;
	
	int ticks = int( abs(angular_duration) * rate) + 35;

	for(int i = 0; i < ticks; i++)
	{
	  cmdVelPub.publish(rotation);//// notice!!!
	  loopRate.sleep(); 
	}
	
	rotation.angular.z = 0;
	cmdVelPub.publish(geometry_msgs::Twist());	

}

double SubscribeAndPublish::getDistance(float x, float y)
{
	float dis = pow((pow(x,2)+pow(y,2)),0.5);
	return dis; 
}

void SubscribeAndPublish::soundCallback(const geometry_msgs::Pose& pose)
{

	  ROS_INFO("hear a sound <<<<<");
	  sPoseOrien[0] = pose.orientation.w;

	  hearT = true;
	  ControlRotation(sPoseOrien);//rotation 
}

void SubscribeAndPublish::findObject(const darknet_ros_msgs::BoundingBoxes& bounding_boxes)
{
  int bbLength = bounding_boxes.bounding_boxes.size();
 ////获取数组长度函数，用于获取boundingbox的个数
  for(int i=0; i<bbLength; i++)
  {
    if(bounding_boxes.bounding_boxes[i].Class == Object)
    {
      //检测标记
      reP = true;//检测到人返回1，并跳出for循环，有一个人存在即可
      //确定目标边界
      objectBox.xmax = bounding_boxes.bounding_boxes[i].xmax;
      objectBox.xmin = bounding_boxes.bounding_boxes[i].xmin;
      objectBox.ymax = bounding_boxes.bounding_boxes[i].ymax;
      objectBox.ymin = bounding_boxes.bounding_boxes[i].ymin;

      break;  
     }
    }
    
 if(sdT == true && reP == false) 
  {
    cout<<"接收到声音，没有识别到目标" <<endl;
    BackControlRotation(sPoseOrien);//rotation back
    sdT = false;
  }
  
}

void SubscribeAndPublish::cameraCallback(const darknet_ros_msgs::BoundingBoxes& bounding_boxes)
{
  findObject(bounding_boxes);// get the reP //see	    
}


void SubscribeAndPublish::TOFDistance(int64 u, int64 v, unsigned int d)
{
  p.z = (float)(d)/scale; 
  p.x = (u-cx)*p.z/fx;
  p.y = (v-cy)*p.z/fy; 
}

float SubscribeAndPublish::kinectDepth(int objectBoxX, int objectBoxY, int cols, uchar* depth)
{
  return ((float*)depth)[objectBoxY*(cols)+objectBoxX];
}

void SubscribeAndPublish::getObjectPosition(const sensor_msgs::Image& image_dist, xyCoo objectBox )
{
  int objectBoxX = (objectBox.xmax + objectBox.xmin)/2;
  //int objectBoxY = (objectBox.ymin + objectBox.ymax)/2;
  int objectBoxY = objectBox.ymin;
  //Image to Opencv
  cv_bridge::CvImagePtr cv_ptr;
  cv_ptr = cv_bridge::toCvCopy(image_dist, sensor_msgs::image_encodings::TYPE_32FC1);

  cout<<"v=rows:"<<cv_ptr->image.rows<<endl<<"u=cols:"<<cv_ptr->image.cols<<endl;
  
  for(int rows=0; rows<cv_ptr->image.rows; rows++)
  {
    for(int cols=0; cols<cv_ptr->image.cols; cols++)
    {     
      if(rows == objectBoxY && cols == objectBoxX)
      {

	cout<<"objectBoxY:"<<objectBoxY<<" "<<"objectBoxX:"<<objectBoxX<<endl;

	cout<<"isContinuous:"<<cv_ptr->image.isContinuous()<<endl;

	float d = kinectDepth(objectBoxX, objectBoxY, cv_ptr->image.cols, cv_ptr->image.data);
	cout<<"d:"<<d<<endl;
	D = d;
      }     
    }
  }
}

void SubscribeAndPublish::imagedistCallback(const sensor_msgs::Image& image_dist)
{
  //第一次执行且识别到人条件成立
  //if(odomLinear[2] > 0.001) cout<<"odom->twist.twist.angular.z="<<odomLinear[2]<<endl;
  /*只能进入一次action 也就是再次测试需要重新launch 
   *测试通过， 但返回的obsavoid.position.x 值不准确 需要修改算法
   */
  if(T == false && odomLinear[2] < 0.001 && reP == true /*&& hearT == true*/)// T = 0; reP = 1/
  {  
    cout<<"entered the action"<<endl;
    cout<<"get the person position.."<<endl;
    //得到目标位置p
    cout<<"objectBoundingBox = "<<endl<<objectBox.xmin<<" "<<objectBox.xmax<<" "<<endl
	<<objectBox.ymin<<" "<<objectBox.ymax<<" "<<endl;	      

    cout<<"接收到声音并且识别到目标"<<endl;
    getObjectPosition(image_dist, objectBox);// get p.x, p.y
	      
    float theta = 0;
    string space = " ";
	      
    geometry_msgs::Pose obsavoid;
    obsavoid.position.x = altha * D;
    obsavoid.position.y = beta * theta; 
    obsavoid.position.z = 0;
    obsavoid.orientation.x = obsavoid.orientation.y = obsavoid.orientation.z = obsavoid.orientation.w = 0;
    
    /*cout<<"obsavoid.position ="
    <<obsavoid.position.x<<space<<obsavoid.position.y<<space<<obsavoid.position.z<<"; "
    <<"obsavoid.orientation ="
    <<obsavoid.orientation.x<<space<<obsavoid.orientation.y<<space<<obsavoid.orientation.z<<space<<obsavoid.orientation.w <<endl;
    */
    obsavoidPub.publish(obsavoid); 
    T = true;
  }
}

void SubscribeAndPublish::StartReadingLoop()
{ 
  camera_sub = RosNodeHandler.subscribe("/bounding_boxes", 10, &SubscribeAndPublish::cameraCallback,this);
  Imagedist_sub = RosNodeHandler.subscribe("/image_dist", 10, &SubscribeAndPublish::imagedistCallback,this);
  odome_sub = RosNodeHandler.subscribe("/odom", 10, &SubscribeAndPublish::odomCallback,this);
  sound_sub  = RosNodeHandler.subscribe("/sound_direction", 10, &SubscribeAndPublish::soundCallback,this);
  	  
  cmdVelPub   = RosNodeHandler.advertise<geometry_msgs::Twist>("/cmd_vel", 1);
  obsavoidPub = RosNodeHandler.advertise<geometry_msgs::Pose> ("/targetP", 1);
  ROS_INFO("Published..");	
}

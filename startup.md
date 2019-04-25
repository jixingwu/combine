# 示例应用

## 基于3D传感器的地面机器人局部路径规划
### 示例介绍
![示例介绍](https://github.com/jixingwu/combine/blob/master/example.png)

机器人的初始状态是朝向任意方向并处于静止。首先调用**声源定位模块**，当人物发出一个在可识别范围内的声音后，机器人发生旋转并朝向声源方向。然后调用**目标识别模块**，根据是否存在人物执行不同的动作。如果能够识别到人物，调用**导航模块**，机器人移动前进并避开与人物之间的障碍物,到达目标身边。如果并未识别到人物，则机器人回转到初始状态。

### 环境配置
1. 声源定位模块`launch`文件：[respeaker_ros/launch](https://github.com/furushchev/respeaker_ros/tree/master/launch)
2. 目标识别模块`launch`文件：[darknet_ros/launch](https://github.com/leggedrobotics/darknet_ros/tree/master/darknet_ros/launch)
3. 导航模块`launch`文件：[obsavoid/launch](https://github.com/jixingwu/obsavoid/tree/master/obsavoid/launch)
4. 机器人底盘`launch`文件：[jiaolong_ros_wrapper/launch](https://github.com/NLS-SJTU/jiaolong_ros_wrapper/tree/master/launch)
5. 深度相机TOF的驱动`launch`文件：[dmcam_ros/launch](https://github.com/smarttofsdk/SDK/tree/master/ros/src/dmcam_ros/launch)

### 运行启动
1. 使用[zed_camera](http://192.168.22.201/software/sensors/ai_robot_sensors/startup.html#zed-camera)或[Cartograpther](http://192.168.22.201/software/localization/cartographer/startup.html#id3)启动场景中的定位：
    ```bash
	roslaunch zed_nodelet_example zed_nodelet_laserscan.launch
    ```
2. 对底盘的控制:
	```bash
	roslaunch jiaolong_ros_wrapper start.launch
	```
3. [手柄](http://192.168.22.201/software/navigation/startup.html#id8)控制，用于对机器人的保护：
	```bash
	roslaunch ai_robot_navigation justmove.launch
	```
4. 声源定位模块：
	```bash
	roslaunch respeaker_ros respeaker.launch
	```
5. 目标识别模块:
    * 修改darknet_ros/config/ros.yaml中camera_reading收听的topic:
	```bash
	/zed/left/image_raw_color
	```
    * 启动zed_camera和yolo：
	```bash
	roslaunch zed_cpu_ros zed_cpu_ros.launch
	roslaunch darknet_ros yolo_v3.launch
	```
6. 驱动深度相机TOF：
	```bash
	roslaunch dmcam_ros start.launch
	```

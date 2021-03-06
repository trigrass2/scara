
#include "ros/ros.h"
#include "controller/JointInfo.h"
#include "scara.h"
#include "geometry_msgs/Pose.h"
#include "geometry_msgs/Point.h"
/*
 * Globale Variables
*/
controller::JointInfo JointInfo;
geometry_msgs::Pose endPoint;

/*
 * Callback Function:
 * Calc the forward kinematics of SCARA robot.
 *
*/
void callback( const controller::JointInfo::ConstPtr& msg )
{
  //  ROS_INFO("JointInfo Callback");
  int id=0;
  for(id=0;id<5;id++)
    {
      JointInfo.Status[id]=msg->Status[id];
      JointInfo.Pos[id]=msg->Pos[id];      
      JointInfo.Vel[id]=msg->Vel[id];
      JointInfo.Torque[id]=msg->Torque[id];
    }
}


/*
 * Main Function:
*/
int main( int argc, char **argv )
{
  ros::init(argc,argv,"f_kin_scara");
  ros::NodeHandle n;
  ros::Publisher fkin_pub = n.advertise<geometry_msgs::Pose>("endPoint",5);
  /*
  n.setParam("Joint1MaxPos",109991.0);
  n.setParam("Joint1MinPos",-106409.0);
  n.setParam("Joint2MaxPos",86135.0);
  n.setParam("Joint2MinPos",-82367.0);
  n.setParam("Joint3ZeroPos",114.0);
  n.setParam("Joint4ZeroPos",-66176.0);
  */
  float Joint1MaxPos=109991;
  float Joint1MinPos=-106409;
  float Joint2MaxPos=86135;
  float Joint2MinPos=-82367;
  float Joint1ZeroPos=(Joint1MaxPos+Joint1MinPos)/2;
  float Joint2ZeroPos=(Joint2MaxPos+Joint2MinPos)/2;
  float Joint3ZeroPos=114;
  float Joint4ZeroPos=-66176;
  float L1 = 225;//length of arm 1, in mm
  float L2 = 125;//length of arm 2, in mm

  float JointAngle[5]={0,0,0,0,0}; // in degree
  float JointA[5]={0,0,0,0,0};// in 
  ros::Subscriber sub = n.subscribe("JointInfo", 10,callback);
  JointInfo.Status.resize(5);
  JointInfo.Pos.resize(5);
  JointInfo.Vel.resize(5);
  JointInfo.Torque.resize(5);  
  ros::Rate loop_rate(100);
  ROS_INFO("Forward Kinematics Started");
  while(ros::ok())
    {
      JointAngle[1]=(JointInfo.Pos[1]-Joint1ZeroPos)*JOINT1_RESOLUTION+90;// degree
      JointAngle[2]=(JointInfo.Pos[2]-Joint2ZeroPos)*JOINT2_RESOLUTION;// degree
      JointAngle[3]=(JointInfo.Pos[3]-Joint3ZeroPos)*JOINT3_RESOLUTION;// mm
      JointAngle[4]=(JointInfo.Pos[4]-Joint4ZeroPos)*JOINT4_RESOLUTION;// degree
      JointA[1]=JointAngle[1]/180*3.1415926;
      JointA[2]=JointAngle[2]/180*3.1415926;
      JointA[4]=JointAngle[4]/180*3.1415926;

      endPoint.position.x = L1*cos(JointA[1])+L2*cos(JointA[1]+JointA[2]);// mm
      endPoint.position.y = L1*sin(JointA[1])+L2*sin(JointA[1]+JointA[2]);// mm
      endPoint.position.z = JointAngle[4]*15.0/360.0-JointAngle[3];// mm

      //ROS_INFO("x:%f",endPoint.position.x);
      //ROS_INFO("y:%f",endPoint.position.y);
      //ROS_INFO("JointAngle:%f",JointAngle[3]);
      //ROS_INFO("z:%f",endPoint.position.z);
      fkin_pub.publish(endPoint);
      ros::spinOnce();
      loop_rate.sleep();
    }
  return 0;
}

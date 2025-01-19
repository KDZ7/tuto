#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "moveit/move_group_interface/move_group_interface.hpp"
#include "tf2/LinearMath/Quaternion.hpp"

class Pose : public rclcpp::Node
{
public:
  Pose() : Node("pose", rclcpp::NodeOptions().automatically_declare_parameters_from_overrides(true))
  {
    group_ = this->get_parameter_or("group_", std::string("arm_group"));
    x_ = this->get_parameter_or("x", 0.0);
    y_ = this->get_parameter_or("y", 0.0);
    z_ = this->get_parameter_or("z", 0.0);
    roll_ = this->get_parameter_or("roll", 0.0);
    pitch_ = this->get_parameter_or("pitch", 0.0);
    yaw_ = this->get_parameter_or("yaw", 0.0);
    time_ = this->get_parameter_or("time", 1.0);
    attempt_ = this->get_parameter_or("attempt", 1.0);
    scale_velocity_ = this->get_parameter_or("scale_velocity", 1.0);
    scale_acceleration_ = this->get_parameter_or("scale_acceleration", 1.0);
    direct = this->get_parameter_or("direct", false);
    solver_attempt_ = this->get_parameter_or("solver_attempt", 3);

    RCLCPP_INFO(this->get_logger(), "Set Kinematics Parameters");
    this->declare_parameter("robot_description_kinematics." + group_ + ".kinematics_solver", "cached_ik_kinematics_plugin/CachedKDLKinematicsPlugin");
    this->declare_parameter("robot_description_kinematics." + group_ + ".kinematics_solver_search_resolution", 0.005);
    this->declare_parameter("robot_description_kinematics." + group_ + ".kinematics_solver_timeout", 0.005);
    this->declare_parameter("robot_description_kinematics." + group_ + ".kinematics_solver_attempts", solver_attempt_);
  }

  void
  execute()
  {
    moveit::planning_interface::MoveGroupInterface move_group(shared_from_this(), group_);
    move_group.setPlanningTime(time_);
    move_group.setNumPlanningAttempts(attempt_);
    move_group.setMaxVelocityScalingFactor(scale_velocity_);
    move_group.setMaxAccelerationScalingFactor(scale_acceleration_);

    geometry_msgs::msg::Pose target_pose;
    target_pose.position.x = x_;
    target_pose.position.y = y_;
    target_pose.position.z = z_;
    tf2::Quaternion q;
    q.setRPY(roll_, pitch_, yaw_);
    target_pose.orientation.x = q.x();
    target_pose.orientation.y = q.y();
    target_pose.orientation.z = q.z();
    target_pose.orientation.w = q.w();

    move_group.setPoseTarget(target_pose);

    if (direct)
    {
      move_group.move();
      RCLCPP_INFO(this->get_logger(), "Pose is moved");
    }
    else
    {
      RCLCPP_INFO(this->get_logger(), "Planning to move to pose [%f, %f, %f, %f, %f, %f]", x_, y_, z_, roll_, pitch_, yaw_);
      moveit::planning_interface::MoveGroupInterface::Plan plan;
      bool success = static_cast<bool>(move_group.plan(plan));
      if (success)
      {
        move_group.execute(plan);
        RCLCPP_INFO(this->get_logger(), "Pose goal succeeded");
      }
      else
      {
        RCLCPP_ERROR(this->get_logger(), "Pose goal failed");
      }
    }
  }

private:
  std::string group_;
  double x_;
  double y_;
  double z_;
  double roll_;
  double pitch_;
  double yaw_;
  double time_;
  double attempt_;
  double scale_velocity_;
  double scale_acceleration_;
  bool direct;
  int solver_attempt_;
};

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<Pose>();
  node->execute();
  rclcpp::shutdown();
  return 0;
}
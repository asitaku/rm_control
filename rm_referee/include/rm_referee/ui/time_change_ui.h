//
// Created by llljjjqqq on 22-11-4.
//

#pragma once

#include "rm_referee/ui/ui_base.h"

namespace rm_referee
{
class TimeChangeUi : public UiBase
{
public:
  explicit TimeChangeUi(XmlRpc::XmlRpcValue& rpc_value, Base& base, const std::string& graph_name,
                        std::vector<Graph>* graph_queue)
    : UiBase(rpc_value, base, graph_queue)
  {
    graph_ = new Graph(rpc_value["config"], base_, id_++);
  }
  void update() override;
  void updateForQueue();
  virtual void updateConfig(){};
};

class TimeChangeGroupUi : public GroupUiBase
{
public:
  explicit TimeChangeGroupUi(XmlRpc::XmlRpcValue& rpc_value, Base& base, const std::string& graph_name,
                             std::vector<Graph>* graph_queue)
    : GroupUiBase(rpc_value, base, graph_queue)
  {
    graph_name_ = graph_name;
  }
  void update() override;
  void updateForQueue();
  virtual void updateConfig(){};

protected:
  std::string graph_name_;
};

class CapacitorTimeChangeUi : public TimeChangeUi
{
public:
  explicit CapacitorTimeChangeUi(XmlRpc::XmlRpcValue& rpc_value, Base& base, std::vector<Graph>* graph_queue)
    : TimeChangeUi(rpc_value, base, "capacitor", graph_queue){};
  void add() override;
  void updateCapacityData(const rm_msgs::CapacityData data, const ros::Time& time);

private:
  void updateConfig() override;
  double cap_power_;
};

class EffortTimeChangeUi : public TimeChangeUi
{
public:
  explicit EffortTimeChangeUi(XmlRpc::XmlRpcValue& rpc_value, Base& base, std::vector<Graph>* graph_queue)
    : TimeChangeUi(rpc_value, base, "effort", graph_queue){};
  void updateJointStateData(const sensor_msgs::JointState::ConstPtr data, const ros::Time& time);

private:
  void updateConfig() override;
  double joint_effort_;
  std::string joint_name_;
};

class ProgressTimeChangeUi : public TimeChangeUi
{
public:
  explicit ProgressTimeChangeUi(XmlRpc::XmlRpcValue& rpc_value, Base& base, std::vector<Graph>* graph_queue)
    : TimeChangeUi(rpc_value, base, "progress", graph_queue){};
  void updateEngineerUiData(const rm_msgs::EngineerUi::ConstPtr data, const ros::Time& last_get_data_time);

private:
  void updateConfig() override;
  uint32_t finished_data_, total_steps_;
  std::string step_name_;
};

class DartStatusTimeChangeUi : public TimeChangeUi
{
public:
  explicit DartStatusTimeChangeUi(XmlRpc::XmlRpcValue& rpc_value, Base& base, std::vector<Graph>* graph_queue)
    : TimeChangeUi(rpc_value, base, "dart", graph_queue){};
  void updateDartClientCmd(const rm_msgs::DartClientCmd::ConstPtr data, const ros::Time& last_get_data_time);

private:
  void updateConfig() override;
  uint8_t dart_launch_opening_status_;
};

class RotationTimeChangeUi : public TimeChangeUi
{
public:
  explicit RotationTimeChangeUi(XmlRpc::XmlRpcValue& rpc_value, Base& base, std::vector<Graph>* graph_queue)
    : TimeChangeUi(rpc_value, base, "rotation", graph_queue)
  {
    if (rpc_value.hasMember("data"))
    {
      XmlRpc::XmlRpcValue data = rpc_value["data"];
      try
      {
        arc_scale_ = static_cast<int>(data["scale"]);
        gimbal_reference_frame_ = static_cast<std::string>(data["gimbal_reference_frame"]);
        chassis_reference_frame_ = static_cast<std::string>(data["chassis_reference_frame"]);
      }
      catch (XmlRpc::XmlRpcException& e)
      {
        ROS_FATAL_STREAM("Exception raised by XmlRpc while reading the "
                         << "configuration: " << e.getMessage() << ".\n"
                         << "Please check configuration is exit");
      }
    }
    else
      ROS_WARN("RotationTimeChangeUi config 's member 'data' not defined.");
  };

private:
  void updateConfig() override;
  int arc_scale_;
  std::string gimbal_reference_frame_, chassis_reference_frame_;
};

class LaneLineTimeChangeGroupUi : public TimeChangeGroupUi
{
public:
  explicit LaneLineTimeChangeGroupUi(XmlRpc::XmlRpcValue& rpc_value, Base& base, std::vector<Graph>* graph_queue)
    : TimeChangeGroupUi(rpc_value, base, "lane_line", graph_queue)
  {
    if (rpc_value.hasMember("data"))
    {
      XmlRpc::XmlRpcValue& data = rpc_value["data"];
      robot_radius_ = data["radius"];
      robot_height_ = data["height"];
      camera_range_ = data["camera_range"];
      surface_coefficient_ = data["surface_coefficient"];
    }
    else
      ROS_WARN("LaneLineTimeChangeGroupUi config 's member 'data' not defined.");

    if (rpc_value.hasMember("reference_joint"))
    {
      reference_joint_ = static_cast<std::string>(rpc_value["reference_joint"]);
    }
    else
      ROS_WARN("LaneLineTimeChangeGroupUi config 's member 'reference_joint' not defined.");

    graph_vector_.insert(
        std::pair<std::string, Graph*>(graph_name_ + "_left", new Graph(rpc_value["config"], base_, id_++)));
    graph_vector_.insert(
        std::pair<std::string, Graph*>(graph_name_ + "_right", new Graph(rpc_value["config"], base_, id_++)));

    for (auto it : graph_vector_)
      lane_line_double_graph_.push_back(it.second);
  }
  void updateJointStateData(const sensor_msgs::JointState::ConstPtr data, const ros::Time& time);

protected:
  std::string reference_joint_;
  double robot_radius_, robot_height_, camera_range_, surface_coefficient_ = 0.5;
  double pitch_angle_ = 0., screen_x_ = 1920, screen_y_ = 1080;
  double end_point_a_angle_, end_point_b_angle_;

private:
  void updateConfig() override;

  std::vector<Graph*> lane_line_double_graph_;
};

}  // namespace rm_referee

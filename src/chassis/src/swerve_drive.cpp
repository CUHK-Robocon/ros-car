#include <boost/qvm/vec.hpp>
#include <memory>
#include <numbers>

#include "active_caster_chassis.h"
#include "chassis.h"
#include "geometry_msgs/msg/twist.hpp"
#include "rclcpp/rclcpp.hpp"
#include "vesccom/socketcan.h"

class MinimalSubscriber : public rclcpp::Node {
 public:
  MinimalSubscriber(chassis& chs) : Node("swerve_drive"), chs_(chs) {
    subscription_ = this->create_subscription<geometry_msgs::msg::Twist>(
        "cmd_vel", 10,
        std::bind(&MinimalSubscriber::topic_callback, this,
                  std::placeholders::_1));
    RCLCPP_INFO(this->get_logger(), "vel_from_joy node has been started.");
  }

 private:
  void topic_callback(const geometry_msgs::msg::Twist::UniquePtr msg) {
    auto x = msg->linear.x;
    auto y = msg->linear.y;
    auto rx = msg->angular.z;

    RCLCPP_INFO(this->get_logger(),
                "Received from cmd_vel: x=%.2f, y=%.2f, rx=%.2f", x, y, rx);

    boost::qvm::vec<double, 2> vel{x, y};
    chs_.set_vel(vel);
    double avel;
    if (rx) {
      avel = -rx * std::numbers::pi;
    } else {
      avel = 0.0;
    }
    chs_.set_ang_vel(avel);

    chs_.routine_handler();
  };

  rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr subscription_;
  chassis& chs_;
};

int main(int argc, char* argv[]) {
  vesccom::socketcan::master mch1("can0");
  vesccom::socketcan::slave rfs(mch1, 1);
  vesccom::socketcan::slave rfd(mch1, 2);
  vesccom::socketcan::slave lfs(mch1, 3);
  vesccom::socketcan::slave lfd(mch1, 4);

  vesccom::socketcan::master mch2("can1");
  vesccom::socketcan::slave lrs(mch2, 5);
  vesccom::socketcan::slave lrd(mch2, 6);
  vesccom::socketcan::slave rrs(mch2, 7);
  vesccom::socketcan::slave rrd(mch2, 8);

  vesccom::vesc::start_keep_alive_thread();
  mch1.start_monitor_thread();
  mch2.start_monitor_thread();

  mch1.wait_all_ready();
  mch2.wait_all_ready();

  active_caster_chassis chs_impl(0.05, {
                                           {.steering = lfs,
                                            .driving = lfd,
                                            .driving_pole_pairs = 21,
                                            .offset = {-0.195, 0.195}},
                                           {.steering = rfs,
                                            .driving = rfd,
                                            .driving_pole_pairs = 21,
                                            .offset = {0.195, 0.195}},
                                           {.steering = lrs,
                                            .driving = lrd,
                                            .driving_pole_pairs = 21,
                                            .offset = {-0.195, -0.195}},
                                           {.steering = rrs,
                                            .driving = rrd,
                                            .driving_pole_pairs = 21,
                                            .offset = {0.195, -0.195}},
                                       });
  chassis& chs = chs_impl;

  chs_impl.set_zero();

  rclcpp::init(argc, argv);

  rclcpp::spin(std::make_shared<MinimalSubscriber>(chs));
  rclcpp::shutdown();
  return 0;
}
// TODO: make safety - e.g. if cmd_vel dies and topic callback is no longer
// called, call another function to send 0 to chs routine hander
#include <boost/qvm/vec.hpp>
#include <memory>
#include <numbers>

#include "active_caster_chassis.h"
#include "chassis.h"
#include "geometry_msgs/msg/twist.hpp"
#include "sensor_msgs/msg/joy.hpp"
#include "rclcpp/rclcpp.hpp"
#include "vesccom/socketcan.h"

class MinimalSubscriber : public rclcpp::Node {
 public:
  MinimalSubscriber(active_caster_chassis& chs) : Node("swerve_drive"), chs_(chs) {
    cmd_vel_subscription_ =
        this->create_subscription<geometry_msgs::msg::Twist>(
            "cmd_vel", 10,
            std::bind(&MinimalSubscriber::cmd_vel_callback, this,
                      std::placeholders::_1));

    joy_subscription_ = this->create_subscription<sensor_msgs::msg::Joy>(
        "joy", 10,
        std::bind(&MinimalSubscriber::joy_callback, this,
                  std::placeholders::_1));

    RCLCPP_INFO(this->get_logger(), "swerve_drive node has been started.");
  }

 private:
  void cmd_vel_callback(const geometry_msgs::msg::Twist::UniquePtr msg) {
    auto x = msg->linear.x;
    auto y = msg->linear.y;
    auto rx = msg->angular.z;

    // RCLCPP_INFO(this->get_logger(),
    //             "Received from cmd_vel: x=%.2f, y=%.2f, rx=%.2f", x, y, rx);

    // boost::qvm::vec<double, 2> vel{x, y};
    // chs_.set_vel(vel);
    // double avel;
    // if (rx) {
    //   avel = rx * std::numbers::pi;
    // } else {
    //   avel = 0.0;
    // }
    // chs_.set_ang_vel(avel);

    // chs_.routine_handler();

    if (btn_lmenu_) {
      RCLCPP_INFO(this->get_logger(), "Left Menu button pressed: Resetting chassis.");
      chs_.reset();
    } else if (btn_cross_) {
      RCLCPP_INFO(this->get_logger(), "Cross button pressed: Locking wheels.");
      chs_.lock_wheels();
    } else {
      boost::qvm::vec<double, 2> vel{x, y};
      chs_.set_vel(vel);
      double avel;
      if (rx) {
        avel = rx * std::numbers::pi;
      } else {
        avel = 0.0;
      }
      chs_.set_ang_vel(avel);

      chs_.routine_handler();
    }
  };

  void joy_callback(const sensor_msgs::msg::Joy::SharedPtr msg) {
    btn_cross_ = msg->buttons[0];
    btn_lmenu_ = msg->buttons[8];
    std::cout << "btn_cross: " << btn_cross_ << ", btn_lmenu: " << btn_lmenu_ << std::endl;
  }

  rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr
      cmd_vel_subscription_;
  active_caster_chassis& chs_;
  rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr joy_subscription_;
  bool btn_cross_ = false;
  bool btn_lmenu_ = false;
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

  std::cout << "waiting all ready" << std::endl;
  mch1.wait_all_ready();
  mch2.wait_all_ready();
  std::cout << "all ready" << std::endl;

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

  chs_impl.set_zero();

  rclcpp::init(argc, argv);

  rclcpp::spin(std::make_shared<MinimalSubscriber>(chs_impl));
  rclcpp::shutdown();
  return 0;
}
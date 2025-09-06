#include <memory>

#include "geometry_msgs/msg/twist.hpp"
#include "rclcpp/rclcpp.hpp"

class MinimalSubscriber : public rclcpp::Node {
 public:
  MinimalSubscriber() : Node("swerve_drive") {
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
  };
  rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr subscription_;
};

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MinimalSubscriber>());
  rclcpp::shutdown();
  return 0;
}
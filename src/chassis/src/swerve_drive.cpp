#include <memory>

#include "geometry_msgs/msg/twist.hpp"
#include "rclcpp/rclcpp.hpp"

class MinimalSubscriber : public rclcpp::Node {
 public:
  MinimalSubscriber() : Node("swerve_drive") {
    auto topic_callback =
        [this](geometry_msgs::msg::Twist::UniquePtr msg) -> void {
      auto x = msg->linear.x;
      auto y = msg->linear.y;
      auto rx = msg->angular.z;

      RCLCPP_INFO(this->get_logger(),
                  "Received from cmd_vel: x=%.2f, y=%.2f, rx=%.2f", x, y, rx);
    };
    subscription_ = this->create_subscription<geometry_msgs::msg::Twist>(
        "cmd_vel", 10, topic_callback);
  }

 private:
  rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr subscription_;
};

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MinimalSubscriber>());
  rclcpp::shutdown();
  return 0;
}
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/joy.hpp"

class MinimalSubscriber : public rclcpp::Node
{
public:
  MinimalSubscriber()
  : Node("minimal_subscriber")
  {
    auto topic_callback =
      [this](sensor_msgs::msg::Joy::UniquePtr msg) -> void {
        RCLCPP_INFO(this->get_logger(), "left_x: '%f'", msg->axes[0]);
      };
    subscription_ =
      this->create_subscription<sensor_msgs::msg::Joy>("joy", 10, topic_callback);
  }

private:
  rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr subscription_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MinimalSubscriber>());
  rclcpp::shutdown();
  return 0;
}
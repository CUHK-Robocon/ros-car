// TODO: maybe make a publisher to publish the mapped joystick output

#include <memory>

#include "deadzone.h"
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/joy.hpp"
#include "geometry_msgs/msg/twist.hpp"

using namespace std::chrono_literals;

class JoyToVelNode : public rclcpp::Node {
 public:
  JoyToVelNode() : Node("vel_from_joy") {
    publisher_ =
        this->create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 10);

    subscription_ = this->create_subscription<sensor_msgs::msg::Joy>(
        "joy", 10,
        std::bind(&JoyToVelNode::joy_callback, this, std::placeholders::_1));

    RCLCPP_INFO(this->get_logger(), "Joy to Vel node has been started.");
  }

 private:
  void joy_callback(const sensor_msgs::msg::Joy::SharedPtr msg) {
    auto axis_left_x = -msg->axes[0];
    auto axis_left_y = msg->axes[1];
    auto axis_right_x = -msg->axes[3];
    auto axis_right_y = msg->axes[4];

    auto btn_cross = msg->buttons[0];
    auto btn_circle = msg->buttons[1];
    auto btn_triangle = msg->buttons[2];
    auto btn_square = msg->buttons[3];

    auto btn_lmenu = msg->buttons[8];
    auto btn_mmenu = msg->buttons[10];
    auto btn_rmenu = msg->buttons[9];

    auto btn_lstick = msg->buttons[11];
    auto btn_rstick = msg->buttons[12];

    auto btn_l1 = msg->buttons[4];
    auto btn_r1 = msg->buttons[5];
    auto btn_l2 = -(msg->axes[2] - 1) / 2;
    auto btn_r2 = -(msg->axes[5] - 1) / 2;

    int btn_left = 0;
    int btn_right = 0;
    int btn_up = 0;
    int btn_down = 0;

    auto dpad_ud = msg->axes[7];
    auto dpad_lr = msg->axes[6];

    btn_up = dpad_ud == 1;
    btn_down = dpad_ud == -1;
    btn_left = dpad_lr == 1;
    btn_right = dpad_lr == -1;

    double scale = 1 + 8 * btn_r2;

    double x = controller_deadzone(axis_left_x, 0.1) * scale;
    double y = controller_deadzone(axis_left_y, 0.1) * scale;
    double rx = controller_deadzone(axis_right_x, 0.1);

    RCLCPP_INFO(this->get_logger(), "x: '%f', y: '%f', rx: '%f'", x, y, rx);

    auto twist_msg = geometry_msgs::msg::Twist();
    twist_msg.linear.x = x;
    twist_msg.linear.y = y;
    twist_msg.angular.z = rx;

    RCLCPP_INFO(
        this->get_logger(),
        "Publishing to /cmd_vel: linear.x=%.2f, linear.y=%.2f, angular.z=%.2f",
        twist_msg.linear.x, twist_msg.linear.y, twist_msg.angular.z);

    publisher_->publish(twist_msg);
  }
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
  rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr subscription_;
};

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<JoyToVelNode>());
  rclcpp::shutdown();
  return 0;
}
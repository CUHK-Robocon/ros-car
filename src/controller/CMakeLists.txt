cmake_minimum_required(VERSION 3.5)
project(controller)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)

add_executable(vel_from_joy src/vel_from_joy.cpp src/deadzone.cpp)
target_link_libraries(vel_from_joy PUBLIC rclcpp::rclcpp ${sensor_msgs_TARGETS})

install(TARGETS
  vel_from_joy
  DESTINATION lib/${PROJECT_NAME})

ament_package()
cmake_minimum_required(VERSION 3.5)
project(chassis)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
endif()

# if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
#   add_compile_options(-Wall -Wextra -Wpedantic)
# endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(fmt REQUIRED)
find_package(Boost REQUIRED)
find_package(sensor_msgs REQUIRED)

add_subdirectory(vesccom EXCLUDE_FROM_ALL)

set(MAIN_SBC_LIBRARIES
    vesccom
    Boost::headers
    fmt::fmt
)

add_executable(swerve_drive src/swerve_drive.cpp src/active_caster_chassis.cpp)
target_link_libraries(
  swerve_drive 
  PUBLIC 
    rclcpp::rclcpp 
    ${geometry_msgs_TARGETS}
    ${MAIN_SBC_LIBRARIES}
    ${sensor_msgs_TARGETS}
)

install(TARGETS
  swerve_drive
  DESTINATION lib/${PROJECT_NAME})

ament_package()
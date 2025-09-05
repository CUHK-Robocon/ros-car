#ifndef ROS_CAR_SRC_UTIL_H_
#define ROS_CAR_SRC_UTIL_H_

template <typename T>
T sign(T v) {
  return (v > 0) - (v < 0);
}

#endif

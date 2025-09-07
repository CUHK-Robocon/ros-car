#ifndef ROS_CAR_SRC_CHASSIS_UTIL_H_
#define ROS_CAR_SRC_CHASSIS_UTIL_H_

#include <numbers>

inline double rad_to_deg(double rad) { return rad * 180 / std::numbers::pi; }

inline double deg_to_rad(double deg) { return deg * std::numbers::pi / 180; }

// Normalizes `angle` to [`start`, `end`].
inline double norm(double angle, double start, double end) {
  assert(end > start);
  double range = end - start;
  double offset = angle - start;
  double base = std::floor(offset / range) * range;
  return offset - base + start;
}

// Normalizes `angle` to [-180, 180].
inline double norm_pm180(double angle) { return norm(angle, -180, 180); }

#endif

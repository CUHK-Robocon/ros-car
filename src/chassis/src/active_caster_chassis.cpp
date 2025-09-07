#include "active_caster_chassis.h"

#include <numbers>

#include "spdlog/spdlog.h"
#include "util.h"

void active_caster_chassis::set_zero() {
  for (auto& wheel_set : wheel_sets_) wheel_set.steering.set_zero(90);
}

void active_caster_chassis::reset() {
  for (const auto& wheel_set : wheel_sets_) {
    wheel_set.steering.set_pos_full(90);
    wheel_set.driving.set_erpm(0);
  }
}

void active_caster_chassis::lock_wheels() {
  for (const auto& wheel_set : wheel_sets_) {
    double x = boost::qvm::X(wheel_set.offset);
    double y = boost::qvm::Y(wheel_set.offset);

    double current_deg_full = wheel_set.steering.get_pid_pos_full();

    double first_ang = rad_to_deg(std::atan2(y, x)) - current_deg_full;
    double second_ang = first_ang + 180;
    first_ang = norm_pm180(first_ang);
    second_ang = norm_pm180(second_ang);

    double deg_diff =
        std::abs(first_ang) > std::abs(second_ang) ? second_ang : first_ang;
    wheel_set.steering.set_pos_full(current_deg_full + deg_diff);
    wheel_set.driving.set_erpm(0);
  }
}

void active_caster_chassis::routine_handler_unlocked() {
  if (boost::qvm::mag(vel_unlocked()) == 0 && ang_vel_unlocked() == 0) {
    for (const auto& wheel_set : wheel_sets_) wheel_set.driving.set_erpm(0);
    return;
  }

  for (const auto& wheel_set : wheel_sets_) {
    auto [wheel_ang, wheel_vel] = wheel_ang_and_vel(wheel_set.offset);

    double wheel_circumference = 2 * std::numbers::pi * wheel_radius_;
    double wheel_rpm = wheel_vel / wheel_circumference * 60;
    double wheel_erpm = wheel_rpm * wheel_set.driving_pole_pairs;

    double wheel_deg = rad_to_deg(wheel_ang);
    double current_deg_full = wheel_set.steering.get_pid_pos_full();

    double first_ang = wheel_deg - current_deg_full;
    double second_ang = first_ang + 180;
    first_ang = norm_pm180(first_ang);
    second_ang = norm_pm180(second_ang);

    bool is_second_better = std::abs(first_ang) > std::abs(second_ang);
    double erpm_sign = is_second_better ? -1 : 1;
    double deg_diff = is_second_better ? second_ang : first_ang;

    wheel_set.steering.set_pos_full(current_deg_full + deg_diff);
    wheel_set.driving.set_erpm(erpm_sign * wheel_erpm);
  }
}

boost::qvm::vec<double, 2> active_caster_chassis::r_vec() {
  double ang_vel = ang_vel_unlocked();

  if (ang_vel == 0) return {0, 0};

  boost::qvm::vec<double, 2> r_vec = vel_unlocked() / std::abs(ang_vel);

  if (ang_vel > 0) {
    double x = boost::qvm::X(r_vec);
    boost::qvm::X(r_vec) = boost::qvm::Y(r_vec);
    boost::qvm::Y(r_vec) = -x;
  }

  if (ang_vel < 0) {
    double x = boost::qvm::X(r_vec);
    boost::qvm::X(r_vec) = -boost::qvm::Y(r_vec);
    boost::qvm::Y(r_vec) = x;
  }

  return r_vec;
}

std::pair<double, double> active_caster_chassis::wheel_ang_and_vel(
    boost::qvm::vec<double, 2> wheel_offset) {
  double wheel_ang;
  double wheel_vel;

  boost::qvm::vec<double, 2> vel = vel_unlocked();
  double vel_ang = std::atan2(boost::qvm::Y(vel), boost::qvm::X(vel));

  double ang_vel = ang_vel_unlocked();

  boost::qvm::vec<double, 2> r_v = r_vec();

  if (ang_vel == 0) {
    wheel_ang = vel_ang;
    wheel_vel = boost::qvm::mag(vel);
  } else if (r_v == boost::qvm::zero_vec<double, 2>()) {
    double wheel_offset_ang =
        std::atan2(boost::qvm::Y(wheel_offset), boost::qvm::X(wheel_offset));

    if (ang_vel > 0) wheel_ang = wheel_offset_ang + std::numbers::pi / 2;
    if (ang_vel < 0) wheel_ang = wheel_offset_ang - std::numbers::pi / 2;

    wheel_vel = boost::qvm::mag(wheel_offset) * std::abs(ang_vel);
  } else {
    boost::qvm::vec<double, 2> r_w_v = r_v + wheel_offset;

    double det = boost::qvm::X(r_v) * boost::qvm::Y(r_w_v) -
                 boost::qvm::Y(r_v) * boost::qvm::X(r_w_v);
    double dot = boost::qvm::dot(r_v, r_w_v);

    double r_to_r_w_ang_diff = std::atan2(det, dot);
    wheel_ang = vel_ang + r_to_r_w_ang_diff;

    wheel_vel = boost::qvm::mag(r_w_v) * std::abs(ang_vel);
  }

  return {wheel_ang, wheel_vel};
}

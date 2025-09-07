#ifndef ROS_CAR_SRC_CHASSIS_ACTIVE_CASTER_CHASSIS_H_
#define ROS_CAR_SRC_CHASSIS_ACTIVE_CASTER_CHASSIS_H_

#include <initializer_list>
#include <utility>
#include <vector>

#include "boost/qvm/all.hpp"
#include "chassis.h"
#include "vesccom/vesc.h"

struct active_caster_wheel_set {
  vesccom::vesc& steering;
  vesccom::vesc& driving;
  int driving_pole_pairs;
  boost::qvm::vec<double, 2> offset;
};

class active_caster_chassis : public chassis {
 public:
  active_caster_chassis(
      double wheel_radius,
      std::initializer_list<active_caster_wheel_set> wheel_sets)
      : wheel_radius_(wheel_radius), wheel_sets_(wheel_sets) {}

  void set_zero();
  void reset();
  void lock_wheels();

 protected:
  void routine_handler_unlocked() override;

 private:
  // Computes the vector from the center of turning circle to the center of
  // chassis.
  boost::qvm::vec<double, 2> r_vec();

  std::pair<double, double> wheel_ang_and_vel(
      boost::qvm::vec<double, 2> wheel_offset);

  double wheel_radius_;
  std::vector<active_caster_wheel_set> wheel_sets_;
};

#endif

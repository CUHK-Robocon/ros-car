#ifndef ROS_CAR_SRC_CHASSIS_CHASSIS_H_
#define ROS_CAR_SRC_CHASSIS_CHASSIS_H_

#include <cmath>
#include <mutex>

#include "boost/qvm/all.hpp"
#include "util.h"

class chassis {
 public:
  virtual ~chassis() {}

  void routine_handler() {
    std::lock_guard<std::mutex> lock(mutex_);
    routine_handler_unlocked();
  }

  boost::qvm::vec<double, 2> vel() {
    std::lock_guard<std::mutex> lock(mutex_);
    return vel_;
  }

  void set_vel(boost::qvm::vec<double, 2> new_vel) {
    std::lock_guard<std::mutex> lock(mutex_);
    vel_ = new_vel;
  }

  void set_global_vel(boost::qvm::vec<double, 2> global_vel, double theta) {
    double r = deg_to_rad(-theta);
    boost::qvm::mat<double, 2, 2> rot_mat = {
        std::cos(r),
        -std::sin(r),
        std::sin(r),
        std::cos(r),
    };
    set_vel(rot_mat * global_vel);
  }

  void clear_vel() {
    std::lock_guard<std::mutex> lock(mutex_);
    vel_ = {0, 0};
  }

  double ang_vel() {
    std::lock_guard<std::mutex> lock(mutex_);
    return ang_vel_;
  }

  void set_ang_vel(double new_ang_vel) {
    std::lock_guard<std::mutex> lock(mutex_);
    ang_vel_ = new_ang_vel;
  }

  void clear_ang_vel() {
    std::lock_guard<std::mutex> lock(mutex_);
    ang_vel_ = 0;
  }

  // Allows `std::lock_guard` to be used with this class.
  void lock() { mutex_.lock(); }

  // Allows `std::lock_guard` to be used with this class.
  void unlock() { mutex_.unlock(); }

 protected:
  boost::qvm::vec<double, 2> vel_unlocked() const { return vel_; }
  void set_vel_unlocked(boost::qvm::vec<double, 2> new_vel) { vel_ = new_vel; }
  void clear_vel_unlocked() { vel_ = {0, 0}; }

  double ang_vel_unlocked() const { return ang_vel_; }
  void set_ang_vel_unlocked(double new_ang_vel) { ang_vel_ = new_ang_vel; }
  void clear_ang_vel_unlocked() { ang_vel_ = 0; }

  // Routine to be run in each main loop iteration.
  //
  // Chassis implementations are expected to perform hardware operations e.g.
  // steering, driving in this method. It is recommended to keep the override
  // method `protected`.
  //
  // Note that, the chassis object's mutex will be locked when calling this
  // method. Thus, it is REQUIRED to and safe to use `*_unlocked` method within
  // the handler.
  virtual void routine_handler_unlocked() = 0;

 private:
  boost::qvm::vec<double, 2> vel_ = {0, 0};
  double ang_vel_ = 0;
  std::mutex mutex_;
};

#endif

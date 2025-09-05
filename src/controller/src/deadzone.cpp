#include "deadzone.h"

#include <cmath>

#include "util.h"

double controller_deadzone(double value, double deadzone) {
  if (std::abs(value) < deadzone) {
    value = 0;
  } else {
    value -= sign(value) * deadzone;
    value /= (1 - deadzone);
  }

  return value;
}

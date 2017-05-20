#pragma once

// f(x,y) = (1-x)^2 + 100(y - x^2)^2;
class Rosenbrock {
public:
  template <typename T>
  bool operator()(const T *const x, const T *const y, T *cost) const {
    cost[0] = (T(1.0) - x[0]) * (T(1.0) - x[0]) +
              T(100.0) * (y[0] - x[0] * x[0]) * (y[0] - x[0] * x[0]);
    return true;
  }
};

#include "autodiff_cost_function.h"
#include "rosenbrock.h"

void autodiffTest() {
  auto rosenbrockCostFunction =
      new ceres::AutoDiffCostFunction<Rosenbrock, 1, 1, 1>(new Rosenbrock());
  constexpr double x = 1, y = 3;
  constexpr double epsilon = 1e-16;
  double **parameters = new double *[2];
  parameters[0] = new double;
  parameters[1] = new double;
  parameters[0][0] = x;
  parameters[1][0] = y;

  double **jacobians = new double *[2];
  jacobians[0] = new double;
  jacobians[1] = new double;

  double residuals = 0.0;

  rosenbrockCostFunction->Evaluate(parameters, &residuals, jacobians);

  double cost = (1.0 - x) * (1.0 - x) + 100.0 * (y - x * x) * (y - x * x);
  double gx = -2.0 * (1.0 - x) - 200.0 * (y - x * x) * 2.0 * x;
  double gy = 200.0 * (y - x * x);

  assert(std::abs(cost - residuals) < epsilon);
  assert(std::abs(gx - jacobians[0][0]) < epsilon);
  assert(std::abs(gy - jacobians[1][0]) < epsilon);

  delete rosenbrockCostFunction;
}
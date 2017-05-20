#include "gradientDescent.h"

GradientDescent::GradientDescent(Settings *s) : Optimizer(s) {
  learning_rate_base = settings->lookupFloat("optimization.learning_rate");
  decay = settings->lookupFloat("optimization.decay");
  momentum = settings->lookupFloat("optimization.momentum");
  nestrov = settings->check("optimization.nestrov", "true");
  velocity = 0;
  cout << "Initializing Gradient Descent optimizer complete"
       << " , Nestrov=" << nestrov << " , Momentum=" << momentum
       << " , Decay rate=" << decay << " , learning rate=" << learning_rate_base
       << endl;
}

GradientDescent::~GradientDescent() {}

bool GradientDescent::hasGradientInformation() { return true; }

State *GradientDescent::update(State *u) {
  auto prev = u->getParameter();
  auto iterations = u->getID();

  auto v = new State(parameterDimension, objectiveDimension);
  auto input = new double[parameterDimension]();

  // decayed learning rate:
  auto learning_rate = learning_rate_base * (1.0 / (1.0 + decay * iterations));

  // we store the previous value of velocity to be used in Nestrov's momentum
  // calculation
  previous_velocity = velocity;

  for (int i = 0; i < u->getParameterSize(); i++) {
    auto dx = u->getDerivative(0, i);
    // Momentum update rule
    velocity = momentum * velocity - learning_rate * dx;
    if (nestrov) {
      // Updating the state according to Nestrov momentum
      input[i] =
          prev[i] - momentum * previous_velocity + (1 + momentum) * velocity;
    } else {
      // Descent rule for the Vanilla Gradient Descent (with momentum, but
      // without Nestrov).
      input[i] = prev[i] + velocity;
    }
  }
  // clipParameters(input);
  v->setParameter(input);
  return v;
}

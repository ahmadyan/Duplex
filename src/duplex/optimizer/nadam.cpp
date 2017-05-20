#include "nadam.h"
#include <cmath>

Nadam::Nadam(Settings *s) : Optimizer(s) {
  learning_rate_base = settings->lookupFloat("optimization.learning_rate");
  fudgeFactor = settings->lookupFloat("optimization.fudge_factor");
  exponential_decay_rates_beta1 =
      settings->lookupFloat("optimization.exponential_decay_rates_beta1");
  exponential_decay_rates_beta2 =
      settings->lookupFloat("optimization.exponential_decay_rates_beta2");
  schedule_decay = settings->lookupFloat("optimization.schedule_decay");
  moment = new double[parameterDimension]();
  velocity = new double[parameterDimension]();
  m_schedule = 1;
  decay_beta1 = exponential_decay_rates_beta1;
  decay_beta2 = exponential_decay_rates_beta2;
  cout << "Initializing Nadam optimizer complete"
       << ", learning rate = " << learning_rate_base
       << ", exponential_decay_rates_beta1 = " << exponential_decay_rates_beta1
       << ", exponential_decay_rates_beta2 = " << exponential_decay_rates_beta2
       << ", schedule decay = " << schedule_decay
       << ", fudge factor = " << fudgeFactor << endl;
}

Nadam::~Nadam() {}

bool Nadam::hasGradientInformation() { return true; }
State *Nadam::update(State *u) {
  iterations++;
  prev = u->getParameter();
  auto v = new State(parameterDimension, objectiveDimension);
  auto input = new double[parameterDimension]();
  decay_beta1 *= exponential_decay_rates_beta1;
  decay_beta2 *= exponential_decay_rates_beta2;
  auto learning_rate = learning_rate_base;
  // Get gradients w.r.t. stochastic objective
  dx = u->getDerivativeVector(0);

  // warming up the momentum, it is recommended to keep these values hard-coded
  auto momentum_cache_t = exponential_decay_rates_beta1 *
                          (1.0 - 0.5 * pow(0.96, iterations * schedule_decay));
  auto momentum_cache_t_1 =
      exponential_decay_rates_beta1 *
      (1. - 0.5 * (pow(0.96, (iterations + 1) * schedule_decay)));
  auto m_schedule_new = m_schedule * momentum_cache_t;
  auto m_schedule_next = m_schedule * momentum_cache_t * momentum_cache_t_1;

  vector<double> g_prime(parameterDimension);
  vector<double> m_t(parameterDimension);
  vector<double> m_t_prime(parameterDimension);
  vector<double> v_t(parameterDimension);
  vector<double> v_t_prime(parameterDimension);
  vector<double> m_t_bar(parameterDimension);

  for (int i = 0; i < parameterDimension; i++) {
    g_prime[i] = dx[i] / (1.0 - m_schedule_new);
    m_t[i] = exponential_decay_rates_beta1 * moment[i] +
             (1. - exponential_decay_rates_beta1) * dx[i];
    m_t_prime[i] = m_t[i] / (1. - m_schedule_next);
    v_t[i] = exponential_decay_rates_beta2 * velocity[i] +
             (1.0 - exponential_decay_rates_beta2) * dx[i] * dx[i];
    v_t_prime[i] =
        v_t[i] / (1. - pow(exponential_decay_rates_beta2, iterations));
    m_t_bar[i] = (1. - momentum_cache_t) * g_prime[i] +
                 momentum_cache_t_1 * m_t_prime[i];
  }

  for (int i = 0; i < parameterDimension; i++) {
    moment[i] = m_t[i];
    velocity[i] = v_t[i];
    input[i] = prev[i] -
               learning_rate * m_t_bar[i] / (sqrt(v_t_prime[i]) + fudgeFactor);
  }

  m_schedule = m_schedule_new;

  clipParameters(input);
  v->setParameter(input);

  return v;
}

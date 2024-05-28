#pragma once
#include <functional>

namespace dbb
{

  // Trapezoidal Rule for numerical integration
  //------------------------------------------------------------------------------
  double integrate(std::function<double(double)> func, double a, double b, int n)
  {
    double h = (b - a) / n; // Width of each trapezoid
    double integral = 0.0;

    // Sum the areas of each trapezoid
    for (int i = 0; i <= n; ++i) {
      double x = a + i * h;
      if (i == 0 || i == n) {
        integral += 0.5 * func(x);
      }
      else {
        integral += func(x);
      }
    }

    integral *= h;
    return integral;
  }

  // Central difference method for numerical differentiation
  //------------------------------------------------------------------------------
  double differentiate(std::function<double(double)> func, double x, double h = 1e-5)
  {
    return (func(x + h) - func(x - h)) / (2 * h);
  }

  // Function to create a numerical derivative function
  //------------------------------------------------------------------------------
  std::function<double(double)> make_derivative(std::function<double(double)> func, double h = 1e-5)
  {
    return [func, h](double x) {
      return (func(x + h) - func(x - h)) / (2 * h);
    };
  }
}
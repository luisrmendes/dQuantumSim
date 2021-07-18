#pragma once
#ifndef MULTIQUBIT_H
#define MULTIQUBIT_H

#include "operations.h"
#include <complex>
#include <math.h>
#include <variant>
#include <vector>

/**
 * Defines entangled qubits
 */
class MultiQubit
{
  private:
	int id;
	std::vector<std::complex<double>> state = {0, 0, 0, 0};

  public:
};

#endif
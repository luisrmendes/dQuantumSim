#pragma once
#ifndef MULTIQUBIT_H
#define MULTIQUBIT_H

#include "operations.h"
#include <complex>
#include <iostream>
#include <math.h>
#include <variant>
#include <vector>

/**
 * Defines entangled qubits
 */
class MultiQubit
{
  private:
	std::vector<std::complex<double>> state;

  public:
	MultiQubit(std::vector<std::complex<double>> state) { this->state = state; };
	void setState(std::vector<std::complex<double>> state) { this->state = state; };
	std::vector<std::complex<double>> getState() { return this->state; };
	void printState();
};

#endif
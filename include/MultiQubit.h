#pragma once
#ifndef MULTIQUBIT_H
#define MULTIQUBIT_H

#include "Qubit.h"
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
	
	/**
	 * Applies tensor multiplication between two qubits
	 * Only applies to size 2 vectors (qubit state)
	 */
	static MultiQubit tensorMultiplication(Qubit q1, Qubit q2);
};

#endif
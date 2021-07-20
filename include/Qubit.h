#pragma once
#ifndef QUBIT_H
#define QUBIT_H

#include "operations.h"
#include <complex>
#include <iostream>
#include <math.h>
#include <variant>
#include <vector>

class Qubit
{
  private:
	std::vector<std::complex<double>> state;

  public:
	Qubit(std::vector<std::complex<double>> state) { this->state = state; };
	void setState(std::vector<std::complex<double>> state) { this->state = state; };
	std::vector<std::complex<double>> getState() { return this->state; };
	void printState();
	void pauli_X();
	void pauli_Y();
	void pauli_Z();
	void hadamard();
};

#endif
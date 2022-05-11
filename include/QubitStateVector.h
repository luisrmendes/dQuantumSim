#pragma once
#ifndef QUBITSTATEVECTOR_H
#define QUBITSTATEVECTOR_H

#include <complex>
#include <iostream>
#include <math.h>
#include <variant>
#include <vector>

/**
 * Defines entangled qubits
 */
class QubitStateVector
{
  private:
	std::vector<std::complex<double>> state;
	int numQubits;

  public:
	QubitStateVector(int numQubits);
	void setState(std::vector<std::complex<double>> state) { this->state = state; };
	std::vector<std::complex<double>> getState() { return this->state; };
	void printState();

	void pauliX(int targetQubit);
	void pauliY(int targetQubit);
	void pauliZ(int targetQubit);
	void hadamard(int targetQubit);
	void controlledNot();
};

#endif
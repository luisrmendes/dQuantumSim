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
	int id;
	std::vector<std::complex<double>> state;

  public:
	Qubit(int id, std::vector<std::complex<double>> state)
	{
		this->id = id;
		this->state = state;
	};
	int getId() { return id; };
	void setState(std::vector<std::complex<double>> state) { this->state = state; };
	std::vector<std::complex<double>> getState() { return this->state; };
	void printState();
	void pauli_X();
	void pauli_Y();
	void pauli_Z();
	void hadamard();
};

#endif
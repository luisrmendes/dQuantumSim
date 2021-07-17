#pragma once
#ifndef QUBIT_H
#	define QUBIT_H

#include <vector>
#include <math.h>
#include <complex>
#include <variant>
#include "operations.h"

class Qubit
{ 
  private:
	int id;
	std::vector<double> state = {0, 1};

  public:
	Qubit(int id) { this->id = id; };
	int getId() { return id; };
	void setState(std::vector<double> state) { this->state = state; };
	std::vector<double> getState() { return this->state; };
	void printState();
	void pauli_X();
	void pauli_Y();
	void pauli_Z();
};

#endif
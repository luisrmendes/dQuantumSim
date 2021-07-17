#pragma once
#ifndef QUBIT_H
#	define QUBIT_H

#include <vector>
#include <math.h>
#include "operations.h"

using namespace std;


class Qubit
{
  private:
	int id;
	vector<double> state = {0, 1};

  public:
	Qubit(int id) { this->id = id; };
	int getId() { return id; };
	void setState(vector<double> state) { this->state = state; };
	vector<double> getState() { return this->state; };
	void printState();
	void pauli_X();
	void pauli_Y();
	void pauli_Z();
};

#endif
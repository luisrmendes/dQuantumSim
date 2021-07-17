#pragma once
#ifndef QUBIT_H
#	define QUBIT_H

#include <vector>

using namespace std;

class Qubit
{
  private:
	int id;
	vector<double> state = {1, 0};

  public:
	Qubit(int id);
	int getId() { return id; };
	void setState(vector<double> state) { this->state = state; };
	vector<double> getState() { return this->state; };
	void printState();
};

#endif
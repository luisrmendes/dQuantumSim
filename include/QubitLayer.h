#pragma once
#ifndef QUBITLAYER_H
#define QUBITLAYER_H

#include <complex>
#include <iostream>
#include <vector>

typedef std::vector<std::complex<double>> qubitStates;

class QubitLayer
{
  private:
	qubitStates state;

  public:
	QubitLayer(int qubitCount);
	void pauliX(int targetQubit);
	void printStates();
};

#endif

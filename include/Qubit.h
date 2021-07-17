#pragma once
#ifndef QUBIT_H
#	define QUBIT_H

class Qubit
{
  private:
	int id;
	int state[1][2] = {0, 0};

  public:
	Qubit(int id);
	int getId() { return id; };
	typedef int (*state_pointer)[2];
	state_pointer getState() { return state; };
	void printState();
};

#endif
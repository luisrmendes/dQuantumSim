#ifndef QUBIT_H
#define QUBIT_H

class Qubit
{
	private:
		int id;

	public:
		Qubit(int id);
		int getId() { return id; };
		int add(int a, int b);
};

#endif
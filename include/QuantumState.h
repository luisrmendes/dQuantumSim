#ifndef QUANTUMSTATE_H
#define QUANTUMSTATE_H
#include <cmath>
#include <iostream>
#include <vector>

#define MASK(N) (0x1ull << N)

class QuantumState
{
  private:
	std::vector<bool> qState;

  public:
	QuantumState();
	QuantumState(size_t number)
	{
		if(number == 0)
			this->qState.push_back(0);
		while(number > 0) {
			this->qState.push_back(number % 2);
			number = number >> 1; // / 2
		}
	}
	void setQGS(std::vector<bool> qState) { this->qState = qState; }
	std::vector<bool> getQGS() { return this->qState; }
	void printState() const;
	void flip(unsigned int index)
	{
		if(index >= this->qState.size())
			throw std::invalid_argument("qState.flip() argument out of bounds!");
		qState[index] = !qState[index];
	}
	bool test(unsigned int index)
	{
		if(index >= this->qState.size())
			throw std::invalid_argument("qState.test() argument out of bounds!");
		return this->qState[index] == 1;
	}

	// convert to a unsigned long long
	uint64_t to_uint64()
	{
		if(this->qState.size() > 64) {
			throw std::overflow_error(
				"Can't convert to uint64 number larger that uint64_t limit!");
			std::cerr << "\tSize of qState: " << this->qState.size() << std::endl;
		}

		uint64_t result = 0;
		for(size_t i = 0; i < this->qState.size(); ++i) {
			result += this->qState[i] * MASK(i);
		}

		return result;
	}

	// overload + operator (size_t + QuantumGlobalState)
	QuantumState operator+(size_t n)
	{
		QuantumState newQGS;

		while(n > 0) {
			newQGS.qState.push_back(n % 2);
			n = n / 2;
		}

		return newQGS;
	}
};

void QuantumState::printState() const
{
	std::cout << '|';
	for(size_t i = this->qState.size(); i > 0; i--) {
		std::cout << this->qState[i - 1];
	}
	std::cout << ">\n";
}

// int main()
// {
// 	uint64_t number = ~0;
// 	std::cout << "Number: " << number << std::endl;

// 	QuantumState qState(number);
// 	qState.printState();
// 	qState.flip(63);
// 	qState.printState();
// 	std::cout << "Converted number: " << qState.to_uint64() << std::endl;

// 	return 0;
// }

#endif
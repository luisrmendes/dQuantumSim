#ifndef QUANTUMSTATE_H
#define QUANTUMSTATE_H

#include <iostream>
#include <vector>

#define MASK(N) (0x1ull << N)

class QuantumState
{
  private:
	std::vector<bool> qState;

  public:
	QuantumState() {}
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
			std::cerr << "Size of qState: " << this->qState.size() << std::endl;
			throw std::overflow_error(
				"Can't convert to uint64 number larger that uint64_t limit!");
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
		QuantumState result;
		QuantumState b(n);

		size_t i = 0;
		bool carry = false;
		for(; i < b.qState.size() || i < this->qState.size(); ++i) {
			bool sum = (this->qState[i] ^ b[i]) ^ carry;
			carry = (this->qState[i] && b[i]) || (this->qState[i] && carry) ||
					(b[i] && carry);
			result.qState.push_back(sum);
		}
		// last carry
		if(carry) {
			bool sum = (this->qState[i] ^ b[i]) ^ carry;
			carry = (this->qState[i] && b[i]) || (this->qState[i] && carry) ||
					(b[i] && carry);
			result.qState.push_back(sum);
		}

		return result;
	}

	std::vector<bool>::reference operator[](size_t n) { return this->qState[n]; }

	bool operator>(size_t n)
	{
		// convert size_t to quantumState
		QuantumState second(n);

		if(this->qState.size() != second.qState.size())
			return (this->qState.size() > second.qState.size());

		bool first_is_greater = false;
		for(size_t i = 0; i < this->qState.size(); ++i) {
			if((this->qState[i] ^ second[i]) == 0x0)
				continue;
			if(this->qState[i] == 1 && second[i] == 0) {
				first_is_greater = true;
				continue;
			}
			if(this->qState[i] == 0 && second[i] == 1)
				first_is_greater = false;
		}

		return first_is_greater;
	}

	bool operator>(QuantumState second)
	{
		if(this->qState.size() != second.qState.size())
			return (this->qState.size() > second.qState.size());

		bool first_is_greater = false;
		for(size_t i = 0; i < this->qState.size(); ++i) {
			if((this->qState[i] ^ second[i]) == 0x0)
				continue;
			if(this->qState[i] == 1 && second[i] == 0) {
				first_is_greater = true;
				continue;
			}
			if(this->qState[i] == 0 && second[i] == 1)
				first_is_greater = false;
		}

		return first_is_greater;
	}

	bool operator<(QuantumState second)
	{
		if(this->qState.size() != second.qState.size())
			return (this->qState.size() < second.qState.size());

		bool first_is_smaller = false;
		for(size_t i = 0; i < this->qState.size(); ++i) {
			if((this->qState[i] ^ second[i]) == 0x0)
				continue;
			if(this->qState[i] == 0 && second[i] == 1) {
				first_is_smaller = true;
				continue;
			}
			if(this->qState[i] == 1 && second[i] == 0)
				first_is_smaller = false;
		}

		return first_is_smaller;
	}

	bool operator<(size_t n)
	{
		// convert size_t to quantumState
		QuantumState second(n);
		if(this->qState.size() != second.qState.size())
			return (this->qState.size() < second.qState.size());

		bool first_is_smaller = false;
		for(size_t i = 0; i < this->qState.size(); ++i) {
			if((this->qState[i] ^ second[i]) == 0x0)
				continue;
			if(this->qState[i] == 0 && second[i] == 1) {
				first_is_smaller = true;
				continue;
			}
			if(this->qState[i] == 1 && second[i] == 0)
				first_is_smaller = false;
		}

		return first_is_smaller;
	}

	bool operator==(QuantumState second)
	{
		if(this->qState.size() != second.qState.size())
			return false;

		for(size_t i = 0; i < this->qState.size(); ++i) {
			if((this->qState[i] ^ second[i]) == 0x1)
				return false;
		}

		return true;
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

// int main() {
//     uint64_t number = 2;
//     // std::cout << "Number: " << number << std::endl;

//     QuantumState state1(0);
//     state1.printState();
//     QuantumState state2(1);
//     state2 = state2 + 2;
//     state2.printState();
//     std::cout << "Converted state1: " << state1.to_uint64() << std::endl;
//     std::cout << "Converted state2: " << state2.to_uint64() << std::endl;

//     std::cout << "Greater than:" << std::endl;
//     if (state1 > state2)
//         std::cout << "\ttrue" << std::endl;
//     else
//         std::cout << "\tfalse" << std::endl;

//     std::cout << "Smaller than:" << std::endl;
//     if (state1 < state2)
//         std::cout << "\ttrue" << std::endl;
//     else
//         std::cout << "\tfalse" << std::endl;

//     std::cout << "Equals:" << std::endl;
//     if (state1 == state2)
//         std::cout << "\ttrue" << std::endl;
//     else
//         std::cout << "\tfalse" << std::endl;

//     return 0;
// }

#endif
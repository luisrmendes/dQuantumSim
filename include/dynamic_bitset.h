#ifndef DYNAMICBITSET_H
#define DYNAMICBITSET_H

#include <iostream>
#include <vector>

class dynamic_bitset
{
  private:
	std::vector<bool> bitset;

  public:
	dynamic_bitset() {}
	dynamic_bitset(uint64_t number);
	std::vector<bool> getBitset() { return this->bitset; }
	
    /**
     * TODO: flip de 1 do mais significativo vira 0. remover 0?
    */
	void flip(size_t index);
	bool test(size_t index);
	uint64_t to_ullong();
	void printBitset();

	dynamic_bitset operator+(dynamic_bitset b);
	dynamic_bitset operator+(uint64_t n);
	dynamic_bitset operator-(dynamic_bitset b);
	dynamic_bitset operator-(uint64_t n);
	void operator>>(uint64_t n);
	void operator<<(uint64_t n);
	std::vector<bool>::reference operator[](uint64_t n);
	bool operator>(dynamic_bitset second);
	bool operator>(uint64_t n);
	bool operator<(dynamic_bitset second);
	bool operator<(uint64_t n);
	bool operator==(dynamic_bitset second);
	bool operator==(uint64_t n);
	bool operator!=(dynamic_bitset second);
	bool operator!=(uint64_t n);
	void operator+=(dynamic_bitset b);
	void operator+=(uint64_t n);
	void operator-=(dynamic_bitset b);
	void operator-=(uint64_t n);
};

#endif
#ifndef DYNAMICBITSET_H
#define DYNAMICBITSET_H

#include <iostream>
#include <string>
#include <vector>

class dynamic_bitset
{
  private:
  public:
	std::vector<bool> bitset;
	static void normalizeBitsets(dynamic_bitset* a, dynamic_bitset* b);
	static void compressBitset(dynamic_bitset* b);
	dynamic_bitset() {}
	dynamic_bitset(uint64_t number);
	std::vector<bool> getBitset() const { return this->bitset; }

	void flip(size_t index);
	bool test(size_t index) const;
	uint64_t to_ullong() const;
	std::string printBitset() const;

	dynamic_bitset operator+(dynamic_bitset b);
	dynamic_bitset operator-(dynamic_bitset b);
	void operator>>(uint64_t n);
	void operator<<(uint64_t n);
	std::vector<bool>::reference operator[](size_t n);
	bool operator>(dynamic_bitset second) const;
	bool operator<(dynamic_bitset second) const;
	bool operator==(dynamic_bitset second) const;
	bool operator!=(dynamic_bitset second) const;
	void operator+=(dynamic_bitset b);
	void operator-=(dynamic_bitset b);
	void operator=(dynamic_bitset b);
	dynamic_bitset operator+(uint64_t n);
	dynamic_bitset operator-(uint64_t n);
	bool operator>(uint64_t n) const;
	bool operator<(uint64_t n) const;
	bool operator==(uint64_t n) const;
	bool operator!=(uint64_t n) const;
	void operator+=(uint64_t n);
	void operator-=(uint64_t n);
	void operator=(uint64_t n);
};

#endif
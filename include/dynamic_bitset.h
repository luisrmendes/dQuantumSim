#ifndef DYNAMICBITSET_H
#define DYNAMICBITSET_H

#include <iostream>
#include <string>
#include <vector>

class dynamic_bitset
{
  private:
	std::vector<bool> bitset;
	static void normalizeBitsets(dynamic_bitset* a, dynamic_bitset* b);
	static void compressBitset(dynamic_bitset* b);

  public:
	dynamic_bitset() {}
	dynamic_bitset(uint64_t number);
	std::string printBitset() const;
	std::string printBitsetFormatted() const;
	uint64_t to_ullong() const;
	void flip(const size_t &index);
	bool test(const size_t &index) const;
	dynamic_bitset operator+(dynamic_bitset b) const;
	dynamic_bitset operator-(dynamic_bitset b) const;
	void operator>>(const uint64_t &n);
	void operator<<(const uint64_t &n);
	std::vector<bool>::reference operator[](const size_t &n);
	bool operator>(const dynamic_bitset &second) const;
	bool operator<(const dynamic_bitset &second) const;
	bool operator>=(const dynamic_bitset &second) const;
	bool operator<=(const dynamic_bitset &second) const;
	bool operator==(const dynamic_bitset &second) const;
	bool operator!=(const dynamic_bitset &second) const;
	void operator+=(const dynamic_bitset &b);
	void operator-=(const dynamic_bitset &b);
	void operator=(const dynamic_bitset &b);
	dynamic_bitset operator+(uint64_t n);
	dynamic_bitset operator-(uint64_t n);
	bool operator>(uint64_t n) const;
	bool operator<(uint64_t n) const;
	bool operator>=(uint64_t n) const;
	bool operator<=(uint64_t n) const;
	bool operator==(uint64_t n) const;
	bool operator!=(uint64_t n) const;
	void operator+=(uint64_t n);
	void operator-=(uint64_t n);
	void operator=(uint64_t n);
	std::vector<bool> getBitset() const { return this->bitset; }
};

#endif
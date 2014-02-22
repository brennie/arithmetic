#ifndef INCLUDE_BIGINT_HPP
#define INCLUDE_BIGINT_HPP

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>


class BigInt
{
public:
	BigInt();
	BigInt(const uint32_t that);
	BigInt(const std::string& str);
	BigInt(const BigInt& that);
	BigInt(BigInt&& that);

	BigInt& operator=(BigInt& that);
	BigInt& operator=(BigInt&& that);

	bool operator==(const BigInt& that) const;
	bool operator==(const uint32_t that) const;

	bool operator!=(const BigInt& that) const;
	bool operator!=(const uint32_t that) const;

	bool operator<(const BigInt& that) const;
	bool operator<(const uint32_t that) const;

	bool operator>(const BigInt& that) const;
	bool operator>(const uint32_t that) const;

	bool operator<=(const BigInt& that) const;
	bool operator<=(const uint32_t that) const;

	bool operator>=(const BigInt& that) const;
	bool operator>=(const uint32_t that) const;

	BigInt operator-() const;
	void negate();

	BigInt operator++();
	BigInt& operator++(const int);

	BigInt operator--();
	BigInt& operator--(const int);

	BigInt operator+(const BigInt& that) const;

	BigInt operator-(const BigInt& that) const;

	BigInt operator/(const BigInt& that) const;
	BigInt operator/(const uint32_t that) const;
	
	BigInt operator%(const BigInt& that) const;
	uint32_t operator%(const uint32_t that) const;

	BigInt operator<<(const uint32_t that) const;

	BigInt operator>>(const uint32_t that) const;

	BigInt& operator+=(const BigInt& that);

	BigInt& operator-=(const BigInt& that);

	BigInt& operator/=(const BigInt& that);
	BigInt& operator/=(const uint32_t that);

	BigInt& operator%=(const BigInt& that);

	BigInt& operator<<=(const uint32_t that);

	BigInt& operator>>=(const uint32_t that);

	operator std::string() const;

	bool isZero() const;
	bool isPositive() const;
	bool isNegative() const;

	size_t size() const;

private:
	typedef std::vector<uint32_t> Words;

	static BigInt zero;
	static BigInt one;
	static BigInt ten;

	bool positive;

	BigInt::Words words;

	BigInt(const BigInt::Words& that);
	BigInt(const BigInt::Words&& that);

	BigInt& operator+=(const BigInt::Words& that);
	
	BigInt& operator-=(const BigInt::Words& that);

	BigInt operator/(const BigInt::Words& that);

	void trim();
};

#endif

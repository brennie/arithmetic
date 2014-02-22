#include "bigint.hpp"

BigInt BigInt::operator%(const BigInt& that) const
{
	BigInt copy(*this);
	copy %= that;
	return copy;
}

uint32_t BigInt::operator%(const uint32_t that) const
{
	uint64_t remainder = 0;

	if (*this < BigInt(that))
		return words[0];

	for (auto word = words.crbegin(); word != words.crend(); ++word)
	{
		remainder = ((remainder << 32) + *word) % that;
	}

	return static_cast<uint32_t>(remainder);
}

BigInt& BigInt::operator%=(const BigInt& that)
{
	bool wasPositive = positive;
	positive = true;

	*this %= that.words;
	std::cout << (std::string)(*this) << std::endl;
	
	if (wasPositive != that.positive)
	{
		*this = that + *this;

		if (!that.positive)
			positive = false;
	}
	else
		positive = wasPositive;

	return *this;
}

BigInt& BigInt::operator%=(const BigInt::Words& that)
{
	BigInt divisor(that);
	size_t shift = size() - divisor.size();

	divisor <<= shift;
	while (divisor > *this)
	{
		divisor >>= 1;
		shift--;
	}

	while (shift)
	{
		if (*this >= divisor)
			*this -= divisor;

		--shift;
		divisor >>= 1;
	}

	if (*this >= divisor)
		*this -= divisor;
	
	return *this;
}


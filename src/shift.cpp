#include <algorithm>

#include "bigint.hpp"

BigInt BigInt::operator<<(const uint32_t that) const
{
	BigInt copy(*this);
	copy <<= that;
	return copy;
}

BigInt BigInt::operator>>(const uint32_t that) const
{
	BigInt copy(*this);
	copy >>= that;
	return copy;
}

BigInt& BigInt::operator<<=(const uint32_t that)
{
	size_t wordShifts = that / 32;
	size_t bitShifts = that % 32;

	if (bitShifts != 0)
	{
		wordShifts++;
		bitShifts = 32 - bitShifts;
	}

	if (wordShifts != 0)
	{
		const size_t newSize = words.size() + wordShifts;

		words.resize(newSize, 0);

		std::copy_backward(words.begin(), words.end() - wordShifts, words.end());

		for (size_t i = newSize - wordShifts; i < newSize; i++)
			words[newSize - i - 1] = 0;
	}

	if (bitShifts != 0)
	{
		*this >>= bitShifts;

		trim();
	}

	return *this;
}

BigInt& BigInt::operator>>=(const uint32_t that)
{
	size_t wordShifts = that / 32;
	size_t bitShifts = that % 32;

	if (that >= size())
	{
		*this = zero;
		return *this;
	}

	if (wordShifts != 0)
	{
		const size_t oldSize = words.size();
		
		std::copy(words.begin() + wordShifts, words.end(), words.begin());

		while (words.size() != oldSize - wordShifts)
			words.pop_back();
	}

	if (bitShifts != 0)
	{
		const uint32_t mask = (1 << bitShifts) - 1;
		uint32_t lastHigh = 0, nextHigh = 0;

		for (auto word = words.rbegin(); word != words.rend(); ++word)
		{
			nextHigh = *word & mask;
			*word >>= bitShifts;
			*word |= lastHigh << (32 - bitShifts);

			lastHigh = nextHigh;
		}
	}

	trim();

	return *this;
}
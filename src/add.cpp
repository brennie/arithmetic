#include "bigint.hpp"

BigInt BigInt::operator++()
{
	BigInt old(*this);
	*this += one;
	return old;
}

BigInt& BigInt::operator++(const int)
{
	*this += one;
	return *this;
}

BigInt BigInt::operator+(const BigInt& that) const
{
	BigInt copy(*this);
	copy += that;
	return copy;
}

BigInt& BigInt::operator+=(const BigInt& that)
{
	if (positive && !that.positive)
		return *this -= that.words;
	else if (!positive && that.positive)
	{
		positive = true;
		/* We have to check if |this| >= that. */
		if (*this >= that)
		{
			*this -= that.words;
			positive = false;
		}
		else
		{
			BigInt copy(that);
			copy -= words;
			*this = std::move(copy);
		}
		
		return *this;
	}
	else
		return *this += that.words;
}

BigInt& BigInt::operator+=(const BigInt::Words& that)
{
	bool thisSmaller = words.size() < that.size();

	const BigInt::Words& larger = thisSmaller ? that : words;
	const BigInt::Words& smaller = thisSmaller ? words : that;

	uint32_t carry = 0;

	for (size_t i = 0; i < larger.size(); i++)
	{
		uint64_t sum;
		if (i < smaller.size())
		{
			sum = static_cast<uint64_t>(carry) + static_cast<uint64_t>(words[i]) + static_cast<uint64_t>(that[i]);

			words[i] = static_cast<uint32_t>(sum);
		}
		else
		{
			sum = static_cast<uint64_t>(carry) + static_cast<uint64_t>(larger[i]);

			if (thisSmaller)
				words.push_back(static_cast<uint32_t>(sum));
			else
				words[i] = static_cast<uint32_t>(sum);
		}
		carry = sum >> 32;
	}

	if (carry != 0)
		words.push_back(carry);

	return *this;
}

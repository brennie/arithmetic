#include "bigint.hpp"

BigInt BigInt::operator*(const BigInt& that) const
{
	BigInt copy(*this);
	copy *= that;
	return copy;
}

BigInt BigInt::operator*(const uint32_t that) const
{
	BigInt copy(*this);
	copy *= that;
	return copy;
}

BigInt& BigInt::operator*=(const BigInt& that)
{
	if (that.size() == 1)
		*this *= that.words.back();
	else
	{
		BigInt result;
		size_t shift = 0;

		result.words.reserve(words.size() * that.size());

		for (auto word : that.words)
		{
			BigInt temp(*this * word);
			temp <<= shift;
			result += temp.words;

			shift += 32;
		}
		result.positive = (positive == that.positive);

		*this = std::move(result);
	}

	return *this;
}

BigInt& BigInt::operator*=(const uint32_t that)
{
	uint32_t carry = 0;
	for (auto& word : words)
	{
		uint64_t product = word;
		product *= that;
		product += carry;

		word = static_cast<uint32_t>(product);
		carry = product >> 32;
	}
	
	if (carry != 0)
		words.push_back(carry);

	return *this;
}

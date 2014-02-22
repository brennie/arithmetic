#include <algorithm>
#include <stdexcept>

#include "bigint.hpp"

BigInt BigInt::operator/(const BigInt& that) const
{
	BigInt copy(*this);
	copy /= that;
	return copy;
}

BigInt& BigInt::operator/=(const BigInt& that)
{
	if (that.isZero())
		throw std::invalid_argument("division by zero");
	else if (positive && *this < that)
		return *this = zero;

	bool signsDiffer = positive != that.positive;

	BigInt dividend(*this);
	BigInt divisor(that);

	size_t shift = dividend.size() - divisor.size();

	std::vector<bool> binaryDigits;

	dividend.positive = divisor.positive = true;

	divisor <<= shift;
	while (divisor > dividend)
	{
		divisor >>= 1;
		shift--;
	}

	while (shift)
	{
		if (divisor <= dividend)
		{
			dividend -= divisor;
			binaryDigits.push_back(true);
		}
		else
			binaryDigits.push_back(false);

		--shift;
		divisor >>= 1;
	}

	if (dividend >= divisor)
	{
		binaryDigits.push_back(true);
		dividend -= divisor;
	}
	else
		binaryDigits.push_back(false);

	std::reverse(binaryDigits.begin(), binaryDigits.end());
	
	*this = BigInt(BigInt::binaryToWords(binaryDigits), positive == that.positive);

	if (signsDiffer && !dividend.isZero())
		*this -= 1;

	trim();

	return *this;
}

BigInt& BigInt::operator/=(const uint32_t that)
{
	if (that == 0)
		throw std::invalid_argument("division by zero");

	uint64_t remainder = 0;

	for (auto word = words.rbegin(); word != words.rend(); ++word)
	{
		remainder = (remainder << 32) + static_cast<uint64_t>(*word);
		*word = static_cast<uint32_t>(remainder / that);
		remainder %= that;
	}
	trim();

	return *this;
}
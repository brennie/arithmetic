#include <cctype>
#include <stdexcept>

#include "bigint.hpp"


BigInt::Compare BigInt::compare(const BigInt& left, const BigInt& right)
{
	if (left.positive != right.positive)
	{
		if (left.positive)
			return Compare::GreaterThan;
		else
			return Compare::LessThan;
	}
	else if (left.positive)
	{
		if (left.size > right.size)
			return Compare::GreaterThan;
		else if (left.size < right.size)
			return Compare::LessThan;

		for (size_t i = left.size; i > 0; i--)
		{
			if (left.words[i - 1] < right.words[i - 1])
				return Compare::LessThan;
			else if (left.words[i - 1] > right.words[i - 1])
				return Compare::GreaterThan;
		}

		return Compare::Equal;
	}
	else
	{
		if (left.size > right.size)
			return Compare::LessThan;
		else if (left.size < right.size)
			return Compare::GreaterThan;

		for (size_t i = left.size; i > 0; i--)
		{
			if (left.words[i - 1] < right.words[i - 1])
				return Compare::GreaterThan;
			else if (left.words[i - 1] > right.words[i - 1])
				return Compare::LessThan;
		}

		return Compare::Equal;
	}
}

BigInt::BigInt(const int32_t value) : positive(value >= 0), size(1)
{
	words.push_back(value >= 0 ? value : -value);
}

BigInt::BigInt(const BigInt& that) : positive(that.positive), size(that.size), words(that.words)
{
}

BigInt::BigInt(BigInt&& that) : positive(that.positive), size(that.size), words(std::move(that.words))
{
}

BigInt& BigInt::operator=(BigInt&& that)
{
	positive = that.positive;
	words = std::move(that.words);

	return *this;
}

BigInt BigInt::operator-() const
{
	BigInt negated(*this);
	negated.positive = !positive;
	return negated;
}

BigInt BigInt::operator+(const BigInt& that) const
{
	BigInt copy(*this);
	copy += that;
	return copy;
}

BigInt BigInt::operator-(const BigInt& that) const
{
	BigInt copy(*this);
	copy -= that;
	return copy;
}

BigInt& BigInt::operator+=(const BigInt& that)
{
	if (positive && !that.positive)
		*this -= -that;
	else if (!positive && that.positive)
		*this = that - *this;
	else
	{
		const BigInt& smaller = std::min(*this, that);
		const BigInt& larger = std::max(*this, that);

		uint32_t carry = 0;

		for (size_t i = 0; i < larger.size; i++)
		{
			if (i < smaller.size)
			{
				uint64_t sum = carry + words[i] + that.words[i];

				if (sum > 0xFFFFFFFF)
				{
					words[i] = sum & 0xFFFFFFFF;
					carry = sum >> 32;
				}
			}
			else
			{
				uint64_t sum = carry + larger.words[i];
				words.push_back(sum & 0xFFFFFFFF);
				carry = sum >> 32;
			}
		}

		if (carry != 0)
			words.push_back(carry);
	}

	return *this;
}

BigInt& BigInt::operator-=(const BigInt& that)
{
	constexpr uint64_t borrow = 0x100000000ull;

	if (positive && !that.positive)
		*this += -that;
	else if (!positive && that.positive)
	{
		BigInt copy(that);
		positive = true;
		copy += *this;
		*this = std::move(copy);
		positive = false;
	}
	else if ((positive && that > *this) || !positive)
			*this = that - *this;
	else
	{
		/* We are guaranteed that *this >= that */

		bool shouldBorrow = false;
		for (size_t i = 0; i < size; i++)
		{
			if (shouldBorrow)
			{
				shouldBorrow = false;
				words[i] -= 1;
			}
			
			if (i < that.size)
			{
				if (words[i] >= that.words[i])
					words[i] -= that.words[i];
				else
				{
					uint64_t sum = words[i] + borrow;
					sum -= that.words[i];

					words[i] = sum;
					
					shouldBorrow = true;
				}
			}
			else
				/* Since we are guaranteed that *this > that, we can stop here
				 * as no more words will change; the last possible word that
				 * could change is the one we just processed and that
				 * is just subtracting 1 if we borrowed.
				 */
				break;
		}

	}

	return *this;
}

bool operator==(const BigInt& left, const BigInt& right)
{
	return BigInt::compare(left, right) == BigInt::Compare::Equal;
}

bool operator!=(const BigInt& left, const BigInt& right)
{
	return BigInt::compare(left, right) != BigInt::Compare::Equal;
}

bool operator<(const BigInt& left, const BigInt& right)
{
	return BigInt::compare(left, right) == BigInt::Compare::LessThan;
}

bool operator>(const BigInt& left, const BigInt& right)
{
	return BigInt::compare(left, right) == BigInt::Compare::GreaterThan;
}

bool operator>=(const BigInt& left, const BigInt& right)
{
	return BigInt::compare(left, right) != BigInt::Compare::LessThan;
}

bool operator<=(const BigInt& left, const BigInt& right)
{
	return BigInt::compare(left, right) != BigInt::Compare::GreaterThan;
}

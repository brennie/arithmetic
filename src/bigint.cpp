#include <algorithm>
#include <cctype>
#include <stdexcept>

#include "bigint.hpp"

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

bool BigInt::operator==(const BigInt& that) const
{
	if (positive != that.positive || size != that.size)
		return false;

	return std::equal(words.cbegin(), words.cend(), that.words.cbegin());
}

bool BigInt::operator!=(const BigInt& that) const
{
	return !(*this == that);
}

bool BigInt::operator<(const BigInt& that) const
{
	if (positive != that.positive)
		return !positive;

	else if (positive)
	{
		if (size < that.size)
			return true;
		else if (size > that.size)
			return false;
		else
			/* We perform a lexographcial compare which returns true if words
			 * reversed is less lexicographically than that.words.
			 */
			return std::lexicographical_compare(words.crbegin(), words.crend(),
				that.words.crbegin(), that.words.crend());
	}
	else
	{
		if (size < that.size)
			return false;
		else if (size > that.size)
			return true;
		else
			/* We check using std::greater as the absolute value of this is
			 * greater than the absolute value of that, then this is larger.
			 */
			return std::lexicographical_compare(words.crbegin(), words.crend(),
				that.words.crbegin(), that.words.crend(), std::greater<BigInt>());
	}
}

bool BigInt::operator>(const BigInt& that) const
{
	return that < *this;
}

bool BigInt::operator<=(const BigInt& that) const
{
	return !(that < *this);
}

bool BigInt::operator>=(const BigInt& that) const
{
	return !(*this < that);
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

	trim();

	return *this;
}



void BigInt::trim()
{
	/* We always ensure that the size is at least one (even if words[0] == 0). */
	while (size > 1 && words[size - 1] == 0)
		size--;
}

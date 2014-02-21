#include <algorithm>
#include <functional>
#include <cctype>
#include <stack>
#include <stdexcept>
#include <sstream>

#include "bigint.hpp"

BigInt BigInt::zero(0);
BigInt BigInt::one(1);

BigInt::BigInt() : positive(true), words(1)
{
	words[0] = 0;
}

BigInt::BigInt(uint32_t that) : positive(true), words(1)
{
	words[0] = that;
}

BigInt::BigInt(const BigInt& that) : positive(that.positive), words(that.words)
{
}

BigInt::BigInt(BigInt&& that) : positive(that.positive), words(std::move(that.words))
{
}

BigInt& BigInt::operator=(BigInt& that)
{
	positive = that.positive;
	words = that.words;

	return *this;
}

BigInt& BigInt::operator=(BigInt&& that)
{
	positive = that.positive;
	words = std::move(that.words);

	return *this;
}

bool BigInt::operator==(const BigInt& that) const
{
	if (positive != that.positive || words.size() != that.words.size())
		return false;

	return std::equal(words.cbegin(), words.cend(), that.words.cbegin());
}

bool BigInt::operator==(const uint32_t that) const
{
	if (!positive || words.size() != 1)
		return false;

	return words.front() == that;
}

bool BigInt::operator!=(const BigInt& that) const
{
	return !(*this == that);
}

bool BigInt::operator!=(const uint32_t that) const
{
	return !(*this == that);
}

bool BigInt::operator<(const BigInt& that) const
{
	if (positive != that.positive)
		return !positive;

	else if (positive)
	{
		if (words.size() < that.words.size())
			return true;
		else if (words.size() > that.words.size())
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
		if (words.size() < that.words.size())
			return false;
		else if (words.size() > that.words.size())
			return true;
		else
			/* We check using std::greater as the absolute value of this is
			 * greater than the absolute value of that, then this is larger.
			 */
			return std::lexicographical_compare(words.crbegin(), words.crend(),
				that.words.crbegin(), that.words.crend(), 
				std::greater<BigInt>());
	}
}

bool BigInt::operator<(const uint32_t that) const
{
	if (!positive)
		return true;

	if (words.size() != 1)
		return false;

	return words.front() < that;
}

bool BigInt::operator>(const BigInt& that) const
{
	return that < *this;
}

bool BigInt::operator>(const uint32_t that) const
{
	if (!positive)
		return false;

	if (words.size() != 1)
		return true;

	return words.front() > that;
}

bool BigInt::operator<=(const BigInt& that) const
{
	return !(that < *this);
}

bool BigInt::operator<=(const uint32_t that) const
{
	return !(*this > that);
}

bool BigInt::operator>=(const BigInt& that) const
{
	return !(*this < that);
}

bool BigInt::operator>=(const uint32_t that) const
{
	return !(*this < that);
}

BigInt BigInt::operator-() const
{
	BigInt negated(*this);
	negated.negate();
	return negated;
}

void BigInt::negate()
{
	positive = !positive;
}

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

BigInt BigInt::operator--()
{
	BigInt old(*this);
	*this -= one;
	return old;
}

BigInt& BigInt::operator--(const int)
{
	*this -= one;
	return *this;
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

BigInt BigInt::operator/(const BigInt& that) const
{
	BigInt copy(*this);
	copy /= that;
	return copy;
}

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

	for (auto word = words.crbegin(); word != words.crend(); word++)
	{
		remainder = ((remainder << 32) + *word) % that;
	}

	return static_cast<uint32_t>(remainder);
}

BigInt& BigInt::operator+=(const BigInt& that)
{
	if (positive && !that.positive)
	{
		*this -= -that;
		trim();
	}
	else if (!positive && that.positive)
	{
		*this = that - *this;
		trim();
	}
	else
	{
		const BigInt& smaller = std::min(*this, that);
		const BigInt& larger = std::max(*this, that);

		uint32_t carry = 0;

		for (size_t i = 0; i < larger.words.size(); i++)
		{
			uint64_t sum;
			if (i < smaller.words.size())
			{
				sum = static_cast<uint64_t>(carry) + static_cast<uint64_t>(words[i]) + static_cast<uint64_t>(that.words[i]);

				words[i] = sum & 0xFFFFFFFF;
			}
			else
			{
				sum = static_cast<uint64_t>(carry) + static_cast<uint64_t>(larger.words[i]);

				if (this == &smaller)
					words.push_back(sum & 0xFFFFFFFF);
				else
					words[i] = sum & 0xFFFFFFFF;
			}
			carry = sum >> 32;
		}

		if (carry != 0)
			words.push_back(carry);

	}

	return *this;
}

BigInt& BigInt::operator-=(const BigInt& that)
{
	const uint64_t borrow = 0x100000000ull;

	if (positive && !that.positive)
		return *this += -that;

	else if (!positive && that.positive)
	{
		/* We subtract |this| from that */
		BigInt copy(that);
		positive = true;
		copy -= *this;
		*this = std::move(copy);
	}
	else if ((positive && that > *this))
		*this = that - *this;

	else if (!positive)
	{
		negate();
		*this += -that;
		negate();
	}
	else
	{
		bool shouldBorrow = false;

		for (size_t i = 0; i < words.size(); i++)
		{
			if (shouldBorrow)
			{
				shouldBorrow = false;
				words[i] -= 1;
			}
			
			if (i < that.words.size())
			{
				if (words[i] >= that.words[i])
					words[i] -= that.words[i];
				else
				{
					uint64_t sum = words[i] + borrow;
					sum -= that.words[i];

					words[i] = static_cast<uint32_t>(sum);

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

BigInt& BigInt::operator/=(const BigInt& that)
{
	if (positive && !that.positive)
	{
		*this /= -that;
		positive = false;
	}
	else if (!positive && that.positive)
	{
		positive = true;
		*this /= that;
		positive = false;
	}
	else if (!positive && !that.positive)
	{
		positive = true;
		*this /= -that;
	}
	else
	{
		BigInt value(std::move(*this));
		*this = zero;

		while (value >= that)
		{
			++(*this);
			value -= that;
		}
	}

	return *this;
}

BigInt& BigInt::operator/=(const uint32_t that)
{
	uint64_t remainder = 0;

	for (auto word = words.rbegin(); word != words.rend(); word++)
	{
		remainder = (remainder << 32) + static_cast<uint64_t>(*word);
		*word = static_cast<uint32_t>(remainder / that);
		remainder %= that;
	}
	trim();

	return *this;
}

BigInt& BigInt::operator%=(const BigInt& that)
{
	if (positive && !that.positive)
	{
		*this /= -that;
		positive = false;
	}
	else if (!positive && that.positive)
	{
		positive = true;
		*this /= that;
		positive = false;
	}
	else if (!positive && !that.positive)
	{
		positive = true;
		*this /= -that;
	}
	else
	{
		while (*this >= that)
			*this -= that;
	}

	return *this;
}

BigInt::operator std::string() const
{
	/* Largest power of 10 under 2^32 */
	const uint32_t divisor = 1000000000;

	if (words.size() == 1 && words.front() == 0)
		return std::string("0");
	else
	{
		std::stringstream builder;
		std::stack<uint32_t> parts;

		BigInt copy(*this);
		uint32_t digits;

		while (copy > zero)
		{
			digits = copy % divisor;
			copy /= divisor;
			parts.push(digits);
		}

		if (!positive)
			builder << '-';

		while (!parts.empty())
		{
			digits = parts.top();
			parts.pop();
			builder << digits;
		}

		return builder.str();
	}
}

void BigInt::trim()
{
	/* We always ensure that the words.size() is at least one (even if words[0] == 0). */
	while (words.size() > 1 && words.back() == 0)
		words.pop_back();
}

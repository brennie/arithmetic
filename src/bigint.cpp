#include <algorithm>
#include <cctype>
#include <functional>
#include <iomanip>
#include <sstream>
#include <stack>
#include <stdexcept>

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

BigInt::BigInt(const std::string& str) : positive(true)
{
	if (str.length() == 0)
		words.push_back(0);

	else if (str.length() == 1 && str[0] == '-')
		throw std::invalid_argument("invalid number string");

	else
	{
		std::vector<uint32_t> digits;
		std::vector<bool> binaryDigits;

		{
			auto endIt = str.crend();
			
			if (str[0] == '-')
			{
				positive = false;
				--endIt;
			}

			for (auto it = str.crbegin(); it != endIt; ++it)
			{
				if (! std::isdigit(*it))
					throw std::invalid_argument("invalid number string");

				digits.push_back(*it - '0');
			}
		}

		/* We can treat digits as a base-10 big integer in little endian order.
		 * If we divide the number through by 2 until it is zero, then the
		 * remainders of those divisions will be the binary digits of the
		 * base-2 integer in little endian order.
		 */
		while (!digits.empty())
		{
			uint32_t remainder = 0;

			for (auto digit = digits.rbegin(); digit != digits.rend(); ++digit)
			{
				*digit += remainder * 10;
				remainder = *digit % 2;
				*digit /= 2;
			}

			binaryDigits.push_back(remainder == 1);

			while (digits.back() == 0)
				digits.pop_back();
		}

		/* We now construct individual base-32 numbers out of our binary digits. */
		for (size_t i = 0; i < binaryDigits.size(); i += 32)
		{
			uint32_t word = 0;
			for (size_t j = 32; j > 0; j--)
				if (i + j - 1 < binaryDigits.size())
					word = (word << 1) + (unsigned)binaryDigits[i + j - 1];

			words.push_back(word);
		}

		if (words.empty())
			words.push_back(0);

		if (isZero() && !positive)
			throw std::invalid_argument("invalid number string");
	}		
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

	else if (words.size() < that.words.size())
		return positive;

	else if (words.size() > that.words.size())
		return !positive;

	else if (positive)
		/* We perform a lexographcial compare which returns true if words
		 * reversed is less lexicographically than that.words.
		 */
		return std::lexicographical_compare(words.crbegin(), words.crend(),
			that.words.crbegin(), that.words.crend());

	else
		/* We check using std::greater as the absolute value of this is
		 * greater than the absolute value of that, then this is larger.
		 */
		return std::lexicographical_compare(words.crbegin(), words.crend(),
			that.words.crbegin(), that.words.crend(), 
			std::greater<BigInt>());
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

	for (auto word = words.crbegin(); word != words.crend(); ++word)
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

	for (auto word = words.rbegin(); word != words.rend(); ++word)
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
		std::string part;
		std::stack<std::string> parts;

		BigInt copy(*this);
		uint32_t digits;

		if (!positive)
		{
			builder << '-';
			copy.negate();
		}
		int i = 0;
		while (!copy.isZero())
		{
			std::stringstream partBuilder;

			if (copy > divisor)
			{
				digits = copy % divisor;
				copy /= divisor;

				partBuilder << std::setw(9) << std::setfill('0') << digits;
			}
			else
			{
				digits = copy.words.back();
				copy = zero;
				partBuilder << digits;
			}

			partBuilder >> part;
			parts.push(part);
			i++;
		}

		while (!parts.empty())
		{
			builder << parts.top();
			parts.pop();
		}

		return builder.str();
	}
}

bool BigInt::isZero() const
{
	return words.size() == 1 && words.back() == 0;
}

void BigInt::trim()
{
	/* We always ensure that the words.size() is at least one (even if words[0] == 0). */
	while (words.size() > 1 && words.back() == 0)
		words.pop_back();
}

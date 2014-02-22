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

			while (!digits.empty() && digits.back() == 0)
				digits.pop_back();
		}

		words = BigInt::binaryToWords(binaryDigits);

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

BigInt::BigInt(const BigInt::Words& that, bool sign) : positive(sign), words(that)
{
}

BigInt::BigInt(const BigInt::Words&& that, bool sign) : positive(sign), words(that)
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

BigInt& BigInt::operator+=(const BigInt& that)
{
	if (positive && !that.positive)
		return *this -= that.words;
	else if (!positive && that.positive)
	{
		BigInt copy(that);
		copy -= words;
		*this = std::move(copy);
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

BigInt& BigInt::operator-=(const BigInt& that)
{
	/* If the signs are different, the number gets either more strictly positive
	 * or more strictly negative, so we increase |this|.
	 */
	if (positive != that.positive)
		*this += that.words;

	else if (*this >= that)
	{
		/* Since both are positive, |that| = that. */
		if (positive)
			*this -= that.words;

		/* Since both are negative and this > that (i.e. this is strictly less
		 * negative than that) and we have:
		 *
		 * this - that = -|this| - (-|that|)
		 *             = -|this| + |that|
		 *             = |that| - |this|
		 */
		else
		{
			BigInt copy(that);
			copy.positive = true;
			copy -= words;
			*this = std::move(copy);
		}
	}
	else
	{
		/* Since both are positive and this < that:
		 *
		 * this - that = -(that - this)
		 */
		if (positive)
		{
			BigInt copy(that);
			copy -= words;
			*this = std::move(copy);
			positive = false;
		}
		/* Since both are negative and this < that (i.e. this is more strictly
		 * negative than that), we have:
		 *
		 * this - that = -|this| - (-|that|)
		 *             = -|this| + |that|
		 *             = - ( |this| - |that| )
		 */
		else
		{
			*this -= that.words;
			positive = false;
		}
	}

	return *this;
}

BigInt& BigInt::operator-=(const BigInt::Words& that)
{ 
	const uint64_t borrow = 0x100000000ull;

	for (size_t i = 0; i < words.size(); i++)
	{	
		if (i < that.size())
		{
			if (words[i] >= that[i])
				words[i] -= that[i];
			else
			{
				uint64_t sum = borrow + static_cast<uint64_t>(words[i]) - static_cast<uint64_t>(that[i]);
				size_t j = i + 1;

				words[i] = static_cast<uint32_t>(sum);

				/* We perform the borrow; if any words to the left are 0, they
				 * become the maximum value and we continue borrowing from the
				 * left.
				 */
				while (words[j] == 0)
				{
					words[j++] = 0xFFFFFFFF;
				}
				
				/* The last word we borrow from is decreased. */
				--words[j];
			}
		}
	}

	trim();

	return *this;
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

BigInt& BigInt::operator%=(const BigInt& that)
{
	*this %= that.words;
	
	if (positive != that.positive)
	{
		*this = that - *this;

		if (!that.positive)
			positive = false;
	}
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

bool BigInt::isPositive() const
{
	return positive;
}

bool BigInt::isNegative() const
{
	return !positive;
}

size_t BigInt::size() const
{
	/* The size of the number is the number of bits in each word.
	 * We know that all but the last word is guaranteed to be full. The last
	 * word (i.e. the msb) has a variable number of bits, so we count those and
	 * add it to the total.
	 */
	size_t size = 32 * (words.size() - 1);
	for (uint32_t msb = words.back(); msb != 0; msb >>= 1)
		size++;

	return size;
}

void BigInt::trim()
{
	/* We always ensure that the words.size() is at least one (even if words[0] == 0). */
	while (words.size() > 1 && words.back() == 0)
		words.pop_back();
}

BigInt::Words BigInt::binaryToWords(const std::vector<bool>& binaryDigits)
{
	BigInt::Words words;

	for (size_t i = 0; i < binaryDigits.size(); i += 32)
	{
		uint32_t word = 0;
		for (size_t j = 32; j > 0; j--)
			if (i + j - 1 < binaryDigits.size())
				word = (word << 1) + (uint32_t)binaryDigits[i + j - 1];

		words.push_back(word);
	}

	return words;
}

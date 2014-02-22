#include <cctype>
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

#include <cctype>
#include <limits>
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

bool BigInt::operator==(const BigInt& that) const
{
	return compare(*this, that) == Compare::Equal;
}

bool BigInt::operator!=(const BigInt& that) const
{
	return compare(*this, that) != Compare::Equal;
}

bool BigInt::operator<(const BigInt& that) const
{
	return compare(*this, that) == Compare::LessThan;
}

bool BigInt::operator>(const BigInt& that) const
{
	return compare(*this, that) == Compare::GreaterThan;
}

bool BigInt::operator>=(const BigInt& that) const
{
	return compare(*this, that) != Compare::LessThan;
}

bool BigInt::operator<=(const BigInt& that) const
{
	return compare(*this, that) != Compare::GreaterThan;
}

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

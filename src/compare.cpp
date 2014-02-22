#include "bigint.hpp"

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
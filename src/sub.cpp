#include "bigint.hpp"

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

BigInt BigInt::operator-(const BigInt& that) const
{
	BigInt copy(*this);
	copy -= that;
	return copy;
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
#ifndef INCLUDE_BIGINT_HPP
#define INCLUDE_BIGINT_HPP

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

/**
 * \brief An arbitrary precision integer.
 */
class BigInt
{
public:
	/**
	 * \brief Initialize a BigInt.
	 * \param that The value to initialize to.
	 */
	BigInt(int value);

	/**
	 * \brief Copy-construct a BigInt from that.
	 * \param that The value to copy.
	 */
	BigInt(const BigInt& that);

	/**
	 * \brief Move-construct a BigInt.
	 * \param that The value to move.
	 */
	BigInt(BigInt&& that);

	/**
	 * \brief Move-assign a BigInt.
	 * \param that The value to move.
	 * \return The object assigned to.
	 */
	BigInt& operator=(BigInt&& that);


	/**
	 * \brief Compare for equality.
	 * \param that The value to compare against.
	 * \return Whether the two values are equal.
	 */
	bool operator==(const BigInt& that) const;

	/**
	 * \brief Compare for inequailty.
	 * \param that The value to compare against.
	 * \return Whether the two values aren't equal.
	 */
	bool operator!=(const BigInt& that) const;

	/**
	 * \brief Check if this value is less than another.
	 * \param that The value to compare against.
	 * \return Whether this value is less than the other.
	 */
	bool operator<(const BigInt& that) const;

	/**
	 * \brief Check if this value is greater than another.
	 * \param that The value to compare against.
	 * \return Whether this value is greater than the other.
	 */
	bool operator>(const BigInt& that) const;

	/**
	 * \brief Check if this value is less than another or equal to it.
	 * \param that The value to compare against.
	 * \return Whether this value is less than the other or equal to it.
	 */
	bool operator<=(const BigInt& that) const;

	/**
	 * \brief Check if this value is greater than another or equal to it.
	 * \param that The value to compare against.
	 * \return Whether this value is greater than the other or equal to it.
	 */
	bool operator>=(const BigInt& that) const;


	/**
	 * \brief Negate a BigInt.
	 * \return The value negated.
	 */
	BigInt operator-() const;

	/**
	 * \brief Add two BigInts.
	 * \param that The value to add to this.
	 * \return The sum.
	 */
	BigInt operator+(const BigInt& that) const;

	/**
	 * \brief Subtract two BigInts.
	 * \param that The value to subtract from this.
	 * \param The difference.
	 */
	BigInt operator-(const BigInt& that) const;

	/**
	 * \brief Add that to this.
	 * \param that The value to add to this.
	 * \return This.
	 */
	BigInt& operator+=(const BigInt& that);

	/**
	 * \brief Subtract that from this.
	 * \param that The value to subtract from this.
	 * \return This.
	 */
	BigInt& operator-=(const BigInt& that);

private:
	bool positive; /*< True if the number is positive. */
	
	/**
	 * \brief The number of words we are using. This may be less than
	 * words.size() as it possible to over-allocate memory.
	 */
	size_t size;

	std::vector<uint32_t> words; /*< The number stored as 32-bit words. */

	/**
	 * \brief Trim the zeroes from the big end of the number.
	 */
	void trim();
};



#endif

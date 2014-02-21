#include <cassert>

#include "BigInt.hpp"

using namespace std;

int main()
{
	BigInt value(0);
	BigInt zero(0);

	BigInt j(0xFFFFFFFF);

	for (size_t i = 0; i < 10; i++)
		value += j;

	for (size_t i = 0; i < 10; i++)
		value -= j;

	assert(value == zero);
	return 0;
}

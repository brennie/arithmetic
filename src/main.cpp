#include <cassert>

#include "BigInt.hpp"

using namespace std;

int main()
{
	BigInt a(123), b(-123), c(123), d(-123), e(256), f(-256);

	assert(a == c);
	assert(b == d);
	assert(a >= c);
	assert(c <= a);
	assert(d < e);
	assert(a >= f);
	return 0;
}

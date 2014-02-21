#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include "BigInt.hpp"

using namespace std;

int main()
{
	vector<string> ss {"0",
		"4294967295", "-4294967295",
		"4294967296", "-4294967296",
		"18446744073709551615", "-18446744073709551615",
		"79228162514264337593543950335", "-79228162514264337593543950335",
		"1289371928311231231", "-1289371928311231231"};
	vector<BigInt> is;

	size_t width = 0;

	for (auto s : ss)
	{
		is.push_back(BigInt(s));
		width = std::max(width, s.length());
	}

	cout << boolalpha;
	for (size_t i = 0; i < is.size(); i++)
	{
		string ii = (string)is[i];
		cout << setw(width) << ss[i] << " =?= " << setw(width) << ii << " = " << (ss[i] == ii) << endl;
	}
	
	return 0;
}

#include <string>
#include <iostream>
#include <vector>
#include <functional>
#include <stdexcept>
#include "BigInt.hpp"

using namespace std;

bool test_roundtrip()
{
	bool success = true;

	vector<string> tests {
		"0",
		"4294967295", "-4294967295",
		"4294967296", "-4294967296",
		"18446744073709551615", "-18446744073709551615",
		"79228162514264337593543950335", "-79228162514264337593543950335",
		"1289371928311231231", "-1289371928311231231",
		"100000000000000000000000000000000000000000000000000000000000000000000",
		"1561749840894125169814814058904901914569840569840894089415146908974098"
	};

	cout << "test_roundtrip:" << endl;
	for (auto test : tests)
	{
		string trip((string)BigInt(test));

		if (test == trip)
			cout << test << " == "  << trip << endl;
		else
		{
			cout << test << " != " << trip << endl;
			success = false;
		}
	}

	return success;
}

bool test_constructor()
{
	size_t caught = 0;
	vector<string> tests {
		"-",
		"--",
		"hello",
		"-hello",
		"-0"
	};
	
	cout << "test_constructor:" << endl;
	for (auto test : tests)
	{
		bool threw = false;
		try
		{
			BigInt i(test);
		}
		catch (const invalid_argument& e)
		{
			threw = true;
		}
		
		caught += threw;
		if (threw)
			cout << "Bad Construct BigInt(\"" << test << "\") threw correctly!" << endl;
		else
			cout << "Bad Construct BigInt(\"" << test << "\") did not throw!" << endl;
	}

	return caught == tests.size();
}

int main()
{
	size_t successes = 0;

	vector<function<bool(void)>> tests {
		test_constructor,
		test_roundtrip
	};

	for (auto test : tests)
	{
		bool success = test();
		successes += success;
		if (success)
			cout << "test succeeded" << endl;
		else
			cout << "test failed" << endl;

		cout << "======================================================" << endl;
	}

	cout << successes << " / " << tests.size() << " passed" << endl;
	
	return 0;
}

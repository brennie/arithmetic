#include <string>
#include <iostream>
#include <vector>
#include <functional>
#include <stdexcept>
#include "bigint.hpp"

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

bool test_sign()
{
	bool success = true;

	struct Test
	{
		string value;
		bool sign;
	};

	vector<Test> tests
	{
		{"0", true},
		{"1", true},
		{"-1", false},
		{"123490182349012384190234812903412341", true},
		{"340823048234902342902345123452435", true},
		{"-340823048234902342902345123452435", false},
		{"-123149359300777481847332467779959906", false}
	};

	cout << "test_sign:" << endl;
	for (auto test : tests)
	{
		BigInt value(test.value);
		if (value.isPositive() == test.sign)
		{
			if (test.sign)
				cout << test.value << " >= 0" << endl;
			else
				cout << test.value << " < 0" << endl;
		}
		else
		{
			if (test.sign)
				cout << test.value << " < 0 (X)" << endl;
			else
				cout << test.value << " >= 0 (X)" << endl;

			success = false;
		}
	}

	return success;
}

bool test_addition()
{
	bool success = true;

	struct Test
	{
		string left, right, result;
	};

	vector<Test> tests
	{
		{"4294967296", "-4294967296", "0"},
		{"123490182349012384190234812903412341", "-123490182349012384190234812903412341", "0"},
		{"123490182349012384190234812903412341", "340823048234902342902345123452435", "123831005397247286533137158026864776"},
		{"340282366920938463463374607431768211456", "-10", "340282366920938463463374607431768211446"},
		{"123490182349012384190234812903412341", "-340823048234902342902345123452435", "123149359300777481847332467779959906"},
		{"-340823048234902342902345123452435", "123490182349012384190234812903412341", "123149359300777481847332467779959906"},
		{"-340823048234902342902345123452435", "-340823048234902342902345123452", "-341163871283137245245247468575887"}
	};

	cout << "test_addition:" << endl;
	for (auto test : tests)
	{
		BigInt left(test.left), right(test.right), result(test.result);
		BigInt sum(left + right);
		if (sum == result)
			cout << test.left << " + " << test.right << " == " << test.result << endl;
		else
		{
			cout << test.left << " + " << test.right << " != " << test.result
		         << " (got " << (string)sum << " instead)" << endl;

		    success = false;
		}
	}

	return success;
}

bool test_subtraction()
{
	bool success = true;

	struct Test
	{
		string left, right, result;
	};

	vector<Test> tests
	{
		{"348927348923742893472398472398423", "45789236458276345872345824", "348927303134506435196052600052599" },
		{"3218916531984769519815369851200", "321658456106546516504651650416305640650460", "-321658456103327599972666880896490270799260"},
		{"340823048234902342902345123452435", "-340823048234902342902345123452", "341163871283137245245247468575887"},
		{"-340823048234902342902345123452", "-340823048234902342902345123452435", "340482225186667440559442778328983"},
		{"-14523440162534416205344125634016234056123441", "-51230441562344010162534", "-14523440162534416205292895192453890045960907"}
	};

	cout << "test_subtraction:" << endl;
	for (auto test : tests)
	{
		BigInt left(test.left), right(test.right), result(test.result);
		BigInt diff(left - right);
		if (diff == result)
			cout << test.left << " - " << test.right << " == " << test.result << endl;
		else
		{
			cout << test.left << " - " << test.right << " != " << test.result
		         << " (got " << (string)diff << " instead)" << endl;

		    success = false;
		}
	}

	return success;
}

bool test_rshift()
{
	bool success = true;

	struct Test
	{
		string value;
		size_t shift;
		string result;
	};

	vector<Test> tests
	{
		{"1", 32, "0"},
		{"18446744073709551615", 31, "8589934591"},
		{"18446744073709551615", 32, "4294967295"},
		{"18446744073709551615", 33, "2147483647"},
		{"2313216784161423789514236106464", 32, "538587752767238716947"},
		{"2313216784161423789514236106464", 64, "125399733140"}
	};

	cout << "test_rshift:" << endl;
	for (auto test : tests)
	{
		BigInt value(test.value), result(test.result);
		BigInt shifted(value >> test.shift);

		cout << test.value << " >> " << test.shift;
		if (shifted == result)
			cout << " == " << test.result << endl;
		else
		{
			cout << " != " << test.result << " (got " << (string)shifted << " instead)" << endl;
			success = false;
		}
	}

	return success;
}

bool test_lshift()
{
	bool success = true;

	struct Test
	{
		string value;
		size_t shift;
		string result;
	};

	vector<Test> tests
	{
		{"1", 32, "4294967296"},
		{"8589934591", 31, "18446744071562067968"},
		{"4294967295", 32, "18446744069414584320"},
		{"2147483647", 33, "18446744065119617024"},
		{"2340128341023948", 23, "19630419322540218384384"},
		{"651561984651612161435878451", 64, "12019197179026560467995728591684751358286626816"}
	};

	cout << "test_lshift:" << endl;
	for (auto test : tests)
	{
		BigInt value(test.value), result(test.result);
		BigInt shifted(value << test.shift);

		cout << test.value << " << " << test.shift;
		if (shifted == result)
			cout << " == " << test.result << endl;
		else
		{
			cout << " != " << test.result << " (got " << (string)shifted << " instead)" << endl;
			success = false;
		}
	}

	return success;
}

bool test_division()
{
	bool success = true;

	struct Test
	{
		string left, right, result;
	};

	vector<Test> tests
	{
		{"256", "16", "16"},
		{"1293847", "1234", "1048"},
		{"4294967295", "1024", "4194303"},
		{"341234918273418923412341234","12341234123412","27649983369659"},
		{"12341234123947819", "123471", "99952491872"},
		{"-12341234123947819", "123471", "-99952491873"},
		{"23419283471289374", "1289347128934", "18163"},
		{"23419283471289374", "-1289347128934", "-18164"},
		{"-1890234189234", "-1293478", "1461357"},
		{"-110535373948910675079005112", "8956589984726", "-12341234123412"}
	};

	cout << "test_division:" << endl;
	for (auto test : tests)
	{
		BigInt left(test.left), right(test.right), result(test.result);
		BigInt sum(left / right);
		if (sum == result)
			cout << test.left << " / " << test.right << " == " << test.result << endl;
		else
		{
			cout << test.left << " / " << test.right << " != " << test.result
		         << " (got " << (string)sum << " instead)" << endl;

		    success = false;
		}
	}

	return success;
}

bool test_mod()
{
	bool success = true;

	struct Test
	{
		string left, right, result;
	};

	vector<Test> tests
	{
		{"256", "16", "0"},
		{"1293847", "1234", "615"},
		{"4294967295", "1024", "1023"},
		{"341234918273418923412341234","12341234123412","8956589984726"}
	};

	cout << "test_mod:" << endl;
	for (auto test : tests)
	{
		BigInt left(test.left), right(test.right), result(test.result);
		BigInt sum(left % right);
		if (sum == result)
			cout << test.left << " % " << test.right << " == " << test.result << endl;
		else
		{
			cout << test.left << " % " << test.right << " != " << test.result
		         << " (got " << (string)sum << " instead)" << endl;

		    success = false;
		}
	}

	return success;
}

int main()
{
	size_t successes = 0;

	vector<function<bool(void)>> tests {
		test_constructor,
		test_roundtrip,
		test_sign,
		test_addition,
		test_subtraction,
		test_rshift,
		test_lshift,
		test_division,
		test_mod
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

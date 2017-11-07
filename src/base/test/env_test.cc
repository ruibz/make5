#include "myGoogleTest.h"
#include "env.h"

using namespace std;

class EnvTest: public gtest_base_test_fixture
{
};

TEST_F(EnvTest, mapTest)
{
	map<int, int> _testMap;
	_testMap[0] = 0;
	_testMap[0]++;
	_testMap[0]++;
	cout << _testMap[0] << endl;
}


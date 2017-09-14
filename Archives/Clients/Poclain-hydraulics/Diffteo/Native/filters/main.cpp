#include "misc.h"
#include "tests.h"

int main(int argc, char** argv)
{
	// Test functions are located in tests.cpp
	int err1 = test1(argc, argv);

	cin.get();
	if (err1 == 0)
		return 0;
}
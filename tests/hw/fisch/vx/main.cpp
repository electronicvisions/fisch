#include <string>

#include <gtest/gtest.h>

#include "connection.h"

TestConnection generate_test_connection()
{
	return TestConnection();
}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}

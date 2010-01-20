#include <UnitTest++.h>
#include "../src/sample.h"

TEST(FailSpectacularly)
{
	CHECK(true);
}

int main()
{
	return UnitTest::RunAllTests();
}

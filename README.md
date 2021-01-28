# Apoapse Very Simple Unit Test framework
The Apoapse Very Simple Unit Test Framework is a C++ 14, single-header library, that provides easy definitions of unit and feature tests and allow to have full control of how and when the tests are run.

Since this system is extremely simple, there is no fancy features like build integration or *main function* repositioning. Instead, your are responsible to call a function that run one or more tests when and where you want in your program flow. This gives a lot of flexibility to those who want to have full control over the integration of unit testing into their test procedures.

Moreover, this allows to use the library to make feature tests. For instance, the run tests function can be called after the program has initialized the systems required by one test.

![Unit Test result on a console](https://guillaume-puyal.com/imgs/apoapse_unit_test.png)

[🚀 Find out more about Apoapse](https://github.com/apoapse)

## How to use
To create tests, just include UnitTest.hpp on a cpp file and use the following syntax to define the tests. There is no limit of the number of tests per cpp file and there is no limit of cpp files containing tests per project.
### Define tests
```cpp
#include "UnitTest.hpp"

UNIT_TEST("TestCategory1:RandomNumber")
{
	int result = RandomNumber(10, 200);
	
	CHECK(result > 10);	// Simple check macro
	CHECK(result < 200);	// Multiple checks can be used on a single test
	
} UNIT_TEST_END

UNIT_TEST("TestCategory1:RandomPassword")
{
	std::string password = GenerateRandomPassword();
	
	REQUIRE(password.length() >= 8);				// Unlike a CHECK, a REQUIRE stop the code execution if it fail
	CHECK_PRINT(password != "admin", "Password not allowed");	// CHECK_PRINT regular check but print a custom debug message
	
} UNIT_TEST_END
```

### Run tests
To run the tests, just use the RunTests function by providing an output stream object like std::cout to print on the console or std::ofstream to write into a file.
Custom output stream objects (inheriting from std::ostream) can of course be used.

In the case of using the console as output, prints coloring is supported on Windows.

```cpp
UnitTestsManager::GetInstance().RunTests(std::cout);	// Print the results on the console
```

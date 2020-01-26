// ----------------------------------------------------------------------------
// Copyright (C) 2020 Apoapse
// Copyright (C) 2020 Guillaume Puyal
//
// Distributed under the Boost Software License, Version 1.0. 
// See accompanying file LICENSE.md or copy at http://www.boost.org/LICENSE_1_0.txt
//
// For more information visit https://github.com/apoapse/Apoapse-Very-Simple-Unit-Test-framework
// And https://apoapse.space/
// ----------------------------------------------------------------------------

#pragma once
#include <vector>
#include <string>
#include <functional>
#include <exception>
#include <iostream>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

class APFailException : public std::exception
{
};

class UnitTest
{
	std::function<void()> m_testCode;
	std::string m_fullName;

public:
	UnitTest(const std::string& fullName, const std::function<void()> code)
		: m_testCode(code)
		, m_fullName(fullName)
	{
	}

	bool operator== (const UnitTest& other) const
	{
		return m_fullName == other.GetFullName();
	}

	bool Run(std::string& exceptionError) const
	{
		try
		{
			m_testCode();
			return true;
		}
		catch (const APFailException&)
		{
			return false;
		}
		catch (const std::exception& e)
		{
			exceptionError = e.what();
			return false;
		}
	}

	std::string GetFullName() const
	{
		return m_fullName;
	}
};

class UnitTestsManager
{
	struct TestExec
	{
		std::vector<std::string> errorMsgs;
	};
	
	std::vector<UnitTest> m_registeredUnitTests;
	TestExec m_currentTest;

	enum class ConsoleColors
	{
		DEFAULT,
		GREEN,
		RED
	};
	
public:
	UnitTestsManager() = default;
	UnitTestsManager(UnitTestsManager const&) = delete;
	void operator=(UnitTestsManager const&) = delete;

	void RunTests(const std::string& testsPath = "")
	{
		int successCount = 0;
		int errorsCount = 0;
		const auto toExecuteTestsCount = static_cast<int>(m_registeredUnitTests.size());

		SortTests();

		Log("EXECUTING " + std::to_string(toExecuteTestsCount) + " UNIT TESTS...");

		for (const UnitTest& test : m_registeredUnitTests)
		{
			std::string exceptionError;
			m_currentTest = TestExec();

			const bool testResult = test.Run(exceptionError);

			if (testResult && m_currentTest.errorMsgs.empty())
			{
				Log("TEST " + test.GetFullName() + " -> SUCCESS", ConsoleColors::GREEN);
				++successCount;
			}
			else
			{
				Log("TEST " + test.GetFullName() + " -> FAILURE", ConsoleColors::RED);

				for (const std::string& errorMsg : m_currentTest.errorMsgs)
				{
					Log("\t " + errorMsg, ConsoleColors::RED);
				}

				if (!exceptionError.empty())
				{
					Log("\t Exception triggered: " + exceptionError, ConsoleColors::RED);
				}
				
				++errorsCount;
			}
		}

		const ConsoleColors resultColor = (errorsCount == 0) ? ConsoleColors::DEFAULT : ConsoleColors::RED;
		Log("EXECUTED " + std::to_string(toExecuteTestsCount) + " UNIT TESTS. " + std::to_string(successCount) + " successful, " + std::to_string(errorsCount) + " failed", resultColor);
	}

	void RegisterTest(const UnitTest& test)
	{
		m_registeredUnitTests.push_back(test);
	}

	static UnitTestsManager& GetInstance()
	{
		static UnitTestsManager testsManager;
		return testsManager;
	}

	void Check(bool exp, const std::string& code)
	{
		if (!exp)
		{
			m_currentTest.errorMsgs.push_back("CHECK failed on: " + code);
		}
	}
	
	void Check(bool exp, const std::string& code, const std::string& debugPrint)
	{
		if (!exp)
		{
			m_currentTest.errorMsgs.push_back("CHECK failed on: " + code + "  -  " + debugPrint);
		}
	}
	
	void Require(bool exp, const std::string& code)
	{
		if (!exp)
		{
			m_currentTest.errorMsgs.push_back("REQUIRE failed on: " + code);
			
			throw APFailException();
		}
	}
	
	void Require(bool exp, const std::string& code, const std::string& debugPrint)
	{
		if (!exp)
		{
			m_currentTest.errorMsgs.push_back("REQUIRE failed on: " + code + "  -  " + debugPrint);

			throw APFailException();
		}
	}

private:
	static void Log(const std::string& msg, ConsoleColors color = ConsoleColors::DEFAULT)
	{
#ifdef _WIN32
		switch (color)
		{
		case ConsoleColors::GREEN:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			break;
		case ConsoleColors::RED:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
			break;
		}
#endif

		std::cout << msg << '\n';

#ifdef _WIN32
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);	// Reset to default color
#endif
	}
	
	void SortTests()
	{
		std::sort(m_registeredUnitTests.begin(), m_registeredUnitTests.end(), [](UnitTest& left, UnitTest& right)
		{
			return (left.GetFullName() < right.GetFullName());
		});
	}
};

class UnitTestAutoRegister
{
public:
	UnitTestAutoRegister(const UnitTest& test)
	{
		UnitTestsManager::GetInstance().RegisterTest(test);
	}
};

#define AP_CONCAT_IMPL( x, y )		x##y
#define AP_MACRO_CONCAT( x, y )	AP_CONCAT_IMPL( x, y )

// Test macros
#define UNIT_TEST(_name)			static UnitTestAutoRegister AP_MACRO_CONCAT(testRegister_, __COUNTER__)(UnitTest(_name, []() -> void
#define UNIT_TEST_END				));
#define CHECK(_exp)					UnitTestsManager::GetInstance().Check(_exp, #_exp);
#define CHECK_PRINT(_exp, _deb)		UnitTestsManager::GetInstance().Check(_exp, #_exp, _deb);
#define REQUIRE(_exp)				UnitTestsManager::GetInstance().Require(_exp, #_exp);
#define REQUIRE_PRINT(_exp, _deb)	UnitTestsManager::GetInstance().Require(_exp, #_exp, _deb);
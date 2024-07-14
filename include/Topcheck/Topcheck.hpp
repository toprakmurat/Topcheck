#ifndef TOPCHECK_HPP
#define TOPCHECK_HPP

#include <iostream>
#include <functional>
#include <string>
#include <sstream>
#include <vector>
#include <memory>

template<typename T>
concept EqualityComparable = requires(T a, T b) {
	{ a == b } -> std::convertible_to<bool>;
};

template<typename T>
concept FloatingPoint = std::is_floating_point_v<T>;

namespace Topcheck
{	
	// Base class for test cases to enable polymorphism
	class ITestCase 
	{
	public:
		virtual ~ITestCase() = default;
		virtual void Run() const = 0;
		virtual const std::string& GetName() const = 0;
	};

	template<typename Func>
	class TestCase : public ITestCase
	{
	public:
		TestCase(const std::string& name, Func testFunc)
			: m_Name(name), m_TestFunc(testFunc)
		{}

		void Run() const override
		{
			if constexpr (std::is_void_v<std::invoke_result_t<Func>>)
			{
				m_TestFunc();
			}
			else 
			{
				auto result = m_TestFunc();
				std::cout << "Test returned: " << result << std::endl;
			}
		}

		const std::string& GetName() const override { return m_Name; }
	private:
		std::string m_Name;
		Func m_TestFunc;

	};  // class TestCase 
	
	class TestRunner
	{
	public:
		template<typename Func>
		void AddTestCase(const std::string& name, Func testFunc)
		{
			m_TestCases.push_back(std::make_unique<TestCase<Func>>(name, testFunc));
		}

		void RunAll() const
		{
			for (const auto& testCase : m_TestCases)
			{
				std::cout << "Running test: " << testCase->GetName() << std::endl;
				testCase->Run();
			}
		}

	private:
		std::vector<std::unique_ptr<ITestCase>> m_TestCases;
	};  // class TestRunner
	
	
	class Assertion 
	{
	public:
		// Constructors
		Assertion() = default;

		Assertion(const std::string& file)
			: m_File(file) {}
		
		Assertion(const std::string& file, int line)
			: m_File(file), m_Line(line) {}

		template<EqualityComparable T>
		void AssertEqual(const T& expected, const T& actual)
		{	
			std::ostringstream oss;
			oss << expected << "==" << actual;

			if (!(expected == actual))
				buildFailMessage(oss.str());
			else
				buildSuccessMessage(oss.str());
		}

		template<FloatingPoint T>
		void AssertAlmostEqual(const T& expected, const T& actual, const T& epsilon) 
		{
			std::ostringstream oss;
			oss << "Expected: " << expected << ", Actual: " << actual << ", Epsilon: " << epsilon;

			if (std::fabs(expected - actual) >= epsilon)
				buildFailMessage(oss.str());
			else
				buildSuccessMessage(oss.str());
		}

		void PrintResults() const 
		{
			for (const auto& message : m_Messages)
			{
				std::cout << message << std::endl;
			}
		}

		bool HasFailures() const
		{
			return !m_FailMessages.empty();
		}

	private:
		void buildFailMessage(const std::string& message) 
		{
			std::ostringstream oss;
			oss << "Test Failed: ";
			
			if (!message.empty())
				oss << message;
			
			if (!m_File.empty() && m_Line != 0)
				oss << " at " << m_File << ":" << m_Line;

			m_Messages.push_back(oss.str());
			m_FailMessages.push_back(oss.str());
		}

		void buildSuccessMessage(const std::string& message) 
		{
			std::ostringstream oss;
			oss << "Test Passed: ";
			if (!message.empty()) 
				oss << message;

			m_Messages.push_back(oss.str());
		}

		std::string m_File;
		int m_Line = 0;
		std::vector<std::string> m_Messages;
		std::vector<std::string> m_FailMessages;
	};  // class Assertion

}  // namespace Topcheck
#endif  // TOPCHECK_HPP
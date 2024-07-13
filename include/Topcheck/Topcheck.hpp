#ifndef TOPCHECK_HPP
#define TOPCHECK_HPP

#include <iostream>
#include <functional>
#include <string>
#include <sstream>
#include <vector>

namespace Topcheck 
{
template<typename T>
std::string toString(const T& value) 
{
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

template<typename Func>
class TestCase 
{
public:
	TestCase(const std::string& name, Func testFunc)
		: m_Name(name), m_TestFunc(testFunc) {}

	void Run() const 
	{
		if constexpr (std::is_void_v<std::invoke_result_t<Func>>) 
		{
			m_TestFunc();
		} 
		else 
		{
			auto result = m_TestFunc();
			std::cout << "Test returned: "<< result << std::endl;
		}
	}

	const std::string& GetName() const { return m_Name; }

private:
	std::string m_Name;
	Func m_TestFunc;
};

class TestFailure : public std::exception 
{
public:
	TestFailure(const std::string& file, int line, const std::string& expr, const std::string& message) 
		: m_File(file), m_Line(line), m_Expr(expr), m_Message(message) 
	{ BuildMessage(); }

	void PrintMessage() const {
		std::cerr << m_Message << std::endl;
	}

	const char* what() noexcept override 
	{
		return m_Message.c_str();
	}

private:
	void BuildMessage() {
		std::ostringstream oss;
		oss << "Test failed: " << m_Expr;
		if (!m_Message.empty()) {
			oss << "(" << m_Message << ")";
		}
		oss << "at" << m_File << ":" << m_Line;

		m_Message = oss.str();
	}

	std::string m_File;
	int m_Line;
	std::string m_Expr;
	std::string m_Message;
};

class TestRunner 
{
public:
	void AddTest(const TestCase& testCase) 
	{
		if (!testCase->GetName().empty()) {
			m_TestCases.push_back(testCase);
		}
		// handle else
	}

	void Run() const
	{
		for (const auto& testCase : m_TestCases)
		{
			try 
			{
				testCase.Run();
			} 
			catch (const std::exception& e)
			{
				throw TestFailure(__FILE__, __LINE__, testCase->GetName(), e.what());
			}
		}
	}
private:
	std::vector<TestCase> m_TestCases;
}

}  // namespace Topcheck
#endif  // TOPCHECK_HPP
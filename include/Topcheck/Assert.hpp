#ifndef TOPCHECK_ASSERT_HPP
#define TOPCHECK_ASSERT_HPP

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace Topcheck 
{
	template<typename T>
	void assertEqual(const T& expected, const T& actual, 
		const std::string& file, int line, const std::string& func) 
	{
		if (expected != actual) 
		{
			std::ostringstream oss;
			oss << "Assertion failed: expected (" << expected << "), actual ( " << actual
				<< "), function " << func << ", file " << file << ", line " << line << ".";

			throw std::runtime_error(oss.str());
		}
	}

	template<typename T>
	void assertTrue(const T& condition, const std::string& file, 
		int line, const std::string& func)
	{
		if (!condition) 
		{
			std::ostringstream oss;
			oss << "Assertion failed: (" << condition << "), function: " << func
				<< ", file " << file << ", line " << line << ".";

			throw std::runtime_error(oss.str());
		}
	}

#define ASSERT_EQ(expected, actual) assertEqual(expected, actual, __FILE__, __LINE__, __FUNCTION__)
#define ASSERT_TRUE(condition) assertTrue(condition, __FILE__, __LINE__, __FUNCTION__)

}  // namespace Topcheck
#endif  // TOPCHECK_ASSERT_HPP
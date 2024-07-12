#ifndef TOPCHECK_TEST_RUNNER_HPP
#define TOPCHECK_TEST_RUNNER_HPP

#include "TestCase.hpp"
#include <vector>
#include <iostream>

namespace Topcheck 
{
	class TestRunner 
	{
	public:
		void AddTest(const TestCase& testCase) 
		{
			m_TestCases.push_back(testCase);
		}

		void Run() const 
		{
			for (const auto& testCase : m_TestCases) 
			{
				try 
				{
					std::cout << "Running " << testCase.GetName() << "..." << std::endl;
					testCase.Run();
					std::cout << testCase.GetName() << " passed." << std::endl;
				}
				catch (const std::exception& e) 
				{
					std::cerr << testCase.GetName() << " failed." << e.what() << std::endl;
				}
			}
		}
	private:
		std::vector<TestCase> m_TestCases;
	};
}  //  namespace Topcheck 
#endif  // !TOPCHECK_TEST_RUNNER_HPP

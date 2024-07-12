#ifndef TOPCHECK_TEST_CASE_HPP
#define TOPCHECK_TEST_CASE_HPP

#include <functional>
#include <string>

namespace Topcheck 
{
	class TestCase 
	{
	public:
		TestCase(const std::string& name, std::function<void()> testFunc)
			: m_Name(name), m_TestFunc(testFunc) {}

		void Run() const { m_TestFunc(); };

		const std::string& GetName() const { return m_Name; }
	private:
		std::string m_Name;
		std::function<void()> m_TestFunc;
	};

}  // namespace Topcheck
#endif  // !TOPCHECK_TEST_CASE_HPP


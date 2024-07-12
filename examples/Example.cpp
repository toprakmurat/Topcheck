#include <Topcheck/Assert.hpp>
#include <Topcheck/TestCase.hpp>
#include <Topcheck/TestRunner.hpp>

using namespace Topcheck;

void testAddition() {
    int sum = 2 + 2;
    ASSERT_EQ(4, sum);
}

void testSubtraction() {
    int diff = 5 - 3;
    ASSERT_EQ(2, diff);
}

int main() {
    Topcheck::TestRunner runner;
    runner.AddTest(TestCase("testAddition", testAddition));
    runner.AddTest(TestCase("testSubtraction", testSubtraction));
    runner.Run();
    return 0;
}
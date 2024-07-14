#include <Topcheck/Topcheck.hpp>

using namespace Topcheck;
Assertion assertion(__FILE__);

void testAddition() {
    int sum = 2 + 2;
    assertion.AssertEqual(4, sum);
}

void testSubtraction() {
    int difference = 5 - 3;
    assertion.AssertEqual(2, difference);
}

void testStringEquality() {
    std::string str = "Hello";
    assertion.AssertEqual(std::string("Hello"), str);
}

void testFloatingPointEquality() {
    double value = 0.1 + 0.2;
    assertion.AssertAlmostEqual(0.3, value, 1e-9);
}

int main() {
    Topcheck::TestRunner runner;

    runner.AddTestCase("testAddition", testAddition);
    runner.AddTestCase("testSubtraction", testSubtraction);
    runner.AddTestCase("testStringEquality", testStringEquality);
    runner.AddTestCase("testFloatingPointEquality", testFloatingPointEquality);

    runner.RunAll();

    assertion.PrintResults();

    return 0;
}

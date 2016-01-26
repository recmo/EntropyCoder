#include <UnitTest++.h>
#include <TestReporter.h>
#include <iostream>
#include <vector>
#include <cstdlib>
using std::cerr;
using std::endl;

namespace Ansi {
	std::string normal;
	std::string bold;
	std::string boldRed;
	std::string boldGreen;
	void enable() {
		if(std::getenv("TERM") != nullptr) {
			normal = "\033[0m";
			bold = "\033[0;1;1m";
			boldRed = "\033[0;1;31m";
			boldGreen = "\033[0;1;32m";
		}
	}
} // end namespace Ansi

SUITE(TestUnitTest) {

TEST(SanityTest)
{
	CHECK_EQUAL(4, 2 + 2);
}

TEST(TestExceptions)
{
	CHECK_THROW(throw std::runtime_error("Catch this"), std::runtime_error);
}

}

class MyTestReporter: public UnitTest::TestReporter
{
private:
	void ReportTestStart(UnitTest::TestDetails const& test) final;
	void ReportFailure(UnitTest::TestDetails const& test, char const* failure) final;
	void ReportTestFinish(UnitTest::TestDetails const& test, float secondsElapsed) final;
	void ReportSummary(int totalTestCount, int failedTestCount, int failureCount, float secondsElapsed) final;
	
private:
	bool _currentFailed = false;
};

void MyTestReporter::ReportTestStart(UnitTest::TestDetails const& test)
{
	_currentFailed = false;
	std::cerr << "Test   " << test.suiteName << "::" << test.testName << "..." << std::flush;
}

void MyTestReporter::ReportFailure(UnitTest::TestDetails const& test, char const* failure)
{
	if(_currentFailed == false) {
		cerr << " ";
		cerr << Ansi::boldRed << "failed" << Ansi::normal << endl;
		_currentFailed  = true;
	}
	
	cerr << Ansi::bold << test.filename << ":" << test.lineNumber << ": ";
	cerr << Ansi::boldRed << "error: " << Ansi::bold;
	cerr << "Test " <<  test.suiteName << "::" << test.testName  << " failed";
	cerr << Ansi::normal << endl;
	cerr << failure << endl << endl;
}

void MyTestReporter::ReportTestFinish(UnitTest::TestDetails const& test, float secondsElapsed)
{
	if(_currentFailed == false) {
		cerr << " ";
		cerr << Ansi::boldGreen << "succeeded" << Ansi::normal;
		cerr << " in " << (secondsElapsed * 1.0e3) << " ms." << endl;
	}
}

void MyTestReporter::ReportSummary(
	int const totalTestCount, int const failedTestCount,
	int const failureCount, float const secondsElapsed)
{
	if (failureCount > 0) {
		cerr << "Test   ";;
		cerr << failedTestCount << " out of " << totalTestCount << " tests ";
		cerr << Ansi::boldRed << "failed" << Ansi::normal;
		cerr << " in " << (secondsElapsed * 1.0e3) << " ms." << endl;
	} else {
		cerr << "Test   " << totalTestCount << " tests ";
		cerr << Ansi::boldGreen << "succeeded" << Ansi::normal;
		cerr << " in " << (secondsElapsed * 1.0e3) << " ms." << endl;
	}
}

int main(int argc, char* argv[])
{
	Ansi::enable();
	cerr << "Test   " << PROGRAM << " version " << VERSION << endl;
	MyTestReporter myReporter;
	UnitTest::TestRunner runner(myReporter);
	return runner.RunTestsIf(UnitTest::Test::GetTestList(), NULL, UnitTest::True(), 0);
}

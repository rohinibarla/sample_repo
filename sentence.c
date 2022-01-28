#include <assert.h>
#include <stdio.h>

// Enhance42 C UnitTest Framework, based on CuTest
#include "E42/UnitTest.h"


// Write your logic in your word here in comments
// 
//
//
//
//
//

int numberOfWords(const char *sentence)
{
    // write your code here

    return 0;
}












// tests

void singleWordTest(UnitTest *tc)
{
    const char *sentence = "namasthey";
    const int actual = numberOfWords(sentence);
    const int expected_output = 1;
    AssertIntEquals(tc, expected_output, actual);
}

void multipleWordTest(UnitTest *tc)
{
    const char *sentence = "namasthey welcome to coding";
    const int actual = numberOfWords(sentence);
    const int expected_output = 4;
    AssertIntEquals(tc, expected_output, actual);
}

void moreThanSingleSpaceTest(UnitTest *tc)
{
    const char *sentence = "namasthey    welcome";
    const int actual = numberOfWords(sentence);
    const int expected_output = 2;
    AssertIntEquals_Msg(tc, "numberOfWords(\"namasthey    welcome\")", expected_output, actual);
}

void extraSpacesTest(UnitTest *tc)
{
    const char *sentence = "   namasthey    welcome  ";
    const int actual = numberOfWords(sentence);
    const int expected_output = 2;
    AssertIntEquals(tc, expected_output, actual);
}


int main()
{
    int returnStatus = 0;
    TestSuite *tests = TestSuiteNew();

    SUITE_ADD_TEST(tests, singleWordTest);
    SUITE_ADD_TEST(tests, multipleWordTest);
    SUITE_ADD_TEST(tests, moreThanSingleSpaceTest);
    SUITE_ADD_TEST(tests, extraSpacesTest);

    TestSuiteRun(tests);
    returnStatus = tests->failCount;
    TestSuiteDelete(tests);

    return returnStatus;
}
#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "UnitTest.h"

/*-------------------------------------------------------------------------*
 * UTStr
 *-------------------------------------------------------------------------*/

char* UTStrAlloc(int size)
{
	char* newStr = (char*) malloc( sizeof(char) * (size) );
	return newStr;
}

char* UTStrCopy(const char* old)
{
	int len = strlen(old);
	char* newStr = UTStrAlloc(len + 1);
	strcpy(newStr, old);
	return newStr;
}

/*-------------------------------------------------------------------------*
 * UTString
 *-------------------------------------------------------------------------*/

void UTStringInit(UTString* str)
{
	str->length = 0;
	str->size = STRING_MAX;
	str->buffer = (char*) malloc(sizeof(char) * str->size);
	str->buffer[0] = '\0';
}

UTString* UTStringNew(void)
{
	UTString* str = (UTString*) malloc(sizeof(UTString));
	str->length = 0;
	str->size = STRING_MAX;
	str->buffer = (char*) malloc(sizeof(char) * str->size);
	str->buffer[0] = '\0';
	return str;
}

void UTStringDelete(UTString *str)
{
        if (!str) return;
        free(str->buffer);
        free(str);
}

void UTStringResize(UTString* str, int newSize)
{
	str->buffer = (char*) realloc(str->buffer, sizeof(char) * newSize);
	str->size = newSize;
}

void UTStringAppend(UTString* str, const char* text)
{
	int length;

	if (text == NULL) {
		text = "NULL";
	}

	length = strlen(text);
	if (str->length + length + 1 >= str->size)
		UTStringResize(str, str->length + length + 1 + STRING_INC);
	str->length += length;
	strcat(str->buffer, text);
}

void UTStringAppendChar(UTString* str, char ch)
{
	char text[2];
	text[0] = ch;
	text[1] = '\0';
	UTStringAppend(str, text);
}

void UTStringAppendFormat(UTString* str, const char* format, ...)
{
	va_list argp;
	char buf[HUGE_STRING_LEN];
	va_start(argp, format);
	vsprintf(buf, format, argp);
	va_end(argp);
	UTStringAppend(str, buf);
}

void UTStringInsert(UTString* str, const char* text, int pos)
{
	int length = strlen(text);
	if (pos > str->length)
		pos = str->length;
	if (str->length + length + 1 >= str->size)
		UTStringResize(str, str->length + length + 1 + STRING_INC);
	memmove(str->buffer + pos + length, str->buffer + pos, (str->length - pos) + 1);
	str->length += length;
	memcpy(str->buffer + pos, text, length);
}

/*-------------------------------------------------------------------------*
 * UnitTest
 *-------------------------------------------------------------------------*/

void UnitTestInit(UnitTest* t, const char* name, TestFunction function)
{
	t->name = UTStrCopy(name);
	t->failed = 0;
	t->ran = 0;
        t->message = NULL;
	t->function = function;
	t->jumpBuf = NULL;
}

UnitTest* UnitTestNew(const char* name, TestFunction function)
{
	UnitTest* tc = UT_ALLOC(UnitTest);
	UnitTestInit(tc, name, function);
	return tc;
}

void UnitTestDelete(UnitTest *t)
{
        if (!t) return;
        UTStringDelete(t->message);
        free(t->name);
        free(t);
}

void UnitTestRun(UnitTest* tc)
{
	jmp_buf buf;
	tc->jumpBuf = &buf;
	if (setjmp(buf) == 0)
	{
		tc->ran = 1;
		(tc->function)(tc);
	}
	tc->jumpBuf = 0;
}

//
// modifid the original method above to display message like this:
// <message> (at <file>:<lineno>)
//
static void TestFailInternal(UnitTest* tc, const char* file, int line, UTString* string)
{
	char buf[HUGE_STRING_LEN];

	sprintf(buf, "%s (at %s:%d)", string->buffer, file, line);

	tc->failed = 1;
	free(tc->message);
	tc->message = UTStringNew();
	UTStringAppend(tc->message, buf);
	if (tc->jumpBuf != 0) longjmp(*(tc->jumpBuf), 0);
}

void TestFail_Line(UnitTest* tc, const char* file, int line, const char* message2, const char* message)
{
	UTString string;

	UTStringInit(&string);
	if (message2 != NULL) 
	{
		UTStringAppend(&string, message2);
		UTStringAppend(&string, ": ");
	}
	UTStringAppend(&string, message);
	TestFailInternal(tc, file, line, &string);
}

void Assert_Line(UnitTest* tc, const char* file, int line, const char* message, int condition)
{
	if (condition) return;
	TestFail_Line(tc, file, line, NULL, message);
}

void AssertStrEquals_LineMsg(UnitTest* tc, const char* file, int line, const char* message, 
	const char* expected, const char* actual)
{
	UTString string;
	if ((expected == NULL && actual == NULL) ||
	    (expected != NULL && actual != NULL &&
	     strcmp(expected, actual) == 0))
	{
		return;
	}

	UTStringInit(&string);
	if (message != NULL) 
	{
		UTStringAppend(&string, message);
		UTStringAppend(&string, ": ");
	}
	UTStringAppend(&string, "expected <");
	UTStringAppend(&string, expected);
	UTStringAppend(&string, "> but was <");
	UTStringAppend(&string, actual);
	UTStringAppend(&string, ">");
	TestFailInternal(tc, file, line, &string);
}

void AssertIntEquals_LineMsg(UnitTest* tc, const char* file, int line, const char* message, 
	int expected, int actual)
{
	char buf[STRING_MAX];
	if (expected == actual) return;
	sprintf(buf, "expected <%d> but was <%d>", expected, actual);
	TestFail_Line(tc, file, line, message, buf);
}

void AssertDblEquals_LineMsg(UnitTest* tc, const char* file, int line, const char* message, 
	double expected, double actual, double delta)
{
	char buf[STRING_MAX];
	if (fabs(expected - actual) <= delta) return;
	sprintf(buf, "expected <%f> but was <%f>", expected, actual); 

	TestFail_Line(tc, file, line, message, buf);
}

void AssertPtrEquals_LineMsg(UnitTest* tc, const char* file, int line, const char* message, 
	void* expected, void* actual)
{
	char buf[STRING_MAX];
	if (expected == actual) return;
	sprintf(buf, "expected pointer <0x%p> but was <0x%p>", expected, actual);
	TestFail_Line(tc, file, line, message, buf);
}


/*-------------------------------------------------------------------------*
 * TestSuite
 *-------------------------------------------------------------------------*/

void TestSuiteInit(TestSuite* testSuite)
{
	testSuite->count = 0;
	testSuite->failCount = 0;
        memset(testSuite->list, 0, sizeof(testSuite->list));
}

TestSuite* TestSuiteNew(void)
{
	TestSuite* testSuite = UT_ALLOC(TestSuite);
	TestSuiteInit(testSuite);
	return testSuite;
}

void TestSuiteDelete(TestSuite *testSuite)
{
        unsigned int n;
        for (n=0; n < MAX_TEST_CASES; n++)
        {
                if (testSuite->list[n])
                {
                        UnitTestDelete(testSuite->list[n]);
                }
        }
        free(testSuite);

}

void TestSuiteAdd(TestSuite* testSuite, UnitTest *testCase)
{
	assert(testSuite->count < MAX_TEST_CASES);
	testSuite->list[testSuite->count] = testCase;
	testSuite->count++;
}

void TestSuiteAddSuite(TestSuite* testSuite, TestSuite* testSuite2)
{
	int i;
	for (i = 0 ; i < testSuite2->count ; ++i)
	{
		UnitTest* testCase = testSuite2->list[i];
		TestSuiteAdd(testSuite, testCase);
	}
}

void TestSuiteRunOrig(TestSuite* testSuite)
{
	int i;
	for (i = 0 ; i < testSuite->count ; ++i)
	{
		UnitTest* testCase = testSuite->list[i];
		UnitTestRun(testCase);
		if (testCase->failed) { testSuite->failCount += 1; }
	}
}

void TestSuiteSummary(TestSuite* testSuite, UTString* summary)
{
	int i;
	for (i = 0 ; i < testSuite->count ; ++i)
	{
		UnitTest* testCase = testSuite->list[i];
		UTStringAppend(summary, testCase->failed ? "F" : ".");
	}
	UTStringAppend(summary, "\n\n");
}

void TestSuiteDetails(TestSuite* testSuite, UTString* details)
{
	int i;
	int failCount = 0;

	if (testSuite->failCount == 0)
	{
		int passCount = testSuite->count - testSuite->failCount;
		const char* testWord = passCount == 1 ? "test" : "tests";
		UTStringAppendFormat(details, "OK (%d %s)\n", passCount, testWord);
	}
	else
	{
		if (testSuite->failCount == 1)
			UTStringAppend(details, "There was 1 failure:\n");
		else
			UTStringAppendFormat(details, "There were %d failures:\n", testSuite->failCount);

		for (i = 0 ; i < testSuite->count ; ++i)
		{
			UnitTest* testCase = testSuite->list[i];
			if (testCase->failed)
			{
				failCount++;
				UTStringAppendFormat(details, "%d) %s: %s\n",
					failCount, testCase->name, testCase->message->buffer);
			}
		}
		UTStringAppend(details, "\n!!!FAILURES!!!\n");

		UTStringAppendFormat(details, "Runs: %d ",   testSuite->count);
		UTStringAppendFormat(details, "Passes: %d ", testSuite->count - testSuite->failCount);
		UTStringAppendFormat(details, "Fails: %d\n",  testSuite->failCount);
	}
}

void TestSuiteDisplayDetails(TestSuite* testSuite)
{
	int i;
	int maxTestCaseNameLen = 36;
	for (i = 0 ; i < testSuite->count ; ++i)
	{
		UnitTest* testCase = testSuite->list[i];

		if (testCase->failed)
		{
			printf("%d: %-*s  FAIL  %s\n",
				i + 1, maxTestCaseNameLen, testCase->name,
				testCase->message->buffer);
		}
		else
		{
			printf("%d: %-*s  PASS\n",
				i + 1, maxTestCaseNameLen, testCase->name);
		}
	}
}
void TestSuiteRun(TestSuite* testSuite)
{
	TestSuiteRunOrig(testSuite);
	TestSuiteDisplayDetails(testSuite);
}
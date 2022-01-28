#ifndef E42_UNIT_TEST_H
#define E42_UNIT_TEST_H

#include <setjmp.h>
#include <stdarg.h>

#define UNIT_TEST_VERSION  "UnitTest 42.01"

/* UTString */

char* UTStrAlloc(int size);
char* UTStrCopy(const char* old);

#define UT_ALLOC(TYPE)		((TYPE*) malloc(sizeof(TYPE)))

#define HUGE_STRING_LEN	8192
#define STRING_MAX		256
#define STRING_INC		256

typedef struct
{
	int length;
	int size;
	char* buffer;
} UTString;

void UTStringInit(UTString* str);
UTString* UTStringNew(void);
void UTStringRead(UTString* str, const char* path);
void UTStringAppend(UTString* str, const char* text);
void UTStringAppendChar(UTString* str, char ch);
void UTStringAppendFormat(UTString* str, const char* format, ...);
void UTStringInsert(UTString* str, const char* text, int pos);
void UTStringResize(UTString* str, int newSize);
void UTStringDelete(UTString* str);

/* UnitTest */

typedef struct UnitTest UnitTest;

typedef void (*TestFunction)(UnitTest *);

struct UnitTest
{
	char* name;
	TestFunction function;
	int failed;
	int ran;
	UTString *message;
	jmp_buf *jumpBuf;
};

void UnitTestInit(UnitTest* t, const char* name, TestFunction function);
UnitTest* UnitTestNew(const char* name, TestFunction function);
void UnitTestRun(UnitTest* tc);
void UnitTestDelete(UnitTest *t);

/* Internal versions of assert functions -- use the public versions */
void TestFail_Line(UnitTest* tc, const char* file, int line, const char* message2, const char* message);
void Assert_Line(UnitTest* tc, const char* file, int line, const char* message, int condition);
void AssertStrEquals_LineMsg(UnitTest* tc, 
	const char* file, int line, const char* message, 
	const char* expected, const char* actual);
void AssertIntEquals_LineMsg(UnitTest* tc, 
	const char* file, int line, const char* message, 
	int expected, int actual);
void AssertDblEquals_LineMsg(UnitTest* tc, 
	const char* file, int line, const char* message, 
	double expected, double actual, double delta);
void AssertPtrEquals_LineMsg(UnitTest* tc, 
	const char* file, int line, const char* message, 
	void* expected, void* actual);

/* public assert functions */

#define TestFail(tc, ms)                        TestFail_Line(  (tc), __FILE__, __LINE__, NULL, (ms))
#define Assert(tc, ms, cond)                Assert_Line((tc), __FILE__, __LINE__, (ms), (cond))
#define AssertTrue(tc, cond)                Assert_Line((tc), __FILE__, __LINE__, "assert failed", (cond))

#define AssertStrEquals(tc,ex,ac)           AssertStrEquals_LineMsg((tc),__FILE__,__LINE__,NULL,(ex),(ac))
#define AssertStrEquals_Msg(tc,ms,ex,ac)    AssertStrEquals_LineMsg((tc),__FILE__,__LINE__,(ms),(ex),(ac))
#define AssertIntEquals(tc,ex,ac)           AssertIntEquals_LineMsg((tc),__FILE__,__LINE__,NULL,(ex),(ac))
#define AssertIntEquals_Msg(tc,ms,ex,ac)    AssertIntEquals_LineMsg((tc),__FILE__,__LINE__,(ms),(ex),(ac))
#define AssertDblEquals(tc,ex,ac,dl)        AssertDblEquals_LineMsg((tc),__FILE__,__LINE__,NULL,(ex),(ac),(dl))
#define AssertDblEquals_Msg(tc,ms,ex,ac,dl) AssertDblEquals_LineMsg((tc),__FILE__,__LINE__,(ms),(ex),(ac),(dl))
#define AssertPtrEquals(tc,ex,ac)           AssertPtrEquals_LineMsg((tc),__FILE__,__LINE__,NULL,(ex),(ac))
#define AssertPtrEquals_Msg(tc,ms,ex,ac)    AssertPtrEquals_LineMsg((tc),__FILE__,__LINE__,(ms),(ex),(ac))

#define AssertPtrNotNull(tc,p)        Assert_Line((tc),__FILE__,__LINE__,"null pointer unexpected",((p) != NULL))
#define AssertPtrNotNullMsg(tc,msg,p) Assert_Line((tc),__FILE__,__LINE__,(msg),((p) != NULL))

/* TestSuite */

#define MAX_TEST_CASES	1024

#define SUITE_ADD_TEST(SUITE,TEST)	TestSuiteAdd(SUITE, UnitTestNew(#TEST, TEST))

typedef struct
{
	int count;
	UnitTest* list[MAX_TEST_CASES];
	int failCount;

} TestSuite;


void TestSuiteInit(TestSuite* testSuite);
TestSuite* TestSuiteNew(void);
void TestSuiteDelete(TestSuite *testSuite);
void TestSuiteAdd(TestSuite* testSuite, UnitTest *testCase);
void TestSuiteAddSuite(TestSuite* testSuite, TestSuite* testSuite2);
void TestSuiteRun(TestSuite* testSuite);
void TestSuiteSummary(TestSuite* testSuite, UTString* summary);
void TestSuiteDetails(TestSuite* testSuite, UTString* details);

//
// place holders to fill in the blank with integer, string, array
//
#define ___ -99  // 3 underscores;  fill in the blank with integer
#define ____ ""  // 4 underscores; fill in the blank with string

// 6 underscores; array of 10 size, fill in the blank with array
#define ______ {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}

#endif /* E42_UNIT_TEST_H */

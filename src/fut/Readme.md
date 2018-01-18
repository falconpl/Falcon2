# Falcon Unit Test Framework

The Falcon Framework for Unit Tests (_FUT_) is a simple C++ framework for creating and running C++ unit tests. 
It is mainly targeted at the development of the Falcon Programming Language, and serves as the backbone of all
C++ unit testing, but it's designed to be a standalone dynamic library.

The framework is designed to losely mimic __GTEST__, and was primarily written to avoid the dependency of 
Falcon build environment from __GTEST__ while providing similar functionalities.

## Bootstrapping unit tests

The FUT subsystem can be accessed by including `falcon/fut/fut.h` and linking against the `falcon_fut` dynamic library. The `cmake/utils.cmake` extension for __cmake__ contains the function `falcon_add_fut` to add a unit test as a compilation target and to __ctest__.

## In a nutshell

A Falcon unit test is a collection of tests on _components_. You can see the component as a significant unit of your code (a class, a signleton, a set of a related functions) or more loosely as a _category_ grouping similar tests.

The macro `TEST(component, test)`  is used to define a single test in the given category.

> __NOTE__: By definining `FALCON_TEST_DONT_DEFINE_TEST`, the `TEST` macro will not be defined; instead `FALCON_TEST` will be declared.

Tests within component are grouped and executed together even if declared in different places of the same unit test, so that it's possible to determine the total runtime of all the tests in the component. 

> __NOTE__: to force the order of execution of tests within a unit test, use a lexicografic ordered name; for example
> you can prefix all the components and tests with a number like `TEST( 01_FirstComponent, 03_thirdTest )`

The macro `FALCON_TEST_MAIN` should be also used to import the common `main()` functions used to perform all the tests in the file, and process the command line options understood by the unit tests.

> __NOTE__: Since the Falcon Unit Tests is compiled as a dynamic library, it's not possible to automatically provide a `main()` function on all the OS.

### Example

The following is a minimal complete test.

```c++
// unittest_test.cpp
#include <falcon/fut/fut.h>

TEST(Component, test) {
  // ... do some things
  int value = 10;
  EXCEPT_EQ( 10, value );
}

FALCON_TEST_MAIN
```

The file can be build and run via ctest adding the following command to a `CMakeList.txt` file.

```
falcon_add_fut(unittest_test)
```

## Check Macros

The following macros defined in `falcon/fut/fut.h` can be used to check for tests to be passed.

* `EXPECT_EQ(EXPECTED, ACTUAL)` - The `EXPECTED` value must compare `==` to `ACTUAL`
* `EXPECT_NE(EXPECTED, ACTUAL)` - The `EXPECTED` value must compare `!=` to `ACTUAL`
* `EXPECT_GT(EXPECTED, ACTUAL)` - The `EXPECTED` value must compare `>` to `ACTUAL`
* `EXPECT_GE(EXPECTED, ACTUAL)` - The `EXPECTED` value must compare `>=` to `ACTUAL`
* `EXPECT_LT(EXPECTED, ACTUAL)` - The `EXPECTED` value must compare `<` to `ACTUAL`
* `EXPECT_LE(EXPECTED, ACTUAL)` - The `EXPECTED` value must compare `<=` to `ACTUAL`
* `EXPECT_STREQ(EXPECTED, ACTUAL)` - `EXPECTED` and `ACTUAL` must be equal strings.
* `EXPECT_STRNE(EXPECTED, ACTUAL)` - `EXPECTED` and `ACTUAL` must be different strings.
* `EXPECT_TRUE(ACTUAL)` - `ACTUAL` Must evaluate to true.
* `EXPECT_FALSE(ACTUAL)` - `ACTUAL` Must evaluate to false.
* `EXPECT_FLOAT_EQ(EXPECTED, ACTUAL)` - `EXPECTED` and `ACTUAL` must be almost equal floating point values.
* `EXPECT_FLOAT_NE(EXPECTED, ACTUAL)` - `EXPECTED` and `ACTUAL` must be widely different floating point values.

In every case, the `EXPECTED` value should be the literal one against an expression `ACTUAL` is evaluated; this is because the `ACTUAL` expression will be evaluated and given both in its literal and evaluated representation when reporting test failures.

## Exception Check Macros

The following macros can be used to check for code slices in `EXPRESSION` to generate or not to generate an exception, possibly an excepton of a certain kind

* `EXPECT_NO_THROW(EXPRESSION)` - The `EXPRESSION` must not throw any exception.
* `EXPECT_THROW(EXPRESSION, CATCH)` - The `EXPRESSION` must throw an exception of type `CATCH`.
* `EXPECT_ANY_THROW(EXPRESSION)` - The `EXPRESSION` must throw an exception of any kind.

In every case, `EXPRESSION` can be anything that can be place inside a `try{}` block; as such, it can consists of a sequence of statements.

## Explicit failure

The `FAIL(MESSAGE)` macro will generate an immediate failure that will be explained with the given `MESSAGE`. The test will return immediately.

> __NOTE__: FUT doesn't provide failure message streams like __GTEST__. If the caller needs to print a pretty error message, it must format the message through other means, i.e. using a `std::stringstream`.

## Common Test Initializations as Fixtures

A unit test _fixture_ is a simple way to initialize multiple tests with the same data. This could be achieved also by providing a function that each test would call in its body, possibly at its beginning, but the fixture reduces the need for cut & paste and boilerplate code.

Basically, a _fixture_ is a base class that will be extended by each test; its `SetUp` method will be called before invoking the test, and its `TearDown` method will be invoked after the test is complete.

To declare a _fixture_:
* Derive a class from testing::TestCase
* Name the class after the component to be tested
* Declare all the members that must be accessed by the tests as public.
* Implement the `void SetUp()` and `void TearDown()` methods as necessary to prepare and eventually delete the class.

To use a fixture, declare the test through the `TEST_F` macro (or `FALCON_TEST_F` if `FALCON_TEST_DONT_DEFINE_TEST` is declared).

Example:

```C++
#include <falcon/fut/fut.h>

class MyComponent: public testing::TestCase {
public:
  //declared as a pointer so we have a reason for TearDown
  std::vector<int> *the_vector;
  
  void SetUp() {
    the_vector =  new std::vector<int>;
    the_vector->push_back(1);
    the_vector->push_back(2);
  }
  
  void TearDown() {
    delete the_vector;
  }
};

TEST_F( MyComponent, vectorNotEmpty ) {
  // We can use public/protected members of MyComponent directly
  EXPECT_EQ(2, the_vector->size());
}

TEST_F( MyComponent, firstIsZero ) {
  // We can use public/protected members of MyComponent directly
  EXPECT_EQ(1, the_vector->front());
}

FALCON_TEST_MAIN
```

> __NOTE__ Each test is seen as a different instance of the _fixture_. `SetUp()` and `TearDown()` 
> are called on different instances at each test, and any change to the data performed in a test is invisible to others.

## Command line parameters and error reporting

Each unit test is a stand-alone executable program (using `falcon_fut` dynamic library). The FALCON_TEST_MAIN macro provides some basic command line parameter parsing.

* __-e__  - Fail tests on STDERR output. If the test generate some output on std::err, this will be considered as if calling `FAIL()`.
* __-i__  - Set an explicit ID for the unit test. Normally, the unit test receives the filename of the program in which it runs as its ID for error reporting (JTEST format). This option allows to override this setting.
* __-h__ - Inline help.
* __-n__ - Set an explicit name for the UnitTest. Normally, the unit test receives the filename of the program in which it runs as its Name for error reporting (JTEST format). This option allows to override this setting.
* __-q__ - Run quietly (suppress all output).
* __-t _NAME___ - Run given test only. The test name must be given in the format "Component::name". The option can be specified multiple times to run multiple tests.
* __-v _N___ - Sets verbosity level to N:
** 0: equivalent to __-q__
** 1: Report only a summary at the end of the whole unit test
** 2: Report the result of each unit test after its completion.
** 3: Announce each unit test before running it and report result of each unit test after its completion.
** 4: like 3, but also dump any text written on std::cerr by the tests.
** 5: like 4, but also dump any text written on std::out by the tests.
If the verbosity is 2 or above, std::cout and/or std::cerr will be dumped in case the test fails, while they will be dumped if the test succeeds only with verbosity 4 (std::cerr) or 5 (std::cout).
* __-x _FILE___ Save an XML report in JTEST schema t the given _FILE_. Useful for Jenkins automation.

    

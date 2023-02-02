#pragma once
#include <vector>
#include <string_view>
#include <ostream>
#include <map>

namespace TDDLib
{
    class ConfirmException
    {
    public:
        ConfirmException(int line)
            : mLine(line)
        {
        }

        virtual ~ConfirmException() = default;

        std::string_view reason() const
        {
            return mReason;
        }

        int line() const
        {
            return mLine;
        }

    protected:
        std::string mReason;
        int mLine;
    };

    class BoolConfirmException : public ConfirmException
    {
    public:
        BoolConfirmException(bool expected, int line)
            : ConfirmException(line)
        {
            mReason += "    Expected: ";
            mReason += expected ? "true" : "false";
        }
    };

    class ActualConfirmException : public ConfirmException
    {
    public:
        ActualConfirmException(
            std::string_view expected,
            std::string_view actual,
            int line)
            : ConfirmException(line),
              mExpected(expected),
              mActual(actual)
        {
            formatReason();
        }

    private:
        void formatReason()
        {
            mReason += "    Expected: " + mExpected + "\n";
            mReason += "    Actual  : " + mActual;
        }

        std::string mExpected;
        std::string mActual;
    };

    class MissingException
    {
    public:
        MissingException(std::string_view exType)
            : mExType(exType)
        {
        }
        std::string_view exType() const
        {
            return mExType;
        }

    private:
        std::string mExType;
    };

    inline void confirm(
        bool expected,
        bool actual,
        int line)
    {
        if (actual != expected)
        {
            throw BoolConfirmException(expected, line);
        }
    }
    inline void confirm(
        std::string_view expected,
        std::string_view actual,
        int line)
    {
        if (actual != expected)
        {
            throw ActualConfirmException(
                expected,
                actual,
                line);
        }
    }

    inline void confirm(
        std::string const &expected,
        std::string const &actual,
        int line)
    {
        confirm(
            std::string_view(expected),
            std::string_view(actual),
            line);
    }

    inline void confirm(
        float expected,
        float actual,
        int line)
    {
        if (actual < (expected - 0.0001f) ||
            actual > (expected + 0.0001f))
        {
            throw ActualConfirmException(
                std::to_string(expected),
                std::to_string(actual),
                line);
        }
    }

    inline void confirm(
        double expected,
        double actual,
        int line)
    {
        if (actual < (expected - 0.000001) ||
            actual > (expected + 0.000001))
        {
            throw ActualConfirmException(
                std::to_string(expected),
                std::to_string(actual),
                line);
        }
    }

    inline void confirm(
        long double expected,
        long double actual,
        int line)
    {
        if (actual < (expected - 0.000001) ||
            actual > (expected + 0.000001))
        {
            throw ActualConfirmException(
                std::to_string(expected),
                std::to_string(actual),
                line);
        }
    }

    template <typename T>
    void confirm(
        T const &expected,
        T const &actual,
        int line)
    {
        if (actual != expected)
        {
            throw ActualConfirmException(
                std::to_string(expected),
                std::to_string(actual),
                line);
        }
    }

    class Test;
    class TestSuite;

    inline std::map<std::string, std::vector<Test *>> &getTests()
    {
        static std::map<std::string, std::vector<Test *>> tests;

        return tests;
    }

    inline std::map<std::string, std::vector<TestSuite *>> &getTestSuites()
    {
        static std::map<std::string, std::vector<TestSuite *>> suites;

        return suites;
    }

    inline void addTest(std::string_view suiteName, Test *test)
    {
        std::string name(suiteName);
        if (not getTests().contains(name))
        {
            getTests().try_emplace(name, std::vector<Test *>());
        }
        getTests()[name].push_back(test);
    }

    inline void addTestSuite(std::string_view suiteName, TestSuite *suite)
    {
        std::string name(suiteName);
        if (not getTestSuites().contains(name))
        {
            getTestSuites().try_emplace(name, std::vector<TestSuite *>());
        }
        getTestSuites()[name].push_back(suite);
    }

    class TestBase
    {
    public:
        TestBase(std::string_view name, std::string_view suiteName)
            : m_name(name),
              m_suiteName(suiteName),
              m_passed(true),
              m_confirmLocation(-1)
        {
        }

        std::string_view name() const
        {
            return m_name;
        }

        std::string_view reason() const
        {
            return m_reason;
        }

        bool passed()
        {
            return m_passed;
        }
        void setExpectedFailureReason(std::string_view expectedReason)
        {
            m_expectedReason = expectedReason;
        }

        std::string_view expectedReason() const
        {
            return m_expectedReason;
        }
        void setFailed(std::string_view reason, int confirmLocation = -1)
        {
            m_passed = false;
            m_reason = reason;
            m_confirmLocation = confirmLocation;
        }

        int confirmLocation() const
        {
            return m_confirmLocation;
        }

    private:
        std::string m_name;
        std::string m_suiteName;
        bool m_passed;
        std::string m_reason;
        std::string m_expectedReason;
        int m_confirmLocation;
    };

    class Test : public TestBase
    {
    public:
        Test(std::string_view name, std::string_view suiteName)
            : TestBase(name, suiteName)
        {
            addTest(suiteName, this);
        }

        virtual void runEx()
        {
            run();
        }

        virtual void run() = 0;

        std::string_view expectedReason() const
        {
            return mExpectedReason;
        }

        void setExpectedFailureReason(std::string_view reason)
        {
            mExpectedReason = reason;
        }

    private:
        std::string mExpectedReason;
    };

    template <typename ExceptionT>
    class TestEx : public Test
    {
    public:
        TestEx(std::string_view name,
               std::string_view suiteName,
               std::string_view exceptionName)
            : Test(name, suiteName), mExceptionName(exceptionName)
        {
        }

        void runEx() override
        {
            try
            {
                run();
            }
            catch (ExceptionT const &)
            {
                return;
            }

            throw MissingException(mExceptionName);
        }

    private:
        std::string mExceptionName;
    };

    class TestSuite : public TestBase
    {
    public:
        TestSuite(
            std::string_view name,
            std::string_view suiteName)
            : TestBase(name, suiteName)
        {
            addTestSuite(suiteName, this);
        }

        virtual void suiteSetup() = 0;

        virtual void suiteTeardown() = 0;
    };

     template <typename T>
    class SetupAndTeardown : public T
    {
    public:
        SetupAndTeardown()
        {
            T::setup();
        }
        ~SetupAndTeardown()
        {
            T::teardown();
        }
    };

    template <typename T>
    class TestSuiteSetupAndTeardown : public T,
                                      public TestSuite
    {
    public:
        TestSuiteSetupAndTeardown(
            std::string_view name,
            std::string_view suite)
            : TestSuite(name, suite)
        {
        }

        void suiteSetup() override
        {
            T::setup();
        }

        void suiteTeardown() override
        {
            T::teardown();
        }
    };

    inline void runTest(std::ostream &output, Test *test,
                        int &numPassed, int &numFailed, int &numMissedFailed)
    {
        output << "------- Test: "
               << test->name()
               << std::endl;

        try
        {
            test->runEx();
        }
        catch (ConfirmException const &ex)
        {
            test->setFailed(ex.reason(), ex.line());
        }
        catch (MissingException const &ex)
        {
            std::string message = "Expected exception type ";
            message += ex.exType();
            message += " was not thrown.";
            test->setFailed(message);
        }
        catch (...)
        {
            test->setFailed("Unexpected exception thrown.");
        }

        if (test->passed())
        {
            if (not test->expectedReason().empty())
            {
                // This test passed but it was supposed
                // to have failed.
                ++numMissedFailed;
                output << "Missed expected failure\n"
                       << "Test passed but was expected to fail."
                       << std::endl;
            }
            else
            {
                ++numPassed;
                output << "Passed"
                       << std::endl;
            }
        }
        else if (not test->expectedReason().empty() &&
                 test->expectedReason() == test->reason())
        {
            ++numPassed;
            output << "Expected failure\n"
                   << test->reason()
                   << std::endl;
        }
        else
        {
            ++numFailed;
            if (test->confirmLocation() != -1)
            {
                output << "Failed confirm on line "
                       << test->confirmLocation() << "\n";
            }
            else
            {
                output << "Failed\n";
            }
            output << test->reason()
                   << std::endl;
        }
    }

    inline bool runSuite(std::ostream &output,
                         bool setup, std::string const &name,
                         int &numPassed, int &numFailed)
    {
        for (auto &suite : getTestSuites()[name])
        {
            if (setup)
            {
                output << "------- Setup: ";
            }
            else
            {
                output << "------- Teardown: ";
            }
            output << suite->name()
                   << std::endl;

            try
            {
                if (setup)
                {
                    suite->suiteSetup();
                }
                else
                {
                    suite->suiteTeardown();
                }
            }
            catch (ConfirmException const &ex)
            {
                suite->setFailed(ex.reason(), ex.line());
            }
            catch (...)
            {
                suite->setFailed("Unexpected exception thrown.");
            }

            if (suite->passed())
            {
                ++numPassed;
                output << "Passed"
                       << std::endl;
            }
            else
            {
                ++numFailed;
                if (suite->confirmLocation() != -1)
                {
                    output << "Failed confirm on line "
                           << suite->confirmLocation() << "\n";
                }
                else
                {
                    output << "Failed\n";
                }
                output << suite->reason()
                       << std::endl;
                return false;
            }
        }
        return true;
    }

    inline unsigned int runTests(std::ostream &output)
    {
        output << "Running "
               << getTests().size()
               << " test suites\n";

        int numPassed = 0;
        int numMissedFailed = 0;
        int numFailed = 0;
        for (auto const &[key, value] : getTests())
        {
            std::string suiteDisplayName = "Suite: ";
            if (key.empty())
            {
                suiteDisplayName += "Single Tests";
            }
            else
            {
                suiteDisplayName += key;
            }
            output << "--------------- "
                   << suiteDisplayName
                   << std::endl;

            if (not key.empty())
            {
                if (not getTestSuites().contains(key))
                {
                    output << "Test suite is not found."
                           << " Exiting test application."
                           << std::endl;
                    return ++numFailed;
                }

                if (not runSuite(output, true, key,
                                 numPassed, numFailed))
                {
                    output << "Test suite setup failed."
                           << " Skipping tests in suite."
                           << std::endl;
                    continue;
                }
            }

            for (auto *test : value)
            {
                runTest(output, test,
                        numPassed, numFailed, numMissedFailed);
            }

            if (not key.empty())
            {
                if (not runSuite(output, false, key,
                                 numPassed, numFailed))
                {
                    output << "Test suite teardown failed."
                           << std::endl;
                }
            }
        }

        output << "---------------\n";
        if (numFailed == 0 && numMissedFailed == 0)
        {
            output << "All tests passed."
                   << std::endl;
        }
        else
        {
            output << "Tests passed: " << numPassed
                   << "\nTests failed: " << numFailed;
        }
        if (numMissedFailed != 0)
        {
            output << "\nTests failures missed: " << numMissedFailed;
        }
        output << std::endl;
        return numFailed;
    }
}

#define TDDLIB_CLASS_FINAL(line) Test##line
#define TDDLIB_CLASS_RELAY(line) TDDLIB_CLASS_FINAL(line)
#define TDDLIB_CLASS TDDLIB_CLASS_RELAY(__LINE__)
#define TDDLIB_INSTANCE_FINAL(line) test##line
#define TDDLIB_INSTANCE_RELAY(line) TDDLIB_INSTANCE_FINAL(line)
#define TDDLIB_INSTANCE TDDLIB_INSTANCE_RELAY(__LINE__)

#define TEST_SUITE(testName, suiteName)                \
    namespace                                          \
    {                                                  \
        class TDDLIB_CLASS : public TDDLib::Test       \
        {                                              \
        public:                                        \
            TDDLIB_CLASS(std::string_view name,        \
                         std::string_view suite)       \
                : Test(name, suite)                    \
            {                                          \
            }                                          \
            void run() override;                       \
        };                                             \
    } /* end of unnamed namespace */                   \
    TDDLIB_CLASS TDDLIB_INSTANCE(testName, suiteName); \
    void TDDLIB_CLASS::run()

#define TEST_SUITE_EX(testName, suiteName, exceptionType)              \
    namespace                                                          \
    {                                                                  \
        class TDDLIB_CLASS : public TDDLib::TestEx<exceptionType>      \
        {                                                              \
        public:                                                        \
            TDDLIB_CLASS(std::string_view name,                        \
                         std::string_view suite,                       \
                         std::string_view exceptionName)               \
                : TestEx(name, suite, exceptionName)                   \
            {                                                          \
            }                                                          \
            void run() override;                                       \
        };                                                             \
    } /* end of unnamed namespace */                                   \
    TDDLIB_CLASS TDDLIB_INSTANCE(testName, suiteName, #exceptionType); \
    void TDDLIB_CLASS::run()

#define TEST(testName)                           \
    namespace                                    \
    {                                            \
        class TDDLIB_CLASS : public TDDLib::Test \
        {                                        \
        public:                                  \
            TDDLIB_CLASS(std::string_view name)  \
                : Test(name, "")                 \
            {                                    \
            }                                    \
            void run() override;                 \
        };                                       \
    } /* end of unnamed namespace */             \
    TDDLIB_CLASS TDDLIB_INSTANCE(testName);      \
    void TDDLIB_CLASS::run()

#define TEST_EX(testName, exceptionType)                          \
    namespace                                                     \
    {                                                             \
        class TDDLIB_CLASS : public TDDLib::TestEx<exceptionType> \
        {                                                         \
        public:                                                   \
            TDDLIB_CLASS(std::string_view name,                   \
                         std::string_view exceptionName)          \
                : TestEx(name, "", exceptionName)                 \
            {                                                     \
            }                                                     \
            void run() override;                                  \
        };                                                        \
    } /* end of unnamed namespace */                              \
    TDDLIB_CLASS TDDLIB_INSTANCE(testName, #exceptionType);       \
    void TDDLIB_CLASS::run()

#define CONFIRM_FALSE(actual) \
    TDDLib::confirm(false, actual, __LINE__)
#define CONFIRM_TRUE(actual) \
    TDDLib::confirm(true, actual, __LINE__)
#define CONFIRM(expected, actual) \
    TDDLib::confirm(expected, actual, __LINE__)

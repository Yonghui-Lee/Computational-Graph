#pragma once
#include <iostream>
#include <vector>
#include <stdexcept>
/** \brief 测试架构
 *
 * 通过运行测试 则输出测试成功，断言通过，否则输出相关错误
 */
class TestFramework {
    const char *name;
    virtual void body() = 0;
protected:
    int num_asserts;
    TestFramework(const char* name): name(name) {}
public:
    bool run_test() {
        std::cout << "==========\n";
        std::cout << "Test: " << name << "\n";
        try {
            num_asserts = 0;
            body();
            std::cout << "Test ok, " << num_asserts << " assertion(s) passed\n";
            return true;
        } catch (const std::exception &err) {
            std::cout << "Test error: " << err.what() << "\n";
            return false;
        }
    }
};

#define TEST(name) \
    class TestFramework__##name : public TestFramework { \
        void body() override; \
    public: \
        TestFramework__##name(): TestFramework(#name) {} \
    } _test__##name; \
    void TestFramework__##name::body()

#define RUN(name) _test__##name.run_test();

/** \brief 位于某处的代码存在断言错误
 */
class AssertionError : public std::logic_error {
public:
    AssertionError(const char *expr, const char *file, int line):
        logic_error(
            "Assertion " + std::string(expr)
            + " failed, at " + std::string(file)
            + ":" + std::to_string(line)) {}
};

#define ASSERT(expr) do { \
    num_asserts ++; \
    if (! (expr)) \
        throw AssertionError(#expr, __FILE__, __LINE__); \
} while(0)

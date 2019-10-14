#pragma once

#include <stdexcept>
/** \brief 计算图中所有错误的基类
 */
class ComputeGraphError : public std::logic_error {
public:
    using logic_error::logic_error;
};

/** \brief 变量缺失
 */
class VariableMissing : public ComputeGraphError {
public:
    explicit VariableMissing(const std::string &what_arg =
        "Variable missing"):
        ComputeGraphError(what_arg) {}
};

/** \brief 占位符缺失
 */
class PlaceholderMissing : public ComputeGraphError {
public:
    explicit PlaceholderMissing(const std::string &what_arg =
        "Placeholder missing"):
        ComputeGraphError(what_arg) {}
};

/** \brief 参数数量错误
 */
class ArityError : public ComputeGraphError {
public:
    explicit ArityError(const std::string &what_arg =
        "Arity error"):
        ComputeGraphError(what_arg) {}
};

/** \brief 函数不存在
 */
class NoSuchFunction : public ComputeGraphError {
public:
    explicit NoSuchFunction(const std::string &what_arg =
        "No such function"):
        ComputeGraphError(what_arg) {}
};

/** \brief 除数为0
 */
class DivisionByZero : public ComputeGraphError {
public:
    explicit DivisionByZero(const std::string &what_arg =
        "Division by zero"):
        ComputeGraphError(what_arg) {}
};

 /** \brief log接口输入必须为正数
 */
class LogNegative : public ComputeGraphError {
public:
    explicit LogNegative(const std::string &what_arg =
        "LOG operator's input must be positive"):
        ComputeGraphError(what_arg) {}
};

 /** \brief ASSERT的报错信息
 */
class AssertionFailed : public ComputeGraphError {
public:
    explicit AssertionFailed(const std::string &what_arg =
        "Assertion failed"):
        ComputeGraphError(what_arg) {}
};

/** \brief 偏导变量不存在
 */
class NoSuchVariable : public ComputeGraphError {
public:
    explicit NoSuchVariable(const std::string &what_arg =
        "No such variable to derivative"):
        ComputeGraphError(what_arg) {}
};

/** \brief 普通的节点无法求导
 */
class NodeNotGradientifible : public ComputeGraphError {
public:
    explicit NodeNotGradientifible(const std::string &what_arg =
        "Generic node is not gradientifible"):
        ComputeGraphError(what_arg) {}
};

/** \brief 文件打开失败
 */
class FileOpenError : public ComputeGraphError {
public:
    explicit FileOpenError(const std::string &what_arg =
        "Failed to open file"):
        ComputeGraphError(what_arg) {}
};

/** \brief 位于某处有未实现的代码。在开发过程中可以在暂时没有实现的部分写上 NOT_IMPLEMENTED;
 */
class NotImplemented : public ComputeGraphError {
public:
    NotImplemented(const char *file, int line, const char *func):
        ComputeGraphError(
            "Not implemented: " + std::string(func)
            + ", at " + std::string(file)
            + ":" + std::to_string(line)) {}
};

#define NOT_IMPLEMENTED do { \
    throw NotImplemented(__FILE__, __LINE__, __PRETTY_FUNCTION__); \
} while (0)


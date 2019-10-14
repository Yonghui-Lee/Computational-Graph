#include "node_function.h"

#include <map>
#include <string>
#include <cmath>

#include "error.h"
#include "grad.h"
#include "node.h"
#include "utils.h"

/** \brief 不可求导的函数
 */
static
void none_grad(node_id, node_id, GradInfo &) {
    throw NodeNotGradientifible();
}

/** \brief 常数
 */
static
void const_grad(node_id self, node_id vn, GradInfo & gi) {
    gi.addGradAt(gi.getChild(self)[0], vn);
}


/** \brief sigmoid 导函数
 * d sigmoid(x) / dx = sigmoid(x) - sigmoid(x)^2
 */
static
void sigmoid_grad(node_id self, node_id vn, GradInfo & gi) {
    gi.addGradAt(gi.getChild(self)[0],
        gi.graph.createNode(my_make_unique<FunctionNode>("*"), {
            vn, gi.graph.createNode(my_make_unique<FunctionNode>("-"), {
                self, gi.graph.createNode(my_make_unique<FunctionNode>("*"), {self, self})
            })
        })
    );
}

/** \brief + 导函数
 * d(a + b) = 1 * d(a) + 1 * d(b)
 */
static
void plus_grad(node_id self, node_id vn, GradInfo & gi) {
    gi.addGradAt(gi.getChild(self)[0], vn);
    gi.addGradAt(gi.getChild(self)[1], vn);
}


/** \brief - 导函数
 * d(a - b) = 1 * d(a) + 1 * (- d(b))
 */
static
void minus_grad(node_id self, node_id vn, GradInfo & gi) {
    gi.addGradAt(gi.getChild(self)[0], vn);
    gi.addGradAt(gi.getChild(self)[1], gi.graph.createNode(my_make_unique<FunctionNode>("NEG"), {vn}));
}

/** \brief * 导函数
 * d(a * b) = b * d(a) + a * d(b)
 */
static
void mul_grad(node_id self, node_id vn, GradInfo & gi) {
    gi.addGradAt(gi.getChild(self)[0], gi.graph.createNode(my_make_unique<FunctionNode>("*"),
        {vn, gi.getChild(self)[1]}
    ));
    gi.addGradAt(gi.getChild(self)[1], gi.graph.createNode(my_make_unique<FunctionNode>("*"),
        {vn, gi.getChild(self)[0]}
    ));
}

/** \brief / 导函数
 * d(a / b) = (1/b) * d(a) + (a * (1/b)^2) * d(b)
 */
static
void div_grad(node_id self, node_id vn, GradInfo & gi) {
    gi.addGradAt(gi.getChild(self)[0], gi.graph.createNode(my_make_unique<FunctionNode>("*"), {
        gi.graph.createNode(my_make_unique<FunctionNode>("INV"), {gi.getChild(self)[1]}), vn
    }));
    gi.addGradAt(gi.getChild(self)[1], gi.graph.createNode(my_make_unique<FunctionNode>("*"), {
        gi.graph.createNode(my_make_unique<FunctionNode>("NEG"), {
            gi.graph.createNode(my_make_unique<FunctionNode>("*"), {
                gi.getChild(self)[0], gi.graph.createNode(my_make_unique<FunctionNode>("INV"), {
                    gi.graph.createNode(my_make_unique<FunctionNode>("*"), {
                        gi.getChild(self)[1], gi.getChild(self)[1]
                    })
                })
            })
        }), vn
    }));
}


/** \brief sin 导函数
 * d(sin(x)) = cos(x) * d(x)
 */
static
void sin_grad(node_id self, node_id vn, GradInfo & gi) {
    gi.addGradAt(gi.getChild(self)[0], gi.graph.createNode(my_make_unique<FunctionNode>("*"),
        {vn, gi.graph.createNode(my_make_unique<FunctionNode>("COS"), {gi.getChild(self)[0]})})
    );
}


/** \brief cos 导函数
 * d(cos(x)) = (-sin(x)) * d(x)
 */
static
void cos_grad(node_id self, node_id vn, GradInfo & gi) {
    gi.addGradAt(gi.getChild(self)[0], gi.graph.createNode(my_make_unique<FunctionNode>("*"), {
        vn, gi.graph.createNode(my_make_unique<FunctionNode>("NEG"), {
            gi.graph.createNode(my_make_unique<FunctionNode>("SIN"), {gi.getChild(self)[0]})
        })
    }));
}


/** \brief exp 导函数
 * d(exp(x)) = exp(x) * d(x)
 */
static
void exp_grad(node_id self, node_id vn, GradInfo & gi) {
    gi.addGradAt(gi.getChild(self)[0], gi.graph.createNode(my_make_unique<FunctionNode>("*"), {vn, self}));
}


/** \brief exp 导函数
 * d(log(x)) = (1/x) * d(x)
 */
static
void log_grad(node_id self, node_id vn, GradInfo & gi) {
    gi.addGradAt(gi.getChild(self)[0], gi.graph.createNode(my_make_unique<FunctionNode>("*"), {
        vn, gi.graph.createNode(my_make_unique<FunctionNode>("INV"), {gi.getChild(self)[0]})
    }));
}

/** \brief tanh 导函数
 * d(tanh(x)) = (1 - tanh^2(x)) * d(x)
 */
static
void tanh_grad(node_id self, node_id vn, GradInfo & gi) {
    gi.addGradAt(gi.getChild(self)[0], gi.graph.createNode(my_make_unique<FunctionNode>("*"), {
        vn, gi.graph.createNode(my_make_unique<FunctionNode>("-"), {
            gi.graph.createNode(my_make_unique<ConstantNode>(1)),
            gi.graph.createNode(my_make_unique<FunctionNode>("*"), {self, self})
        })
    }));
}


/** \brief tanh 导函数
 * d (-x) = (- d(x))
 */
static
void neg_grad(node_id self, node_id vn, GradInfo & gi) {
    gi.addGradAt(gi.getChild(self)[0], gi.graph.createNode(my_make_unique<FunctionNode>("NEG"), {vn}));
}

/** \brief 1/x 导函数
 * d (1/x) = (-(1/x)^2) * d(x)
 */
static
void inv_grad(node_id self, node_id vn, GradInfo & gi) {
    gi.addGradAt(gi.getChild(self)[0], gi.graph.createNode(my_make_unique<FunctionNode>("*"), {
        gi.graph.createNode(my_make_unique<FunctionNode>("*"), {
            gi.graph.createNode(my_make_unique<FunctionNode>("NEG"), {
                gi.graph.createNode(my_make_unique<FunctionNode>("*"), {
                    gi.getChild(self)[0], gi.getChild(self)[0]
                })
            })
        }), vn
    }));
}

/** \brief sigmoid函数
 */
static
float sigmoid(float x) {
    if (x < 0) {
        double ea = exp(x);
        return ea / (1 + ea);
    } else {
        return 1 / (1 + exp(-x));
    }
}

/** \brief 除数为0则报错，否则正常运行
 */
static
float checked_div(float x, float y) {
    if (y == 0)
        throw DivisionByZero();
    else
        return x / y;
}

/** \brief  log非正则报错
 */
static
float checked_log(float x) {
    if (x <= 0)
        throw LogNegative();
    else
        return log(x);
}

/** \brief  Assert: 参数非正则报错
 */
static
float my_assert(float x) {
    if (x <= 0)
        throw AssertionFailed();
    return 0;
}

#define UNARY(func) [](const Inputs &in) { \
    float x = in[0]; \
    return (func); \
}

#define BINARY(func) [](const Inputs &in) { \
    float x = in[0], y = in[1]; \
    return (func); \
}

/** \brief 字符对应到函数
 */
typedef std::pair<Func, GradFunc> funcGroup;
std::map<std::string, funcGroup> funcMap {
    { "+", funcGroup(BINARY(x + y), plus_grad) },
    { "-", funcGroup{BINARY(x - y), minus_grad} },
    { "*", funcGroup{BINARY(x * y), mul_grad} },
    { "/", funcGroup{BINARY(checked_div(x, y)), div_grad} },

    { "<", funcGroup{BINARY((double)(x < y)), none_grad} },
    { "<=", funcGroup{BINARY((double)(x <= y)), none_grad} },
    { ">", funcGroup{BINARY((double)(x > y)), none_grad} },
    { ">=", funcGroup{BINARY((double)(x >= y)), none_grad} },
    { "==", funcGroup{BINARY((double)(x == y)), none_grad} },
    { "!=", funcGroup{BINARY((double)(x != y)), none_grad} },

    { "SIN", funcGroup{UNARY(sin(x)), sin_grad} },
    { "COS", funcGroup{UNARY(cos(x)), cos_grad} },
    { "LOG", funcGroup{UNARY(checked_log(x)), log_grad} },
    { "EXP", funcGroup{UNARY(exp(x)), exp_grad} },
    { "TANH", funcGroup{UNARY(tanh(x)), tanh_grad} },
    { "SIGMOID", funcGroup{UNARY(sigmoid(x)), sigmoid_grad} },

    { "BIND", funcGroup{UNARY(x), const_grad} },  // Bind is binary, but it behaves as an unary
    { "ASSERT", funcGroup{UNARY(my_assert(x)), const_grad} },
	{ "NEG", funcGroup{UNARY(-x), neg_grad} },
	{ "INV", funcGroup{UNARY(checked_div(1, x)), inv_grad} },
};

/** \brief 确定函数，如果无此函数则报错
 */
static
funcGroup findFunction(const std::string &name) {
    auto it = funcMap.find(name);
    if (it == funcMap.end())
        throw NoSuchFunction();
    return it->second;
}

FunctionNode::FunctionNode(const std::string &name):
    func(findFunction(name)) {}

float FunctionNode::compute(const Inputs &in) const {
    return func.first(in);
}

void FunctionNode::grad(node_id self, node_id vn, GradInfo &gi) const {
    return func.second(self, vn, gi);
}

#include <iostream>
#include "node.h"
#include "evaluation.h"
#include "graph.h"
#include "grad.h"
#include "utils.h"

int main() {
    // 求y=e^x*sin(x)的导函数在x=0--3每隔0.1的值
    Graph g;
    // 从叶子节点开始构建表达式
    node_id varNode =
        g.createNode(my_make_unique<PlaceholderNode>("x"));
    node_id sinNode =
        g.createNode(my_make_unique<FunctionNode>("SIN"), {varNode}); // sin(x)
    node_id expNode =
        g.createNode(my_make_unique<FunctionNode>("EXP"), {varNode}); // exp(x)
    node_id mulNode =
        g.createNode(my_make_unique<FunctionNode>("*"), {expNode, sinNode});
    node_id expression = mulNode;
    // 对表达式求微分（梯度），创建名为grad的Grad类
    Grad grad(g, expression);
    // 得到对x（varNode）的偏导数
    node_id derivative = grad.at(varNode);
    // 此时整个表达式已经构建完毕，开始计算
    std::map<std::string, float> placeholderValues {{"x", 0.0}}; // 占位符的值
    std::map<std::string, float> variableValues; // 变量的值，本程序中没有使用变量，将它留空
    for (float val = 0.0; val <= 3.0; val += 0.1) {
        placeholderValues["x"] = val;
        // 用来“求值”的类，一次性，用完即丢弃，所以是循环内局部变量
        Evaluation eval(g, variableValues, std::cout, placeholderValues);
        float result = eval.compute_node(derivative);
        std::cout << "f'(" << val << ") = " << result << std::endl;
    }
    return 0;
}

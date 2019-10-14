/** \file
 *
 * \brief 求值过程
 */
#include "evaluation.h"

#include <string>

#include "node.h"
#include "error.h"

Evaluation::~Evaluation() {
    for (auto id: varChange) {
        VariableNode *v = dynamic_cast<VariableNode *>(graph.nodes[id.first].info.get());
        // it's dangerous to throw an exception in destructor
        // so we just ignore it if it's not a variable
        if (v != nullptr) {
            variables[v->get_name()] = id.second;
        }
    }
}

/** \brief 变量节点的求值过程
 */
float Evaluation::get_variable(const std::string &name) const {
    auto i = variables.find(name);
    if (i == variables.end())
        throw VariableMissing();
    else
        return i->second;
}

/** \brief 占位符节点的求值过程
 */
float Evaluation::get_placeholder(const std::string &name) const {
    auto i = placeholders.find(name);
    if (i == placeholders.end())
        throw PlaceholderMissing();
    else
        return i->second;
}

/** \brief 用于计算节点值
 */
float Evaluation::compute_node(node_id id) {
    if (available[id]) {
        return results[id];
    } else {
        Inputs in(*this, id, graph.nodes[id].deps);
        float res = graph.nodes[id].info->compute(in);
        available[id] = 1;
        results[id] = res;
        return res;
    }
}

void Evaluation::notify_change(node_id id, float value) const {
    varChange[id] = value;
}

/** \brief  重载 operator[] 访问第 i 个依赖
 */
float Inputs::operator[](size_t ix) const {
    return eval.compute_node(deps[ix]);
}

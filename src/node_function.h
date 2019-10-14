#pragma once

#include <map>
#include <string>
#include <functional>

#include "evaluation.h"
#include "node_info.h"

using Func = std::function<float(const Inputs &)>;
using GradFunc = std::function<void(node_id, node_id, GradInfo &)>;

/** \brief 各种函数节点
 */
class FunctionNode : public NodeInfo {
    std::pair<Func, GradFunc> func;
public:
    FunctionNode(const std::string &name);

    float compute(const Inputs &in) const override;
    void grad(node_id self, node_id vn, GradInfo &gi) const override;
};

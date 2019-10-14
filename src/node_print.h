#pragma once

#include <iostream>

#include "evaluation.h"
#include "node_info.h"
/** \brief 输出节点信息
 */
class PrintNode : public NodeInfo {
    std::string name;
public:
    PrintNode(std::string name):
        name(std::move(name)) {}

    float compute(const Inputs &in) const override;
};

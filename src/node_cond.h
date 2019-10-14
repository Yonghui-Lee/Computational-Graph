#pragma once

#include <iostream>

#include "evaluation.h"
#include "node_info.h"

/** \brief COND 节点
 */
class CondNode : public NodeInfo {
public:
    float compute(const Inputs &in) const override;
};

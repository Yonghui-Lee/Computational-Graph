#pragma once

#include <utility>
#include <memory>
#include <vector>
#include <string>

#include "evaluation.h"
#include "node_info.h"
#include "grad.h"

/** \brief 占位符点
 */
class PlaceholderNode : public NodeInfo {
    std::string name;
public:
    explicit PlaceholderNode(std::string name): name(std::move(name)) {}

    float compute(const Inputs &in) const override {
        return in.get_eval().get_placeholder(name);
    }
};

/** \brief 变量点
 */
class VariableNode : public NodeInfo {
    std::string name;
public:
    explicit VariableNode(std::string name): name(std::move(name)) {}

    float compute(const Inputs &in) const override {
        return in.get_eval().get_variable(name);
    }

    // for Assign
    const std::string &get_name() const { return name; }
};

/** \brief 常量点
 */
class ConstantNode : public NodeInfo {
    float value;
public:
    explicit ConstantNode(float value): value(value) {}

    float compute(const Inputs &) const override {
        return value;
    }
};

/** \brief 恒等节点，求导功能所需要的辅助节点
 */
class IdentityNode : public NodeInfo {
public:
    float compute(const Inputs &in) const override {
        return in[0];
    }

    void grad(node_id self, node_id vn, GradInfo &gi) const override {
        gi.addGradAt(gi.getChild(self)[0], vn);
    }
};


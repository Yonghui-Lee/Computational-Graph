/** \file
 *
 * \brief NodeInfo 节点信息抽象类
 */

#pragma once

#include "error.h"

#include <utility>
#include <memory>
#include <vector>
#include <string>

class Inputs;
class GradInfo;

using node_id = size_t;

/** \brief 节点信息抽象类
 *
 * 保存节点的类型和计算时所需的信息，但是不保存依赖信息。
 *
 * 在计算图中，可以认为是每个节点上面的标记。
 */
class NodeInfo {
public:

    /** \brief 计算节点的值
     *
     * \param in 节点的输入，使用方式参见 Inputs 类
     */
    virtual float compute(const Inputs &in) const = 0;
    virtual void grad(node_id, node_id, GradInfo &) const {}
    virtual ~NodeInfo() {}
};

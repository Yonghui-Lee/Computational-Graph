#include "node_cond.h"

/** \brief compute 函数实现
 *
 *若输入 Inputs in 的 in[0] 大于 0，返回 in[1] 否则返回 in[2]
 */
float CondNode::compute(const Inputs &in) const {
    return (in[0] > 0) ? in[1] : in[2];
}

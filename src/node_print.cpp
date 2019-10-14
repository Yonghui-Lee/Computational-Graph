#include "node_print.h"

#include <iomanip>

/** \brief 输出节点信息的具体实现
 */
float PrintNode::compute(const Inputs &in) const {
    float x = in[0];
    in.get_eval().get_out()
        << std::setprecision(4) << std::fixed
        << "PRINT operator: " << name << " = " << x << "\n";
    return x;
}

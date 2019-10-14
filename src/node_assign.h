#pragma once

#include "node.h"
#include "grad.h"

class AssignNode : public NodeInfo {
public:
	float compute(const Inputs& in) const {
		in.get_eval().notify_change(in.get_deps()[0], in[1]);
		return in[1];
	}
};

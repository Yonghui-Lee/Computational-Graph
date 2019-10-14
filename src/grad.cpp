#include "grad.h"
#include "node.h"
#include "utils.h"
#include <queue>
#include <memory>
#include <cassert>

using std::queue;
using std::make_pair;

static
void flatten(Graph &g, node_id id) {
	switch (g.nodes[id].deps.size()) {
	case 0:
		g.nodes[id].info = my_make_unique<ConstantNode>(1);
		break;
	case 1:
		// leave it as it was
		break;
	default:
		// connect deps with +
		auto &deps = g.nodes[id].deps;
		g.nodes[id].info = my_make_unique<FunctionNode>("+");
		while (deps.size() > 2) {
			node_id last = deps.back();
			deps.pop_back();
			deps.back() = g.createNode(my_make_unique<FunctionNode>("+"), {deps.back(), last});
		}
		break;
	}
}

node_id Grad::getVirtualNodeFor(Graph &g, node_id id) {
	auto it = m_grads.find(id);
	if (it == m_grads.end()) {
		node_id resultId = g.createNode(my_make_unique<IdentityNode>());
		m_grads.insert(make_pair(id, resultId));
		return resultId;
	} else {
		return it->second;
	}
}

void Grad::gradHelper(GradInfo &gi, node_id id) {
	if (gi.nodes_met.find(id) == gi.nodes_met.end()) {
		gi.nodes_met.insert(id);
		gi.graph.nodes[id].info->grad(id, getVirtualNodeFor(gi.graph, id), gi);
		for (node_id child: gi.graph.nodes[id].deps) {
			assert(child != id);
			gradHelper(gi, child);
		}
	}
}

// 对g中下标为id的节点求微分（梯度）
// 结果及中间节点保存在g中
Grad::Grad(Graph &g, node_id id) {
	GradInfo gi(g, *this);
	//m_zero = g.createNode(my_make_unique<ConstantNode>(0));
	// Create a virtual node for id
	getVirtualNodeFor(g, id);
	// Depth-first search
	gradHelper(gi, id);
	// flatten the intermediate nodes
	for (const auto &x : m_grads) {
		flatten(g, x.second);
	}
}

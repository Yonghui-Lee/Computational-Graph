#pragma once

#include "graph.h"
#include <set>
#include <map>
#include <string>

class Grad {
public:
	friend class GradInfo;

	Grad(Graph &g, node_id id);

	Grad() = default;
	Grad(const Grad &) = default;
	Grad(Grad &&) = default;
	Grad & operator = (const Grad &) = default;
	Grad & operator = (Grad &&) = default;

	node_id at(node_id id) {
		auto it = m_grads.find(id);
		if (it == m_grads.end()) {
			//return m_zero;
			throw NoSuchVariable();
		}
		return it->second;
	}
private:
	std::map<node_id, node_id> m_grads;
	node_id m_zero;
	node_id getVirtualNodeFor(Graph &g, node_id id);
	void gradHelper(GradInfo &gi, node_id id);
};

class GradInfo {
public:
	friend class Grad;
	void addGradAt(node_id id, node_id partToAdd) {
		graph.nodes[grad.getVirtualNodeFor(graph, id)].deps.push_back(partToAdd);
	}
	const std::vector<node_id> &getChild(node_id id) const {
		return graph.nodes[id].deps;
	}
	Graph &graph;
private:
	Grad &grad;
	std::set<node_id> nodes_met;
	GradInfo(Graph &g, Grad &gr): graph(g), grad(gr) {}
};

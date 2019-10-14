#pragma once

#include <vector>
#include <memory>
#include <cstdlib>

#include "node_info.h"

/** \brief   info 存储节点本身的信息， deps 记录节点间的结构关系
 */
class Node {
public:
/** \brief 智能指针，指向节点上的标记，即节点上的数值或操作符
 */
    std::unique_ptr<NodeInfo> info;

/** \brief 向量，存储该节点的所有依赖节点的下标
 */
    std::vector<node_id> deps;

    Node(std::unique_ptr<NodeInfo> &&info, std::vector<node_id> deps):
        info(std::move(info)), deps(std::move(deps)) {}
};

/** \brief   Node 类向量的封装类，存储计算图的所有节点
 */
class Graph {
public:
    std::vector<Node> nodes;

    Graph(std::vector<Node> nodes = std::vector<Node>()):
        nodes(std::move(nodes)) {}

	node_id createNode(std::unique_ptr<NodeInfo> &&info,
	                   std::vector<node_id> &&deps = std::vector<node_id>()) {
		node_id resultId = nodes.size();
		nodes.emplace_back(std::move(info), deps);
		return resultId;
	}
};

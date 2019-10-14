#pragma once

#include <map>
#include <utility>
#include <iostream>

#include "node_info.h"
#include "graph.h"

struct TextGraph {
    Graph graph;
    std::map<std::string, node_id> id_map;
    std::map<std::string, float> variables;

	void loadSessionFrom(const std::string &filename);

	void saveSessionTo(const std::string &filename) const;
};

/** \brief 读入计算图
 */
TextGraph read_graph(std::istream &in);

std::map<std::string, float> read_placeholders(std::istream &in);

// Support C++11
template<typename T, typename... Args>
std::unique_ptr<T> my_make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

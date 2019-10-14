/** \file
 *
 * \brief 求值环境
 */

#pragma once

#include <string>
#include <map>

#include "graph.h"
#include "error.h"

/** \brief 求值环境
 *
 * 每次对计算图节点求值的时候，都需要一个求值环境来保存当前所有变量和占位符的值，哪些节点计算过了，哪些没有，以及计算过的节点的值。
 */
class Evaluation {
    /** \brief 当前计算图
     */
    const Graph &graph;

    /** \brief 当前变量的值
     */
    std::map<std::string, float> &variables;

    /** \brief 当前输出流，用于 \c PRINT 节点
     */
    std::ostream &out;

    /** \brief 当前占位符的赋值
     */
    std::map<std::string, float> placeholders;

    /** \brief 已经计算过的节点的值，在 \ref available 对应的值为 \c 1 时有效
     */
    std::vector<float> results;

    /** \brief 每个节点是否被计算过了
     */
    std::vector<int> available;

    /** \brief 哪些变量的值即将改变
     */
    mutable std::map<node_id, float> varChange;

public:
    Evaluation(
        const Graph &graph,
        std::map<std::string, float> &variables,
        std::ostream &out,
        std::map<std::string, float> placeholders):
        graph(graph),
        variables(variables),
        out(out),
        placeholders(std::move(placeholders)),
        results(graph.nodes.size()),
        available(graph.nodes.size()) {}

	~Evaluation();

    /** \brief 读取名为 \p name 的变量
     *
     * \exception VariableMissing 如果变量 \p name 不存在
     */
    float get_variable(const std::string &name) const;

    /** \brief 读取名为 \p name 的占位符
     *
     * \exception PlaceholderMissing 如果占位符 \p name 不存在
     */
    float get_placeholder(const std::string &name) const;

    /** \brief 获取当前输出流
     */
    std::ostream& get_out() const {
        return out;
    }

    /** \brief 计算节点 \p id 或返回其已保存的值
     */
    float compute_node(node_id id);

    /** \brief 使下次求值时的变量改变
     */
    void notify_change(node_id name, float value) const;
};

/** \brief 获取节点输入的包装类
 *
 * 在对每一个节点求值的时候，会有一个针对这个节点的 Inputs 对象传入 NodeInfo::compute 的 \p in 参数。这个参数的主要用法是用 Inputs::operator[] 重载来获取输入节点的值，第一个输入是 \c in[0] ，第二个输入是 \c in[1] ，以此类推。例如，一个加法节点的 NodeInfo::compute 可以实现为。
 *
 * ```cpp
 * float compute(const Inputs &in) const {
 *     return in[0] + in[1];
 * }
 * ```
 *
 * 此外还可以用 \ref get_eval 直接获取求值环境来访问更多信息。
 */
class Inputs {
    const std::vector<node_id> &deps;
    node_id id;
    Evaluation &eval;
public:
    Inputs(Evaluation &eval, node_id id, const std::vector<node_id> &deps):
        deps(deps), id(id), eval(eval) {
        for (node_id x: deps) {
			eval.compute_node(x);
        }
	}

    /** \brief 获取当前求值环境
     */
    Evaluation& get_eval() const {
        return eval;
    }


    /** \brief 获取依赖变量
     */
    const std::vector<node_id> & get_deps() const {
        return deps;
    }

    /** \brief 获取第 \p ix 个输入的值
     */
    float operator[](size_t ix) const;
};

#include "utils.h"

#include <map>
#include <cctype>
#include <iostream>
#include <fstream>
#include <string>

#include "error.h"
#include "graph.h"
#include "node.h"
#include "grad.h"

using namespace std;

static
bool is_symbol(const std::string &str) {
    for (auto x : str)
        if (isalnum(x)) return false;
    return true;
}

void TextGraph::saveSessionTo(const std::string &filename) const
{
	ofstream file;
	file.open(filename);
	if (!file.is_open())
	{
        throw FileOpenError();
	}
	for(auto i=variables.begin();i!=variables.end();i++)
	{
		file << i->first << " " << i->second << std::endl;
	}
	file.close();
}

void TextGraph::loadSessionFrom(const std::string &filename)
{
	ifstream file;
	string name_of_var;
	float value_of_var;
	file.open(filename);
	if (!file.is_open())
	{
        throw FileOpenError();
	}
	variables.clear();
	while(file >> name_of_var >> value_of_var)
	{
		variables[name_of_var]=value_of_var;
	}
	file.close();
}

/** \brief 读入计算图的具体实现
 */
TextGraph read_graph(std::istream &in) {
    TextGraph result;
    std::vector<Node> &gvec = result.graph.nodes;
    std::map<std::string, node_id> &id_map = result.id_map;
    std::map<std::string, float> &variables = result.variables;
    std::map<std::string, Grad> grads;
    int n;
    in >> n;
    for (int c = 0; c < n; c ++) {
        std::string name, type;
        in >> name >> type;
        int id = gvec.size();
        id_map[name] = id;
        if (type == "P") {
            gvec.emplace_back(
                my_make_unique<PlaceholderNode>(name),
                std::vector<node_id> {});
        } else if (type == "V") {
            float val;
            in >> val;
            variables[name] = val;
            gvec.emplace_back(
                my_make_unique<VariableNode>(name),
                std::vector<node_id> {});
        } else if (type == "C") {
            float val;
            in >> val;
            gvec.emplace_back(
                my_make_unique<ConstantNode>(val),
                std::vector<node_id> {});
        }
    }

    int m;
    in >> m;
    for (int q = 0; q < m; q ++) {
        try {
            std::vector<std::string> words;
            int c;
            do {
                std::string w;
                in >> w;
                words.push_back(w);
            } while (c = in.get(), c != EOF && c != '\n');
            // res = ???
            node_id id = gvec.size();

            if (words.size() == 5 && is_symbol(words[3])) {
                // res = x op y
                gvec.emplace_back(
                    my_make_unique<FunctionNode>(words[3]),
                    std::vector<node_id> { id_map[words[2]], id_map[words[4]] });
            } else if (words[2] == "GRAD") {
                // res = GRAD g
                node_id id = id_map[words[3]];
                grads[words[0]] = Grad(result.graph, id);
                continue;
            } else if (words.size() == 5 && words[3] == "AT") {
                // res = g AT n
                id_map[words[0]] = grads.find(words[2])->second.at(id_map[words[4]]);
                continue;
            } else {
                std::vector<node_id> deps;
                for (auto i = words.begin() + 3; i != words.end(); i ++) {
                    deps.push_back(id_map[*i]);
                }

                if (words[2] == "PRINT") {
                    // res = PRINT var
                    gvec.emplace_back(
                        my_make_unique<PrintNode>(words[3]),
                        std::move(deps));
                } else if (words[2] == "COND") {
                    // res = COND i t e
                    gvec.emplace_back(
                        my_make_unique<CondNode>(),
                        std::move(deps));
                } else if (words[2] == "ASSIGN") {
                    // res = COND i t e
                    gvec.emplace_back(
                        my_make_unique<AssignNode>(),
                        std::move(deps));
                } else {
                    // res = F x y z ...
                    gvec.emplace_back(
                        my_make_unique<FunctionNode>(words[2]),
                        std::move(deps));
                }
            }

            id_map[words[0]] = id;
        } catch (ComputeGraphError &ex) {
            std::cout << ex.what() << std::endl;
        } catch (std::exception &ex) {
            std::cout << ex.what() << std::endl;
        } catch (...) {
            throw;
        }
    }
    return std::move(result);
}

/** \brief 读入占位符的具体实现
 */
std::map<std::string, float> read_placeholders(std::istream &in) {
    int k;
    in >> k;
    std::map<std::string, float> placeholders;
    for (int i = 0; i < k; i ++) {
        std::string var;
        float val;
        in >> var >> val;
        placeholders[var] = val;
    }

    return placeholders;
}

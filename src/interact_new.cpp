/** \file
 *
 * \brief 主程序文本交互的实现
 */

#include <iostream>
#include <iomanip>

#include "evaluation.h"
#include "utils.h"
#include "error.h"

/** \brief 主程序文本交互的实现
 *
 * \param in 输入流
 * \param out 输出流
 */
void interact_new(std::istream &in, std::ostream &out) {
    TextGraph gr = read_graph(in);
    int q;
    in >> q;
    std::vector<float> results(q);
    for (int c = 0; c < q; c ++) {
        try {
            std::string command;
            in >> command;
            if (command == "EVAL") {
                std::string var;
                in >> var;
                std::map<std::string, float> pl;
                int ch = in.get();

                if (ch == ' ') {
                    pl = read_placeholders(in);
                } else {
                    // Empty EVAL command
                }

                Evaluation eval (
                    gr.graph,
                    gr.variables,
                    out,
                    std::move(pl)
                );
                float res = eval.compute_node(gr.id_map[var]);
                out
                    << std::setprecision(4) << std::fixed
                    << res << "\n";
                results[c] = res;
            } else if (command == "SETCONSTANT") {
                std::string name;
                float val;
                in >> name >> val;
                gr.variables[name] = val;
            } else if (command == "SETANSWER") {
                std::string name;
                int id;
                in >> name >> id;
                if (id - 1 >= c) {
                    out << "ERROR: Bad answer number\n";
                } else {
                    gr.variables[name] = results[id - 1];
                }
            } else if (command == "LOAD") {
                std::string filename;
                in >> filename;
                gr.loadSessionFrom(filename);
            } else if (command == "SAVE") {
                std::string filename;
                in >> filename;
                gr.saveSessionTo(filename);
            }
        } catch (ComputeGraphError &ex) {
            out << "ERROR: " << ex.what() << std::endl;
        }
    }
}

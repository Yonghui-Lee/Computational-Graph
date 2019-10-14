#include <iostream>
#include "graph.h"
#include "node.h"
#include "evaluation.h"
#include "utils.h"
#include "grad.h"

using std::cin;
using std::cout;
using std::endl;

void doNewtonIteration() {
	Graph g;

	int power;
	cin >> power;

	node_id var = g.createNode(my_make_unique<PlaceholderNode>("x"));
	float coeff;
	cin >> coeff;
	node_id last = g.createNode(my_make_unique<ConstantNode>(coeff));
	for (int i = 0; i < power; i++) {
		float coeff;
		cin >> coeff;
		last = g.createNode(my_make_unique<FunctionNode>("+"), {
			g.createNode(my_make_unique<FunctionNode>("*"), {var, last}),
			g.createNode(my_make_unique<ConstantNode>(coeff))
		});
	}

	node_id derivative = Grad(g, last).at(var);
	float point;
	cin >> point;
	std::map<std::string, float> varValue;

	for (int i = 0; i < 5; i++) {
		std::map<std::string, float> placeholderValue {{"x", point}};
		Evaluation eval(g, varValue, cout, placeholderValue);
		float f = eval.compute_node(last), df = eval.compute_node(derivative);
		point -= f / df;
		placeholderValue["x"] = point;
		cout << point << ' ';
	}
	cout << endl;
}

int main() {
	int n;
	cin >> n;

	while (n-- > 0) {
		doNewtonIteration();
	}

	return 0;
}


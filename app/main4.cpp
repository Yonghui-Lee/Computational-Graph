#include <iostream>
#include <cmath>
#include "graph.h"
#include "node.h"
#include "evaluation.h"
#include "utils.h"
#include "grad.h"

using std::cin;
using std::cout;
using std::endl;


void gradient_descent(){
	int samplenum;
	int variblenum;
	cin>>samplenum>>variblenum;

	std::vector<node_id> var(variblenum);
	Graph g;

	//构造变量Placeholder
	std::map<std::string, float> placeholderValue;
	for(int i=0;i<variblenum;i++)
	{
		var[i] = g.createNode(my_make_unique<PlaceholderNode>("x"+std::to_string(i)));
	    placeholderValue["x"+std::to_string(i)]=1;
		//系数均初始化为1；
	}

	node_id expression = g.createNode(my_make_unique<ConstantNode>(0));


	float  sample;
	for(int i=0;i<samplenum;i++)
	{
		node_id subexpression =g.createNode(my_make_unique<ConstantNode>(0));


		for(int j=0;j<variblenum-1;j++)
		{cin>>sample;
		 subexpression= g.createNode(my_make_unique<FunctionNode>("+"), {
			g.createNode(my_make_unique<FunctionNode>("*"), {var[j], g.createNode(my_make_unique<ConstantNode>(sample))}),
			subexpression } ) ;
		}

		cin>>sample;
		subexpression =  g.createNode(my_make_unique<FunctionNode>("-"), {
			g.createNode(my_make_unique<FunctionNode>("+"), {var[variblenum-1], subexpression}),
			g.createNode(my_make_unique<ConstantNode>(sample))
		 });


		subexpression = g.createNode(my_make_unique<FunctionNode>("*"), {subexpression, subexpression}),

		expression=g.createNode(my_make_unique<FunctionNode>("+"), {expression, subexpression});

	}

	float last_len = 0;
	float target_len = 0, init_len = 0, coeff;
	int times = 2;
	std::vector<node_id> derivative(variblenum);
	std::vector<float> var_grad(variblenum);
	Grad grad(g, expression);
	for(int i=0;i<variblenum;i++)
	{ derivative[i] = grad.at(var[i]);
	}

	while(true)
	{
		std::map<std::string, float> varValue;
		Evaluation eval(g, varValue, cout, placeholderValue);

		float length=0;
		for(int i=0;i<variblenum;i++)
		{var_grad[i] = eval.compute_node(derivative[i]);
			length+=pow(var_grad[i],2);
		}
		length=sqrt(length);

		if (times++ <= 2) {
			init_len = length / 200;
		}
		target_len = init_len / times;
		coeff = target_len / length;
		if (length > last_len) coeff /= 2;
		if(length<0.001)		//终止条件
			break;


		for(int i=0;i<variblenum;i++)
	    {
	    placeholderValue["x"+std::to_string(i)]-=var_grad[i] * coeff;   //步长调整；
	    }


		last_len = length;


	}

	for(int i=0;i<variblenum;i++)
	    {
	    cout<<placeholderValue["x"+std::to_string(i)]<<" ";   //步长调整；
	    }

}

int main(){


	gradient_descent();

	return 0;
}

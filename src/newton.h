#pragma once

#include <iostream>
#include "utils.h"
#include "graph.h"
#include "node.h"
#include "grad.h" 

float newton(std::istream &in)
{
	int n;
	in>>n;
	TextGraph gr = read_new_graph(in,n);
	float x[6];
	float res=0.0;
	in>>x[0];
    for(int i=1;i<=5;i++)
    {
    	gr.variables["x"] = x[i-1];
    	std::map<std::string, float> pl;
    	Evaluation eval (
            gr.graph,
            gr.variables,
            out,
            std::move(pl)
        );
        float res = eval.compute_node(gr.id_map["y"]);
        Grad grad(gr,0);
        float result = grad.at(2*n);
        x[i]=1-res / result;
    }
    res=x[5];
    return res;
}

void solve_newton(std::istream &in, std::ostream &out)
{
	int m;
	in>>m;
	for(int i=0;i<m;i++)
	{
		out<<newton(in)<<std::endl;
	}
	return 0;
}

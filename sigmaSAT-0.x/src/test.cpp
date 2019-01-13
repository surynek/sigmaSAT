/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                             sigmaSAT 0.71-sky                              */
/*                                                                            */
/*                   (C) Copyright 2009-2011 Pavel Surynek                    */
/*            http://www.surynek.com | <pavel.surynek@mff.cuni.cz>            */
/*                                                                            */
/*                                                                            */
/*============================================================================*/
// test.cpp / 0.71-sky
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
// Testing module.

#include <stdio.h>
#include <stdlib.h>

#include "graph.h"


void test_Graph(void)
{
	Graph graph;

	int cN[] = {2, 2, 2, 2};

	graph.init(4, 4, cN);
	graph.print();
	printf("\n");

	graph.add_edge(1, 2, 2);
	graph.print();
	printf("\n");

	graph.add_edge(0, 1, 2);
	graph.add_edge(3, 2, 3);
	graph.add_edge(3, 0, 4);
	graph.print();
	printf("\n");
}


void test_IncrementalGraph(void)
{
	printf("igraph 1...\n");
	sIncrementalGraph igraph;
	igraph.init(NULL, 16, 16, 16);
	igraph.print();
	printf("\n");

	igraph.add_Vertices(1);
	igraph.print();
	printf("\n");

	igraph.add_Vertices(7);
	igraph.print();
	printf("\n");

	igraph.add_Vertices(5);
	igraph.print();
	printf("\n");

	igraph.add_Vertices(3);
	igraph.print();
	printf("\n");

	igraph.add_Vertices(1);
	igraph.print();
	printf("\n");

	printf("igraph 2...\n");
	sIncrementalGraph igraph2;
	igraph2.init(NULL, 4, 4, 4);
	igraph2.print();
	printf("\n");

	igraph2.add_Vertices(7);
	igraph2.print();
	printf("\n");

	igraph2.add_Edge(1, 2, 15);
	igraph2.print();
	printf("\n");

	igraph2.add_Edge(1, 3, 16);
	igraph2.print();
	printf("\n");

	igraph2.add_Edge(1, 5, 17);
	igraph2.print();
	printf("\n");

	igraph2.add_Edge(1, 6, 18);
	igraph2.print();
	printf("\n");

	igraph2.add_Edge(2, 3, 19);
	igraph2.print();
	printf("\n");

	igraph2.add_Edge(2, 4, 20);
	igraph2.print();
	printf("\n");

	igraph2.add_Edge(2, 5, 21);
	igraph2.print();
	printf("\n");

	igraph2.add_Edge(2, 6, 22);
	igraph2.print();
	printf("\n");
}


void test_IncrementalDigraph_1(void)
{
	printf("igraph 2...\n");
	sIncrementalGraph igraph;
	igraph.init(NULL, 16, 16, 16);
	igraph.print_Directed();
	printf("\n");

	igraph.add_Vertices(1);
	igraph.print_Directed();
	printf("\n");

	igraph.add_Vertices(7);
	igraph.print_Directed();
	printf("\n");

	igraph.add_Vertices(5);
	igraph.print_Directed();
	printf("\n");

	igraph.add_Vertices(3);
	igraph.print_Directed();
	printf("\n");

	igraph.add_Vertices(1);
	igraph.print_Directed();
	printf("\n");

	printf("igraph 2...\n");
	sIncrementalGraph igraph2;
	igraph2.init(NULL, 4, 4, 4);
	igraph2.print_Directed();
	printf("\n");

	igraph2.add_Vertices(7);
	igraph2.print_Directed();
	printf("\n");

	igraph2.add_DirectedEdge(0, 2, 13);
	igraph2.add_DirectedEdge(1, 0, 12);
	igraph2.add_DirectedEdge(1, 2, 15);
	igraph2.print_Directed();
	printf("\n");

	igraph2.add_DirectedEdge(1, 3, 16);
	igraph2.add_DirectedEdge(3, 1, 13);
	igraph2.print_Directed();
	printf("\n");

	igraph2.add_DirectedEdge(1, 5, 17);
	igraph2.print_Directed();
	printf("\n");

	igraph2.add_DirectedEdge(1, 6, 18);
	igraph2.add_DirectedEdge(6, 1, 17);
	igraph2.print_Directed();
	printf("\n");

	igraph2.add_DirectedEdge(2, 3, 19);
	igraph2.print_Directed();
	printf("\n");

	igraph2.add_DirectedEdge(2, 4, 20);
	igraph2.add_DirectedEdge(4, 2, 23);
	igraph2.print_Directed();
	printf("\n");

	igraph2.add_DirectedEdge(2, 5, 21);
	igraph2.print_Directed();
	printf("\n");

	igraph2.add_DirectedEdge(2, 6, 22);
	igraph2.add_DirectedEdge(6, 2, 11);
	igraph2.print_Directed();
	printf("\n");

	sVertex *source = igraph2.get_Vertex(1);
	sVertex *sink = igraph2.get_Vertex(6);

	int result = igraph2.compute_MaximumFlow(source, sink);
	if (result < 0)
	{
		printf("Cannot compute maximum flow (result:%d).\n", result);
		exit(-1);
	}
	igraph2.print_DirectedFlow();
}


void test_IncrementalDigraph_2(void)
{
	printf("igraph 3...\n");
	sIncrementalGraph igraph3;
	igraph3.init(NULL, 4, 4, 4);
	igraph3.print_Directed();
	printf("\n");

	igraph3.add_Vertices(4);

	igraph3.print_Directed();
	printf("\n");

	igraph3.add_DirectedEdge(0, 1, 100);
	igraph3.add_DirectedEdge(0, 2, 100);

	igraph3.add_DirectedEdge(1, 3, 50);
	igraph3.add_DirectedEdge(2, 3, 150);

	igraph3.add_DirectedEdge(1, 2, 40);
	igraph3.add_DirectedEdge(2, 1, 40);

	igraph3.print_Directed();
	printf("\n");

	sVertex *source = igraph3.get_Vertex(0);
	sVertex *sink = igraph3.get_Vertex(3);

	int result = igraph3.compute_MaximumFlow(source, sink);
	if (result < 0)
	{
		printf("Cannot compute maximum flow (result:%d).\n", result);
		exit(-1);
	}
	igraph3.print_DirectedFlow();
}


int main(void)
{
//	test_Graph();
//	test_IncrementalGraph();
	test_IncrementalDigraph_1();
	test_IncrementalDigraph_2();

	return 0;
}

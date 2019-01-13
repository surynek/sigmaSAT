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
// graph.h / 0.71-sky
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
// Graph representation.


#ifndef __GRAPH_H__
#define __GRAPH_H__

#include <set>
#include <list>


class Vertex;
class sVertex;
class sLiteral;
class sClause;
class sBooleanFormula;
class VMatrix;
class sCluster;
class Clustering;
class Layering;


class VMatrix
{
public:
	int init(int L, int max_visits);
	void destroy(void);

	void reset(void);
	void print(void);

public:
	int m_L, m_max_visits;
	int m_total_paths;
	int **m_cPaths;
};


class Edge
{
public:
	int m_weight;
};


class Neighbor
{
public:
	Edge *m_edge;
	Vertex *m_vertex;
};


class Vertex
{
public:
	int m_N, m_cN;
	Neighbor *m_neighbors;
};


class Graph
{
public:
	int init(int V, int E, int *cN);
	void destroy(void);

	void add_edge(int u, int v, int weight = 0);
	void print(void) const;

public:
	int m_V;
	Vertex *m_vertices;

	int m_E, m_cE;
	Edge *m_edges;

	int m_N, m_cN;
	Neighbor *m_neighbors;
};


class sEdge
{
public:
	int m_weight;
	int m_flow;
	int m_capacity;

	bool m_to_push;
	bool m_active;
};


class sNeighborNode
{
public:
	sEdge *m_edge;
	sVertex *m_vertex;
	sNeighborNode *m_next;
};


typedef std::set<int> Neighbors_set;

class sVertex
{
public:
	int m_idx;
	sLiteral *m_literal;

	int m_N_Neighs;
	sNeighborNode *m_first_nnode;
	sNeighborNode **m_last_nnode;
	Neighbors_set m_Neighbors;

	int m_N_Ins;
	sNeighborNode *m_first_in_node;
	sNeighborNode **m_last_in_node;

	int m_N_Outs;
	sNeighborNode *m_first_out_node;
	sNeighborNode **m_last_out_node;

	int m_component;

// path search specific members
	int m_cluster;
	int m_layer;

	int m_N_Layers, m_Layer_cnt;
	int *m_Layers;

	int m_distance;
	VMatrix m_vmatrix;
	int m_max_visits;

	int m_active_N_Neighs;
	int m_time_stamp;

// flow specific members
	int m_capacity;
	int m_score;
	sClause *m_clause;
	int m_heap_idx;

	int m_height;
	int m_excess;

	bool m_to_relabel;
};


class sVertexBlock
{
public:
	sVertex *m_vertices;
	sVertexBlock *m_next;
};


class sEdgeBlock
{
public:
	sEdge *m_edges;
	sEdgeBlock *m_next;
};


class sNeighborBlock
{
public:
	sNeighborNode *m_nnodes;
	sNeighborBlock *m_next;
};


class sClusterBlock
{
public:
	sCluster *m_clusters;
	sClusterBlock *m_next;
};


class VONode
{
public:
	sVertex *m_vertex;
	VONode *m_next;	
};


class sVertexQueue
{
public:
	sVertexQueue();
	~sVertexQueue();       

	int init(int N_Vertices);
	void destroy(void);

	sVertex* get_Front(void) const;
	int get_VertexCount(void) const;
	bool is_Empty() const;

	void push_Back(sVertex *vertex);
	sVertex* pop_Front(void);

	void print(void) const;
	
public:
	int m_N_Vertices;
	sVertex **m_Vertices;
	sVertex **m_boundary;

	int m_Vertex_cnt;
	sVertex **m_begin;
	sVertex **m_end;
};


class sEdgeQueue
{
public:
	typedef struct
	{
		sVertex *m_u, *m_v;
		sEdge *m_edge;
	} Edge;

	sEdgeQueue();
	~sEdgeQueue();       

	int init(int N_Edges);
	void destroy(void);

	Edge* get_Front(void) const;
	int get_EdgeCount(void) const;
	bool is_Empty() const;

	void push_Back(sVertex *u, sVertex *v, sEdge *edge);
	Edge* pop_Front(void);
	
public:
	int m_N_Edges;
	Edge *m_Edges;
	Edge *m_boundary;

	int m_Edge_cnt;
	Edge *m_begin;
	Edge *m_end;
};


/*----------------------------------------------------------------------------*/
// sIncrementalGraph class

class sIncrementalGraph
{
public:
	int init(sBooleanFormula *formula, int V_block, int E_block, int N_block);
	void destroy(void);

	int add_Vertices(int nVertices = 1);

	int add_Edge(int u_idx, int v_idx, int weight = 0);
	int add_UndirectedEdge(int u_idx, int v_idx, int weight = 0);
	int add_DirectedEdge(int u_idx, int v_idx, int capacity = 0);

	int increase_EdgeWeight(int u_idx, int v_idx, int dweight = 1);

	int get_VertexCount(void) const;
	sVertex *get_Vertex(int idx) const;

	int get_EdgeCount(void) const;
        sEdge *get_Edge(int u_idx, int v_idx) const;
	bool is_Adjacent(int u_idx, int v_idx) const;

	sEdge *get_UndirectedEdge(int u_idx, int v_idx) const;
	sEdge *get_DirectedEdge(int u_idx, int v_idx) const;

	int calc_ActiveNeighbors(sVertex *vertex);

	int calc_ComponentSize(sVertex *vertex, int stamp);
	int clusterize_Component(sCluster *cluster, int idx, sVertex *vertex, int stamp);

	int calc_DenseComponentSize(sVertex *vertex, int stamp);
	int clusterize_DenseComponent(sCluster *cluster, int idx, sVertex *vertex, int stamp);

	int calc_CliqueSize(sVertex *vertex, int stamp);
	int clusterize_Clique(sCluster *cluster, int idx, sVertex *vertex, int stamp);

	int build_CoveringLayers(Clustering *clustering, Layering *layering);
	int build_NextCoveringLayer(Clustering *clustering, Layering *layering, sVertex **buffer);

	int compute_MaximumFlow(sVertex *source, sVertex *sink);
	void init_Preflow(sVertex *source, sVertex *sink, sVertexQueue *relabel_queue);
	void push_in_Preflow(sVertex *source, sVertex *sink, sVertex *u, sVertex *v, sEdge *edge, sVertexQueue *relabel_queue, sEdgeQueue *push_queue_in, sEdgeQueue *push_queue_out);
	void push_out_Preflow(sVertex *source, sVertex *sink, sVertex *u, sVertex *v, sEdge *edge, sVertexQueue *relabel_queue, sEdgeQueue *push_queue_in, sEdgeQueue *push_queue_out);
	void relabel_Preflow(sVertex *source, sVertex *sink, sVertex *u, sEdgeQueue *push_queue_in, sEdgeQueue *push_queue_out);

	void print(void) const;
	void print_Undirected(void) const;
	void print_Directed(void) const;
	void print_DirectedFlow(void) const;
	void print_CoveringLayers(void) const;

	void print_flow_specific(void) const;
	void print_flow_specific_nontrivial(void) const;
	void export_GR(bool show_literals = false, bool exp_clusters = false, bool exp_layers = false, bool exp_visits = false) const;

	int calc_ClusterNeighbors(sVertex *vertex) const;
	int calc_VertexScore(sVertex *vertex) const;
	int calc_VertexScore_2(sVertex *vertex) const;

	void get_label(char *label, sVertex *vertex, bool exp_literals = false) const;
	void get_data(char *data, sVertex *vertex, bool exp_clusters = false, bool exp_layers = false, bool exp_visits = false) const;

private:
	int alloc_vertices(int cV);
	int alloc_edge(sEdge **edge);
	int alloc_neighbor(sNeighborNode **nnode);

public:
	sBooleanFormula *m_formula;
	int m_last_idx;

	int m_V, m_cV, m_ccV;
	int m_V_block;
	sVertexBlock *m_first_vblock;
	sVertexBlock **m_last_vblock;

	int m_E, m_cE, m_ccE;
	int m_E_block;
	sEdgeBlock *m_first_eblock;
	sEdgeBlock **m_last_eblock;

	int m_N, m_cN, m_ccN;
	int m_N_block;
	sNeighborBlock *m_first_nblock;
	sNeighborBlock **m_last_nblock;

	int m_cVO;
	VONode *m_vo_nodes;
	int m_time_stamp;
};


class Cluster
{
public:
	int m_cV;
	sVertex **m_vertices;
	sVertex *m_path_vertex;

	int m_capacity;
};


class Clustering
{
public:
	int init(const sBooleanFormula *formula, sClause **path, int length, sIncrementalGraph *graph);
	void destroy(void);

	void print(void) const;
	void print_distances(void) const;
	void print_visits(void) const;
	void print_visit_matrices(void) const;

public:
	const sBooleanFormula *m_formula;
	sIncrementalGraph *m_graph;

	int m_cC;
	Cluster *m_clusters;
};


class sCluster
{
public:
	int m_idx;

	int m_N_Vertices;
	sVertex **m_Vertices;
	sVertex *m_path_vertex;

	int m_capacity;
	int m_total_capacity;
};


class sPtr_VertexBlock
{
public:
	sVertex **m_Vertices;
	sPtr_VertexBlock *m_next;
};


class sIncrementalClustering
{
public:
	int init(const sBooleanFormula *formula, sIncrementalGraph *graph, int C_block, int V_block);
	void destroy(void);

	int get_ClusterCount(void) const;
	sCluster* get_cluster(int idx);
	int add_cluster(int N_Vertices);

	void print(void) const;
	void print_nontrivial(void) const;
	void print_cluster(const sCluster *cluster) const;

private:
	int alloc_cluster(sCluster **cluster);
	int alloc_cluster_vertices(int nVertices, sVertex ***Vertices);

public:
	const sBooleanFormula *m_formula;
	sIncrementalGraph *m_graph;

	int m_last_idx;

	int m_C, m_cC, m_ccC;
	int m_C_block;
	sClusterBlock *m_first_cblock;
	sClusterBlock **m_last_cblock;

	int m_V, m_cV, m_ccV;
	int m_V_block;
	sPtr_VertexBlock *m_first_vblock;
	sPtr_VertexBlock **m_last_vblock;
};


class Layer
{
public:
	int m_V, m_cV;
	sVertex **m_vertices;
	int m_path_selects;
	bool m_checked;
};


class Layering
{
public:
	int init(const sBooleanFormula *formula, sIncrementalGraph *graph);
	int init(const sBooleanFormula *formula, sClause **path, int length, sIncrementalGraph *graph);
	int init_visit_matrices(int L, int max_visits);
	void destroy(void);
	void destroy_visit_matrices(void);

	void reset(void);

	void print(void) const;
	void print_visits(void) const;
public:
	const sBooleanFormula *m_formula;
	sIncrementalGraph *m_graph;

	int m_L, m_cL;
	Layer *m_Layers;
	
	int m_N_Indices, m_Index_cnt;
	int *m_layer_Indices;
	int m_N_Vertices, m_Vertex_cnt;
	sVertex **m_layer_Vertices;
};


class sVertexHeap
{
public:
	int init(int N_Vertices);
	void destroy(void);
	int get_VertexCount(void);

	void insert_Vertex(sVertex *vertex);
	void remove_Vertex(sVertex *vertex);
	
	sVertex* get_Min(void) const;
	void remove_Min(void);
	void change_Score(sVertex *vertex, int score);

	void to_Screen(void) const;

private:
	void maintain_OrderingIncreased(int idx);
	void maintain_OrderingDecreased(int idx);

public:
	int m_N_Vertices, m_Vertex_cnt;
	sVertex **m_Vertices;
};


/*----------------------------------------------------------------------------*/
// Global functions


#endif

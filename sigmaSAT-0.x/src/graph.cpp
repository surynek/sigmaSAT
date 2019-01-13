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
// graph.cpp / 0.71-sky
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
//  Graph representation.


#include <stdlib.h>

#include "config.h"
#include "defs.h"
#include "system.h"
#include "version.h"
#include "graph.h"


/*----------------------------------------------------------------------------*/
// Graph

int Graph::init(int V, int E, int *cN)
{
	m_V = V;

	m_vertices = (Vertex*)malloc(V * sizeof(Vertex));
	if (m_vertices == NULL)
	{
		return -1;
	}
        assert(m_vertices != NULL);

	m_E = E;
	m_cE = 0;

	m_edges = (Edge*)malloc(E * sizeof(Edge));
	if (m_edges == NULL)
	{
		return -1;
	}
	assert(m_edges != NULL);

	m_N = 2 * E;
	m_cN = 0;
	m_neighbors = (Neighbor*)malloc(m_N * sizeof(Neighbor));
	if (m_neighbors == NULL)
	{
		return -1;
	}
	assert(m_neighbors != NULL);

	Neighbor *last_neighbor = m_neighbors;

	for (int i = 0; i < V; ++i)
	{
		m_vertices[i].m_cN = 0;
		m_vertices[i].m_N = cN[i];
		m_vertices[i].m_neighbors = last_neighbor;
		last_neighbor += cN[i];
	}

	return 0;
}


void Graph::destroy(void)
{
	free(m_vertices);
	free(m_edges);
	free(m_neighbors);
}


void Graph::add_edge(int u, int v, int weight)
{
	Vertex *vertex_u = &m_vertices[u];
	Vertex *vertex_v = &m_vertices[v];

	Neighbor *neighbor = &vertex_u->m_neighbors[vertex_u->m_cN++];
	assert(vertex_u->m_cN <= vertex_u->m_N);
	Edge *edge = neighbor->m_edge = &m_edges[m_cE++];
	edge->m_weight = weight;
	neighbor->m_vertex = vertex_v;

	neighbor = &vertex_v->m_neighbors[vertex_v->m_cN++];
	assert(vertex_v->m_cN <= vertex_v->m_N);
	neighbor->m_edge = edge;
	neighbor->m_vertex = vertex_u;
}


void Graph::print(void) const
{
	for (int i = 0; i < m_V; ++i)
	{
		printf("%d: {", i);

		Vertex *vertex = &m_vertices[i];

		Neighbor *neighbor = vertex->m_neighbors;
		for (int j = 0; j < vertex->m_cN - 1; ++j)
		{
			printf("%ld(%d),", neighbor->m_vertex - m_vertices, neighbor->m_edge->m_weight);
			++neighbor;
		}
		if (vertex->m_cN > 0)
		{
			printf("%ld(%d)", neighbor->m_vertex - m_vertices, neighbor->m_edge->m_weight);
		}

		printf("}\n");
	}
}


/*----------------------------------------------------------------------------*/
// sVertexQueue

sVertexQueue::sVertexQueue()
{
	m_Vertices = NULL;
}


sVertexQueue::~sVertexQueue()
{
	if (m_Vertices != NULL)
	{
		destroy();
	}
}


int sVertexQueue::init(int N_Vertices)
{
	m_Vertices = (sVertex**)malloc(N_Vertices * sizeof(sVertex*));

	if (m_Vertices == NULL)
	{
		return -1;
	}
	assert(m_Vertices != NULL);
	m_N_Vertices = N_Vertices;
	m_Vertex_cnt = 0;

	m_begin = m_end = m_Vertices;
	m_boundary = m_Vertices + N_Vertices; 
	return 0;
}


void sVertexQueue::destroy(void)
{
	free(m_Vertices);
}


sVertex* sVertexQueue::get_Front(void) const
{
	assert(!is_Empty());
	return *m_begin;
}


int sVertexQueue::get_VertexCount(void) const
{
	return m_Vertex_cnt;
}


bool sVertexQueue::is_Empty() const
{
	return (m_Vertex_cnt <= 0);
}


void sVertexQueue::push_Back(sVertex *vertex)
{
	*m_end++ = vertex;
	if (m_end == m_boundary)
	{
		m_end = m_Vertices;
	}
	++m_Vertex_cnt;
}


sVertex* sVertexQueue::pop_Front(void)
{
	sVertex *front = *m_begin++;
	if (m_begin == m_boundary)
	{
		m_begin = m_Vertices;
	}
	--m_Vertex_cnt;

	return front;
}


void sVertexQueue::print(void) const
{
	printf("Vertex queue:");

	sVertex **vertex = m_begin;
	while (vertex != m_end)
	{
		printf("%d ", (*vertex)->m_idx);
		++vertex;
		if (vertex == m_boundary)
		{
			vertex = m_Vertices;
		}
	}
	printf("\n");
}


/*----------------------------------------------------------------------------*/
// sEdgeQueue

sEdgeQueue::sEdgeQueue()
{
	m_Edges = NULL;
}


sEdgeQueue::~sEdgeQueue()
{
	if (m_Edges != NULL)
	{
//		destroy();
	}
}


int sEdgeQueue::init(int N_Edges)
{
	m_Edges = (Edge*)malloc(N_Edges * sizeof(Edge));
	if (m_Edges == NULL)
	{
		return -1;
	}
	assert(m_Edges != NULL);
	m_N_Edges = N_Edges;
	m_Edge_cnt = 0;

	m_begin = m_end = m_Edges;
	m_boundary = m_Edges + N_Edges; 
	return 0;
}


void sEdgeQueue::destroy(void)
{
	free(m_Edges);
}


sEdgeQueue::Edge* sEdgeQueue::get_Front(void) const
{
	assert(!is_Empty());
	return m_begin;
}


int sEdgeQueue::get_EdgeCount(void) const
{
	return m_Edge_cnt;
}


bool sEdgeQueue::is_Empty() const
{
	return (m_Edge_cnt <= 0);
}


void sEdgeQueue::push_Back(sVertex *u, sVertex *v, sEdge *edge)
{
	m_end->m_u = u;
	m_end->m_v = v;
	m_end->m_edge = edge;
	++m_end;

	if (m_end == m_boundary)
	{
		m_end = m_Edges;
	}
	++m_Edge_cnt;
}


sEdgeQueue::Edge* sEdgeQueue::pop_Front(void)
{
	Edge *front = m_begin++;
	if (m_begin == m_boundary)
	{
		m_begin = m_Edges;
	}
	--m_Edge_cnt;

	return front;
}


/*----------------------------------------------------------------------------*/
// sIncrementalGraph class

int sIncrementalGraph::init(sBooleanFormula *formula, int V_block, int E_block, int N_block)
{
	m_formula = formula;

	m_last_idx = 0;

	m_E = m_N = 0;
	m_cV = m_cE = m_cN = 0;
	m_ccV = m_ccE = m_ccN = 0;

	m_V_block = V_block;
	m_E_block = E_block;
	m_N_block = N_block;

	m_first_vblock = (sVertexBlock*)malloc(sizeof(sVertexBlock));
	if (m_first_vblock == NULL)
	{
		return -1;
	}
	assert(m_first_vblock != NULL);
	
	m_first_vblock->m_vertices = new sVertex[V_block];
	if (m_first_vblock->m_vertices == NULL)
	{
		return -1;
	}
	assert(m_first_vblock->m_vertices != NULL);

	m_first_vblock->m_next = NULL;
	m_V = m_V_block;

	m_first_eblock = (sEdgeBlock*)malloc(sizeof(sEdgeBlock));
	if (m_first_eblock == NULL)
	{
		return -1;
	}
	assert(m_first_eblock != NULL);

	m_first_eblock->m_edges = (sEdge*)malloc(E_block * sizeof(sEdge));
	if (m_first_eblock->m_edges == NULL)
	{
		return -1;
	}
	assert(m_first_eblock->m_edges != NULL);
	m_first_eblock->m_next = NULL;
	m_E = m_E_block;

	m_first_nblock = (sNeighborBlock*)malloc(sizeof(sNeighborBlock));
	if (m_first_nblock == NULL)
	{
		return -1;
	}
	assert(m_first_nblock != NULL);

	m_first_nblock->m_nnodes = (sNeighborNode*)malloc(N_block * sizeof(sNeighborNode));
	if (m_first_nblock->m_nnodes == NULL)
	{
		return -1;
	}
	assert(m_first_nblock->m_nnodes != NULL);
	m_first_nblock->m_next = NULL;
	m_N = m_N_block;

	m_last_vblock = &m_first_vblock;
	m_last_eblock = &m_first_eblock;
	m_last_nblock = &m_first_nblock;

	m_cVO = 0;

	m_vo_nodes = (VONode*)malloc(V_block * sizeof(VONode));
	if (m_vo_nodes == NULL)
	{
		return -1;
	}
	assert(m_vo_nodes != NULL);
	m_time_stamp = 0;

	return 0;
}


void sIncrementalGraph::destroy(void)
{
	sVertexBlock *vblock_delete;
	sVertexBlock *vblock = m_first_vblock;

	while (vblock != NULL)
	{
		delete [] vblock->m_vertices;
		vblock_delete = vblock;
		vblock = vblock->m_next;

		free(vblock_delete);
	}

	sEdgeBlock *eblock_delete;
	sEdgeBlock *eblock = m_first_eblock;

	while (eblock != NULL)
	{
		free(eblock->m_edges);
		eblock_delete = eblock;
		eblock = eblock->m_next;
		free(eblock_delete);
	}

	sNeighborBlock *nblock_delete;
	sNeighborBlock *nblock = m_first_nblock;

	while (nblock != NULL)
	{
		free(nblock->m_nnodes);
		nblock_delete = nblock;
		nblock = nblock->m_next;

		free(nblock_delete);
	}

	free(m_vo_nodes);
}


int sIncrementalGraph::add_Vertices(int cV)
{
	int result;

	sVertexBlock *last_vblock = *m_last_vblock;

	if ((result = alloc_vertices(cV)) < 0)
	{
		return result;
	}

	for (sVertexBlock *vblock = last_vblock; vblock != NULL; vblock = vblock->m_next)
	{
		int fill_V = m_V_block - m_ccV;
		if (cV > fill_V)
		{
			while (m_ccV < m_V_block)
			{
				sVertex *vertex = &vblock->m_vertices[m_ccV++];
				vertex->m_idx = m_last_idx++;

				vertex->m_N_Neighs = 0;
				vertex->m_first_nnode = NULL;
				vertex->m_last_nnode = &vertex->m_first_nnode;

				vertex->m_N_Ins = 0;
				vertex->m_first_in_node = NULL;
				vertex->m_last_in_node = &vertex->m_first_in_node;

				vertex->m_N_Outs = 0;
				vertex->m_first_out_node = NULL;
				vertex->m_last_out_node = &vertex->m_first_out_node;

				vertex->m_component = 0;
			}
			cV -= fill_V;
			m_cV += fill_V;
			m_ccV = 0;
		}
		else
		{
			m_cV += cV;

			while (cV > 0)
			{
				sVertex *vertex = &vblock->m_vertices[m_ccV++];
				vertex->m_idx = m_last_idx++;

				vertex->m_N_Neighs = 0;
				vertex->m_first_nnode = NULL;
				vertex->m_last_nnode = &vertex->m_first_nnode;

				vertex->m_N_Ins = 0;
				vertex->m_first_in_node = NULL;
				vertex->m_last_in_node = &vertex->m_first_in_node;

				vertex->m_N_Outs = 0;
				vertex->m_first_out_node = NULL;
				vertex->m_last_out_node = &vertex->m_first_out_node;

				vertex->m_component = 0;
				--cV;
			}
		}
	}

	return 0;
}


int sIncrementalGraph::add_Edge(int u_idx, int v_idx, int weight)
{
	return add_UndirectedEdge(u_idx, v_idx, weight);
}


int sIncrementalGraph::add_UndirectedEdge(int u_idx, int v_idx, int weight)
{
	int result;

	sEdge *edge;
	sNeighborNode *nnode_u, *nnode_v;

	if ((result = alloc_edge(&edge)) < 0)
	{
		return result;
	}
	if ((result = alloc_neighbor(&nnode_u)) < 0)
	{
		return result;
	}
	if ((result = alloc_neighbor(&nnode_v)) < 0)
	{
		return result;
	}

	sVertex *vertex_u = get_Vertex(u_idx);
	sVertex *vertex_v = get_Vertex(v_idx);

	vertex_u->m_Neighbors.insert(v_idx);
	vertex_v->m_Neighbors.insert(u_idx);

	edge->m_weight = weight;

	nnode_u->m_edge = edge;
	nnode_u->m_vertex = vertex_v;
	nnode_u->m_next = NULL;

	*vertex_u->m_last_nnode = nnode_u;
	vertex_u->m_last_nnode = &nnode_u->m_next;
	++vertex_u->m_N_Neighs;

	nnode_v->m_edge = edge;
	nnode_v->m_vertex = vertex_u;
	nnode_v->m_next = NULL;

	*vertex_v->m_last_nnode = nnode_v;
	vertex_v->m_last_nnode = &nnode_v->m_next;
	++vertex_v->m_N_Neighs;

	return 0;
}


int sIncrementalGraph::add_DirectedEdge(int u_idx, int v_idx, int capacity)
{	
	int result;

	sEdge *edge;
	sNeighborNode *nnode_u, *nnode_v;

	if ((result = alloc_edge(&edge)) < 0)
	{
		return result;
	}
	if ((result = alloc_neighbor(&nnode_u)) < 0)
	{
		return result;
	}
	if ((result = alloc_neighbor(&nnode_v)) < 0)
	{
		return result;
	}

	sVertex *vertex_u = get_Vertex(u_idx);
	sVertex *vertex_v = get_Vertex(v_idx);

	edge->m_capacity = capacity;

	nnode_u->m_edge = edge;
	nnode_u->m_vertex = vertex_v;
	nnode_u->m_next = NULL;

	*vertex_u->m_last_out_node = nnode_u;
	vertex_u->m_last_out_node = &nnode_u->m_next;
	++vertex_u->m_N_Outs;

	nnode_v->m_edge = edge;
	nnode_v->m_vertex = vertex_u;
	nnode_v->m_next = NULL;

	*vertex_v->m_last_in_node = nnode_v;
	vertex_v->m_last_in_node = &nnode_v->m_next;
	++vertex_v->m_N_Ins;

	return 0;
}


int sIncrementalGraph::increase_EdgeWeight(int u_idx, int v_idx, int dweight)
{
	int result;

	sEdge *edge = get_Edge(u_idx, v_idx);

	if (edge != NULL)
	{
		edge->m_weight += dweight;
	}
	else
	{
		if ((result = add_Edge(u_idx, v_idx, dweight)) < 0)
		{
			return result;
		}
	}

	return 0;
}


int sIncrementalGraph::get_VertexCount(void) const
{
	return m_cV;
}


sVertex* sIncrementalGraph::get_Vertex(int idx) const
{
	sVertexBlock *vblock = m_first_vblock;

	while (idx >= m_V_block)
	{
		vblock = vblock->m_next;
		idx -= m_V_block;
	}

	return &vblock->m_vertices[idx];
}


int sIncrementalGraph::get_EdgeCount(void) const
{
	return m_cE;
}


sEdge* sIncrementalGraph::get_Edge(int u_idx, int v_idx) const
{
	return get_UndirectedEdge(u_idx, v_idx);
}


sEdge* sIncrementalGraph::get_UndirectedEdge(int u_idx, int v_idx) const
{
	sVertex *vertex_u = get_Vertex(u_idx);
	sVertex *vertex_v = get_Vertex(v_idx);

	if (vertex_u->m_N_Neighs > vertex_v->m_N_Neighs)
	{
		sNeighborNode *nnode = vertex_v->m_first_nnode;

		while (nnode != NULL)
		{
			if (nnode->m_vertex->m_idx == u_idx)
			{
				return nnode->m_edge;
			}
			nnode = nnode->m_next;
		}
		return NULL;
	}
	else
	{
		sNeighborNode *nnode = vertex_u->m_first_nnode;

		while (nnode != NULL)
		{
			if (nnode->m_vertex->m_idx == v_idx)
			{
				return nnode->m_edge;
			}
			nnode = nnode->m_next;
		}
		return NULL;
	}
}


bool sIncrementalGraph::is_Adjacent(int u_idx, int v_idx) const
{
	sVertex *vertex_u = get_Vertex(u_idx);
	sVertex *vertex_v = get_Vertex(v_idx);

	if (vertex_u->m_N_Neighs > vertex_v->m_N_Neighs)
	{
		if (vertex_v->m_N_Neighs > sSET_USAGE_LIMIT)
		{
			const Neighbors_set &neighbors = vertex_v->m_Neighbors;

			if (neighbors.find(u_idx) != neighbors.end())
			{
				return true;
			}
			return false;
		}
		else
		{
			sNeighborNode *nnode = vertex_v->m_first_nnode;

			while (nnode != NULL)
			{
				if (nnode->m_vertex->m_idx == u_idx)
				{
					return true;
				}
				nnode = nnode->m_next;
			}
			return false;
		}
	}
	else
	{
		if (vertex_u->m_N_Neighs > sSET_USAGE_LIMIT)
		{
			const Neighbors_set &neighbors = vertex_u->m_Neighbors;
			if (neighbors.find(v_idx) != neighbors.end())
			{
				return true;
			}
			return false;
		}
		else
		{
			sNeighborNode *nnode = vertex_u->m_first_nnode;

			while (nnode != NULL)
			{
				if (nnode->m_vertex->m_idx == v_idx)
				{
					return true;
				}
				nnode = nnode->m_next;
			}
			return false;
		}
	}
}


sEdge* sIncrementalGraph::get_DirectedEdge(int u_idx, int v_idx) const
{
	sVertex *vertex_u = get_Vertex(u_idx);
	sVertex *vertex_v = get_Vertex(v_idx);

	if (vertex_u->m_N_Outs > vertex_v->m_N_Ins)
	{
		sNeighborNode *nnode = vertex_v->m_first_in_node;

		while (nnode != NULL)
		{
			if (nnode->m_vertex->m_idx == u_idx)
			{
				return nnode->m_edge;
			}
			nnode = nnode->m_next;
		}
		return NULL;
	}
	else
	{
		sNeighborNode *nnode = vertex_u->m_first_out_node;

		while (nnode != NULL)
		{
			if (nnode->m_vertex->m_idx == v_idx)
			{
				return nnode->m_edge;
			}
			nnode = nnode->m_next;
		}
		return NULL;
	}
}


int sIncrementalGraph::calc_ActiveNeighbors(sVertex *vertex)
{
	if (vertex->m_time_stamp >= m_time_stamp)
	{
		return vertex->m_active_N_Neighs;
	}
	else
	{
		vertex->m_active_N_Neighs = 0;

		sNeighborNode *nnode = vertex->m_first_nnode;

		while (nnode != NULL)
		{
			if (nnode->m_edge->m_active)
			{
				++vertex->m_active_N_Neighs;
			}
			nnode = nnode->m_next;
		}
		vertex->m_time_stamp = m_time_stamp;
		
		return vertex->m_active_N_Neighs;
	}
}


int sIncrementalGraph::calc_ComponentSize(sVertex *vertex, int stamp)
{
	int component_size = 0;
	vertex->m_component = stamp;

	sNeighborNode *nnode = vertex->m_first_nnode;
	while (nnode != NULL)
	{
		if (nnode->m_vertex->m_component != stamp && nnode->m_vertex->m_cluster < 0)
		{
			component_size += calc_ComponentSize(nnode->m_vertex, stamp);
		}
		nnode = nnode->m_next;
	}

	return component_size + 1;
}


int sIncrementalGraph::clusterize_Component(sCluster *cluster, int idx, sVertex *vertex, int stamp)
{
	int component_size = 0;
	vertex->m_component = stamp;
	cluster->m_Vertices[idx] = vertex;
	++idx;
	vertex->m_cluster = cluster->m_idx;

	sNeighborNode *nnode = vertex->m_first_nnode;
	while (nnode != NULL)
	{
		if (nnode->m_vertex->m_component != stamp && nnode->m_vertex->m_cluster < 0)
		{
			int sub_component_size = clusterize_Component(cluster, idx, nnode->m_vertex, stamp);
			component_size += sub_component_size;
			idx += sub_component_size;
		}
		nnode = nnode->m_next;
	}
	return component_size + 1;
}


int sIncrementalGraph::calc_DenseComponentSize(sVertex *vertex, int stamp)
{
	int component_size = 0;
	vertex->m_component = stamp;

	sNeighborNode *nnode = vertex->m_first_nnode;
	while (nnode != NULL)
	{
		if (nnode->m_vertex->m_component != stamp && nnode->m_vertex->m_cluster < 0 && sDFR(nnode->m_vertex->m_N_Neighs, vertex->m_N_Neighs) <= 1)
		{
			++component_size;
		}
		nnode = nnode->m_next;
	}

	return component_size + 1;
}


int sIncrementalGraph::clusterize_DenseComponent(sCluster *cluster, int idx, sVertex *vertex, int stamp)
{
	int component_size = 0;
	vertex->m_component = stamp;
	cluster->m_Vertices[idx] = vertex;
	++idx;
	vertex->m_cluster = cluster->m_idx;

	sNeighborNode *nnode = vertex->m_first_nnode;
	while (nnode != NULL)
	{
		if (nnode->m_vertex->m_component != stamp && nnode->m_vertex->m_cluster < 0 && sDFR(nnode->m_vertex->m_N_Neighs, vertex->m_N_Neighs) <= 1)
		{
			cluster->m_Vertices[idx++] = nnode->m_vertex;
			nnode->m_vertex->m_cluster = cluster->m_idx;
			nnode->m_vertex->m_component = stamp;
			++component_size;
		}
		nnode = nnode->m_next;
	}
	return component_size + 1;
}


int sIncrementalGraph::calc_CliqueSize(sVertex *vertex, int stamp)
{
	int clique_size = 0;

	vertex->m_component = stamp;
	sNeighborNode *nnode_1 = vertex->m_first_nnode;
	while (nnode_1 != NULL)
	{
		if (nnode_1->m_vertex->m_cluster < 0)
		{
			bool candidate = true;
			sNeighborNode *nnode_2 = vertex->m_first_nnode;
			while (nnode_2 != nnode_1)
			{
				if (nnode_2->m_vertex->m_component == stamp && get_Edge(nnode_1->m_vertex->m_idx, nnode_2->m_vertex->m_idx) == NULL)
				{
					candidate = false;
					break;
				}
				nnode_2 = nnode_2->m_next;
			}
			if (candidate)
			{
				nnode_1->m_vertex->m_component = stamp;
				++clique_size;
			}
		}
		nnode_1 = nnode_1->m_next;
	}

	vertex->m_component = -1;
	nnode_1 = vertex->m_first_nnode;
	while (nnode_1 != NULL)
	{
		nnode_1->m_vertex->m_component = -1;
		nnode_1 = nnode_1->m_next;
	}

	return clique_size + 1;
}


int sIncrementalGraph::clusterize_Clique(sCluster *cluster, int idx, sVertex *vertex, int stamp)
{
	int clique_size = 0;

	vertex->m_component = stamp;
	cluster->m_Vertices[idx] = vertex;
	++idx;
	vertex->m_cluster = cluster->m_idx;

	sNeighborNode *nnode_1 = vertex->m_first_nnode;
	while (nnode_1 != NULL)
	{
		if (nnode_1->m_vertex->m_cluster < 0)
		{
			bool candidate = true;
			sNeighborNode *nnode_2 = vertex->m_first_nnode;
			while (nnode_2 != nnode_1)
			{
				{
					if (nnode_2->m_vertex->m_component == stamp && get_Edge(nnode_1->m_vertex->m_idx, nnode_2->m_vertex->m_idx) == NULL)
					{
						candidate = false;
						break;
					}
				}
				nnode_2 = nnode_2->m_next;
			}
			if (candidate)
			{
				cluster->m_Vertices[idx++] = nnode_1->m_vertex;
				nnode_1->m_vertex->m_cluster = cluster->m_idx;
				nnode_1->m_vertex->m_component = stamp;
				++clique_size;
			}
		}
		nnode_1 = nnode_1->m_next;
	}

	return clique_size + 1;
}


int sIncrementalGraph::build_CoveringLayers(Clustering *clustering, Layering *layering)
{
	int N_Vertices = get_VertexCount();

	sVertex **buffer = new sVertex*[N_Vertices];
	if (buffer == NULL)
	{
		return -1;
	}
	assert(buffer != NULL);

	for (int i = 0; i < N_Vertices; ++i)
	{
		sVertex *vertex = get_Vertex(i);

		vertex->m_time_stamp = -1;
		vertex->m_Layer_cnt = 0;
		vertex->m_Layers = layering->m_layer_Indices + layering->m_Index_cnt;
		layering->m_Index_cnt += vertex->m_N_Neighs;

		sNeighborNode *nnode = vertex->m_first_nnode;
		while (nnode != NULL)
		{
			nnode->m_edge->m_active = true;
			nnode = nnode->m_next;
		}
	}
	m_time_stamp = 0;
	
	while (true)
	{
		int layer_size = build_NextCoveringLayer(clustering, layering, buffer);

		if (layer_size == 0)
		{
			break;
		}
	}
/*
	clustering->print();
	layering->print();
	print_CoveringLayers();
*/
	delete [] buffer;

	return 0;
}


int sIncrementalGraph::build_NextCoveringLayer(Clustering *sUNUSED(clustering), Layering *layering, sVertex **buffer)
{
	Layer &layer = layering->m_Layers[layering->m_cL];

	int N_Vertices = get_VertexCount();
	sVertex *best_vertex = get_Vertex(0);
	best_vertex->m_component = 0;
	int best_active_N_Neighs = calc_ActiveNeighbors(best_vertex);

	for (int i = 1; i < N_Vertices; ++i)
	{
		sVertex *vertex = get_Vertex(i);
		vertex->m_component = 0;
		
		int active_N_Neighs = calc_ActiveNeighbors(vertex);

		if (active_N_Neighs > best_active_N_Neighs)
		{
			best_vertex = vertex;
			best_active_N_Neighs = active_N_Neighs;
		}
	}
	if (best_active_N_Neighs == 0)
	{
		return 0;
	}

	int N_Candidates = 0;

	sNeighborNode *nnode = best_vertex->m_first_nnode;
	while (nnode != NULL)
	{
		if (nnode->m_edge->m_active)
		{
			++nnode->m_vertex->m_component;
			buffer[N_Candidates++] = nnode->m_vertex;
		}
		nnode = nnode->m_next;
	}

	layer.m_cV = 0;
	layer.m_vertices = layering->m_layer_Vertices + layering->m_Vertex_cnt;

	layer.m_vertices[layer.m_cV++] = best_vertex;
	best_vertex->m_Layers[best_vertex->m_Layer_cnt++] = layering->m_cL;
	
	int iteration = 1;

	while (N_Candidates > 0)
	{
		best_vertex = buffer[0];
		best_active_N_Neighs = calc_ActiveNeighbors(best_vertex);

		for (int i = 1; i < N_Candidates; ++i)
		{
			sVertex *vertex = buffer[i];
			int active_N_Neighs = calc_ActiveNeighbors(vertex);

			if (active_N_Neighs > best_active_N_Neighs)
			{
				best_vertex = vertex;
				best_active_N_Neighs = active_N_Neighs;
			}
		}
		layer.m_vertices[layer.m_cV++] = best_vertex;
		best_vertex->m_Layers[best_vertex->m_Layer_cnt++] = layering->m_cL;

		N_Candidates = 0;

		sNeighborNode *nnode = best_vertex->m_first_nnode;
		while (nnode != NULL)
		{
			if (nnode->m_edge->m_active)
			{
				if (nnode->m_vertex->m_component++ == iteration)
				{
					buffer[N_Candidates++] = nnode->m_vertex;
				}
			}
			nnode = nnode->m_next;
		}
		++iteration;
	}

	layer.m_vertices[layer.m_cV] = NULL;

	for (int v_idx_1 = 1; v_idx_1 < layer.m_cV; ++v_idx_1)
	{
		for (int v_idx_2 = 0; v_idx_2 < v_idx_1; ++v_idx_2)
		{
			sEdge *edge = get_Edge(layer.m_vertices[v_idx_2]->m_idx, layer.m_vertices[v_idx_1]->m_idx);
			edge->m_active = false;
		}
	}
	++m_time_stamp;
	
	layering->m_Vertex_cnt += layer.m_cV + 1;
	++layering->m_cL;

	assert(layering->m_Vertex_cnt <= layering->m_N_Vertices);

	return layer.m_cV;
}


int sIncrementalGraph::compute_MaximumFlow(sVertex *source, sVertex *sink)
{
	int result;
	sVertexQueue relabel_queue;
	sEdgeQueue push_queue_in, push_queue_out;

	if ((result = relabel_queue.init(m_cV)) < 0)
	{
		return result;
	}
	if ((result = push_queue_in.init(m_cE) < 0))
	{
		return result;
	}
	if ((result = push_queue_out.init(m_cE) < 0))
	{
		return result;
	}
	init_Preflow(source, sink, &relabel_queue);

	while (!relabel_queue.is_Empty())
	{
//		printf("Queue sizes:%d,%d,%d\n", relabel_queue.get_VertexCount(), push_queue_in.get_EdgeCount(), push_queue_out.get_EdgeCount());

		while (!relabel_queue.is_Empty() && push_queue_in.is_Empty() && push_queue_out.is_Empty())
		{
			sVertex *rlb_vertex = relabel_queue.pop_Front();
			rlb_vertex->m_to_relabel = false;
			relabel_Preflow(source, sink, rlb_vertex, &push_queue_in, &push_queue_out);
		}
		while (!push_queue_out.is_Empty())
		{
			sEdgeQueue::Edge push_out_edge = *push_queue_out.pop_Front();
			push_out_edge.m_edge->m_to_push = false;
			push_out_Preflow(source, sink, push_out_edge.m_u, push_out_edge.m_v, push_out_edge.m_edge, &relabel_queue, &push_queue_in, &push_queue_out);
		}
		while (!push_queue_in.is_Empty())
		{
			sEdgeQueue::Edge push_in_edge = *push_queue_in.pop_Front();
			push_in_edge.m_edge->m_to_push = false;
			push_in_Preflow(source, sink, push_in_edge.m_u, push_in_edge.m_v, push_in_edge.m_edge, &relabel_queue, &push_queue_in, &push_queue_out);
		}
	}

	return 0;
}


void sIncrementalGraph::init_Preflow(sVertex *source, sVertex *sUNUSED(sink), sVertexQueue *relabel_queue)
{
	int cV = 0;

	for (sVertexBlock *vblock = m_first_vblock; vblock != NULL; vblock = vblock->m_next)
	{
		if (m_cV - cV > m_V_block)
		{
			for (int i = 0; i < m_V_block; ++i)
			{
				sVertex *vertex = &vblock->m_vertices[i];
				vertex->m_height = vertex->m_excess = 0;
				vertex->m_to_relabel = false;

				sNeighborNode *nnode = vertex->m_first_out_node;
				for (int j = 0; j < vertex->m_N_Outs; ++j)
				{
					nnode->m_edge->m_flow = 0;
					nnode->m_edge->m_to_push = false;
					nnode = nnode->m_next;
				}
			}
			cV += m_V_block;
		}
		else
		{
			for (int i = 0; i < m_ccV; ++i)
			{
				sVertex *vertex = &vblock->m_vertices[i];
				vertex->m_height = vertex->m_excess = 0;
				vertex->m_to_relabel = false;

				sNeighborNode *nnode = vertex->m_first_out_node;
				for (int j = 0; j < vertex->m_N_Outs; ++j)
				{
					nnode->m_edge->m_flow = 0;
					nnode->m_edge->m_to_push = false;
					nnode = nnode->m_next;
				}
			}
		}
	}

	source->m_height = m_cV;

	sNeighborNode *nnode = source->m_first_out_node;
	for (int j = 0; j < source->m_N_Outs; ++j)
	{
		nnode->m_edge->m_flow = nnode->m_edge->m_capacity;
		nnode->m_vertex->m_excess += nnode->m_edge->m_flow;
		source->m_excess -= nnode->m_edge->m_flow;

		if (!nnode->m_vertex->m_to_relabel)
		{
			relabel_queue->push_Back(nnode->m_vertex);
			nnode->m_vertex->m_to_relabel = true;
		}
		nnode = nnode->m_next;
	}	
}


void sIncrementalGraph::push_in_Preflow(sVertex *source, sVertex *sink, sVertex *u, sVertex *v, sEdge *edge, sVertexQueue *relabel_queue, sEdgeQueue *push_queue_in, sEdgeQueue *push_queue_out)
{
	if (u->m_excess > 0)
	{
		int residue = edge->m_flow;
		int delta = sMIN(u->m_excess, residue);

		if (delta > 0)
		{
			if (v->m_excess == 0)
			{
				if (v->m_N_Outs > 0)
				{
					sNeighborNode *nnode = v->m_first_out_node;

					for (int j = 0; j < v->m_N_Outs; ++j)
					{
						if (v->m_height > nnode->m_vertex->m_height)
						{
							if (!nnode->m_edge->m_to_push)
							{
								push_queue_out->push_Back(v, nnode->m_vertex, nnode->m_edge);
								nnode->m_edge->m_to_push = true;
							}
						}
						nnode = nnode->m_next;
					}
				}
				if (v->m_N_Ins > 0)
				{
					sNeighborNode *nnode = v->m_first_in_node;

					for (int j = 0; j < v->m_N_Ins; ++j)
					{
						if (v->m_height > nnode->m_vertex->m_height)
						{
							if (!nnode->m_edge->m_to_push)
							{
								push_queue_in->push_Back(v, nnode->m_vertex, nnode->m_edge);
								nnode->m_edge->m_to_push = true;
							}
						}
						nnode = nnode->m_next;
					}
				}
			}

			edge->m_flow -= delta;
			v->m_excess += delta;
			u->m_excess -= delta;

			if (u != source && u != sink)
			{
				if (!u->m_to_relabel)
				{
					relabel_queue->push_Back(u);
					u->m_to_relabel = true;
				}
			}
			if (v != source && v != sink)
			{
				if (!v->m_to_relabel)
				{
					relabel_queue->push_Back(v);
					v->m_to_relabel = true;
				}
			}
		}
	}
}


void sIncrementalGraph::push_out_Preflow(sVertex *source, sVertex *sink, sVertex *u, sVertex *v, sEdge *edge, sVertexQueue *relabel_queue, sEdgeQueue *push_queue_in, sEdgeQueue *push_queue_out)
{
	if (u->m_excess > 0)
	{
		int residue = edge->m_capacity - edge->m_flow;
		int delta = sMIN(u->m_excess, residue);

		if (delta > 0)
		{
			if (v->m_excess == 0)
			{
				if (v->m_N_Outs > 0)
				{
					sNeighborNode *nnode = v->m_first_out_node;

					for (int j = 0; j < v->m_N_Outs; ++j)
					{
						if (v->m_height > nnode->m_vertex->m_height)
						{
							if (!nnode->m_edge->m_to_push)
							{
								push_queue_out->push_Back(v, nnode->m_vertex, nnode->m_edge);
								nnode->m_edge->m_to_push = true;
							}
						}
						nnode = nnode->m_next;
					}
				}
				if (v->m_N_Ins > 0)
				{
					sNeighborNode *nnode = v->m_first_in_node;

					for (int j = 0; j < v->m_N_Ins; ++j)
					{
						if (v->m_height > nnode->m_vertex->m_height)
						{
							if (!nnode->m_edge->m_to_push)
							{
								push_queue_in->push_Back(v, nnode->m_vertex, nnode->m_edge);
								nnode->m_edge->m_to_push = true;
							}
						}
						nnode = nnode->m_next;
					}
				}
			}
			edge->m_flow += delta;
			v->m_excess += delta;
			u->m_excess -= delta;

			if (u != source && u != sink)
			{
				if (!u->m_to_relabel)
				{
					relabel_queue->push_Back(u);
					u->m_to_relabel = true;
				}
			}
			if (v != source && v != sink)
			{
				if (!v->m_to_relabel)
				{
					relabel_queue->push_Back(v);
					v->m_to_relabel = true;
				}
			}
		}
	}
}


void sIncrementalGraph::relabel_Preflow(sVertex *sUNUSED(source), sVertex *sUNUSED(sink), sVertex *u, sEdgeQueue *push_queue_in, sEdgeQueue *push_queue_out)
{
	bool to_relabel = true;
	sNeighborNode *min_in_nnode = NULL;
	sNeighborNode *min_out_nnode = NULL;

	if (u->m_excess > 0)
	{
		if (u->m_N_Outs > 0)
		{
			sNeighborNode *nnode = u->m_first_out_node;
			
			for (int j = 0; j < u->m_N_Outs; ++j)
			{
				if (nnode->m_edge->m_flow < nnode->m_edge->m_capacity)
				{
					if (min_out_nnode != NULL)
					{
						if (min_out_nnode->m_vertex->m_height > nnode->m_vertex->m_height)
						{
							min_out_nnode = nnode;
						}
					}
					else
					{
				min_out_nnode = nnode;
					}
					if (u->m_height > nnode->m_vertex->m_height)
					{
						if (!nnode->m_edge->m_to_push)
						{
							push_queue_out->push_Back(u, nnode->m_vertex, nnode->m_edge);
							nnode->m_edge->m_to_push = true;
						}
						to_relabel = false;
					}
				}
				nnode = nnode->m_next;
			}
		}
		if (u->m_N_Ins > 0)
		{
			sNeighborNode *nnode = u->m_first_in_node;
			
			for (int j = 0; j < u->m_N_Ins; ++j)
			{
				if (nnode->m_edge->m_flow > 0)
				{
					if (min_in_nnode != NULL)
					{
						if (min_in_nnode->m_vertex->m_height > nnode->m_vertex->m_height)
						{
							min_in_nnode = nnode;
						}
					}
					else
					{
						min_in_nnode = nnode;
					}
					if (u->m_height > nnode->m_vertex->m_height)
					{
						if (!nnode->m_edge->m_to_push)
						{
							push_queue_in->push_Back(u, nnode->m_vertex, nnode->m_edge);
							nnode->m_edge->m_to_push = true;
						}
						to_relabel = false;
					}
				}
				nnode = nnode->m_next;
			}
		}
		if (to_relabel)
		{
			if (min_in_nnode != NULL)
			{
				if (min_out_nnode != NULL)
				{
					if (min_in_nnode->m_vertex->m_height > min_out_nnode->m_vertex->m_height)
					{
						if (!min_out_nnode->m_edge->m_to_push && min_out_nnode->m_edge->m_flow < min_out_nnode->m_edge->m_capacity)
						{
							push_queue_out->push_Back(u, min_out_nnode->m_vertex, min_out_nnode->m_edge);
							min_out_nnode->m_edge->m_to_push = true;
						}
						u->m_height = min_out_nnode->m_vertex->m_height + 1;
					}			
					else
					{
						if (!min_in_nnode->m_edge->m_to_push)
						{
							push_queue_in->push_Back(u, min_in_nnode->m_vertex, min_in_nnode->m_edge);
							min_in_nnode->m_edge->m_to_push = true;
						}
						u->m_height = min_in_nnode->m_vertex->m_height + 1;
					}
				}
				else
				{
					if (!min_in_nnode->m_edge->m_to_push && min_in_nnode->m_edge->m_flow > 0)
					{
						push_queue_in->push_Back(u, min_in_nnode->m_vertex, min_in_nnode->m_edge);
						min_in_nnode->m_edge->m_to_push = true;
					}
					u->m_height = min_in_nnode->m_vertex->m_height + 1;
				}
			}
			else
			{
				if (min_out_nnode != NULL)
				{
					if (!min_out_nnode->m_edge->m_to_push && min_out_nnode->m_edge->m_flow < min_out_nnode->m_edge->m_capacity)
					{
						push_queue_out->push_Back(u, min_out_nnode->m_vertex, min_out_nnode->m_edge);
						min_out_nnode->m_edge->m_to_push = true;
					}
					u->m_height = min_out_nnode->m_vertex->m_height + 1;
				}
				else
				{
					assert(false);
				}
			}
		}
	}
}


void sIncrementalGraph::print(void) const
{
	print_Undirected();
}


void sIncrementalGraph::print_Undirected(void) const
{
/*
	printf("m_V:%d, m_cV:%d, m_ccV:%d\n", m_V, m_cV, m_ccV);
	printf("m_E:%d, m_cE:%d, m_ccE:%d\n", m_E, m_cE, m_ccE);
	printf("m_N:%d, m_N_Neighs:%d, m_ccN:%d\n", m_N, m_N_Neighs, m_ccN);
*/
	int cV = 0;

	for (sVertexBlock *vblock = m_first_vblock; vblock != NULL; vblock = vblock->m_next)
	{
		if (m_cV - cV > m_V_block)
		{
			for (int i = 0; i < m_V_block; ++i)
			{
				sVertex *vertex = &vblock->m_vertices[i];
				printf("%d: {", vertex->m_idx);

				sNeighborNode *nnode = vertex->m_first_nnode;
				for (int j = 0; j < vertex->m_N_Neighs - 1; ++j)
				{
					printf("%d(%d),", nnode->m_vertex->m_idx, nnode->m_edge->m_weight);
					nnode = nnode->m_next;
				}
				if (vertex->m_N_Neighs > 0)
				{
					printf("%d(%d)", nnode->m_vertex->m_idx, nnode->m_edge->m_weight);
				}
				printf("}\n");
			}
			cV += m_V_block;
		}
		else
		{
			for (int i = 0; i < m_ccV; ++i)
			{
				sVertex *vertex = &vblock->m_vertices[i];
				printf("%d: {", vertex->m_idx);

				sNeighborNode *nnode = vertex->m_first_nnode;
				for (int j = 0; j < vertex->m_N_Neighs - 1; ++j)
				{
					printf("%d(%d),", nnode->m_vertex->m_idx, nnode->m_edge->m_weight);
					nnode = nnode->m_next;
				}
				if (vertex->m_N_Neighs > 0)
				{
					printf("%d(%d)", nnode->m_vertex->m_idx, nnode->m_edge->m_weight);
				}
				printf("}\n");
			}
			break;
		}
	}
}


void sIncrementalGraph::print_Directed(void) const
{
/*
	printf("m_V:%d, m_cV:%d, m_ccV:%d\n", m_V, m_cV, m_ccV);
	printf("m_E:%d, m_cE:%d, m_ccE:%d\n", m_E, m_cE, m_ccE);
	printf("m_N:%d, m_N_Neighs:%d, m_ccN:%d\n", m_N, m_N_Neighs, m_ccN);
*/
	int cV = 0;

	for (sVertexBlock *vblock = m_first_vblock; vblock != NULL; vblock = vblock->m_next)
	{
		if (m_cV - cV > m_V_block)
		{
			for (int i = 0; i < m_V_block; ++i)
			{
				sVertex *vertex = &vblock->m_vertices[i];
				printf("%d: (", vertex->m_idx);

				sNeighborNode *nnode = vertex->m_first_out_node;
				for (int j = 0; j < vertex->m_N_Outs - 1; ++j)
				{
					printf("%d(%d),", nnode->m_vertex->m_idx, nnode->m_edge->m_capacity);
					nnode = nnode->m_next;
				}
				if (vertex->m_N_Outs > 0)
				{
					printf("%d(%d)", nnode->m_vertex->m_idx, nnode->m_edge->m_capacity);
				}
				printf(") ");

				printf("[");
				nnode = vertex->m_first_in_node;
				for (int j = 0; j < vertex->m_N_Ins - 1; ++j)
				{
					printf("%d(%d),", nnode->m_vertex->m_idx, nnode->m_edge->m_capacity);
					nnode = nnode->m_next;
				}
				if (vertex->m_N_Ins > 0)
				{
					printf("%d(%d)", nnode->m_vertex->m_idx, nnode->m_edge->m_capacity);
				}
				printf("]\n");
			}
			cV += m_V_block;
		}
		else
		{
			for (int i = 0; i < m_ccV; ++i)
			{
				sVertex *vertex = &vblock->m_vertices[i];
				printf("%d: (", vertex->m_idx);

				sNeighborNode *nnode = vertex->m_first_out_node;
				for (int j = 0; j < vertex->m_N_Outs - 1; ++j)
				{
					printf("%d(%d),", nnode->m_vertex->m_idx, nnode->m_edge->m_capacity);
					nnode = nnode->m_next;
				}
				if (vertex->m_N_Outs > 0)
				{
					printf("%d(%d)", nnode->m_vertex->m_idx, nnode->m_edge->m_capacity);
				}
				printf(") ");

				printf("[");
				nnode = vertex->m_first_in_node;
				for (int j = 0; j < vertex->m_N_Ins - 1; ++j)
				{
					printf("%d(%d),", nnode->m_vertex->m_idx, nnode->m_edge->m_capacity);
					nnode = nnode->m_next;
				}
				if (vertex->m_N_Ins > 0)
				{
					printf("%d(%d)", nnode->m_vertex->m_idx, nnode->m_edge->m_capacity);
				}
				printf("]\n");
			}
			break;
		}
	}
}


void sIncrementalGraph::print_DirectedFlow(void) const
{
/*
	printf("m_V:%d, m_cV:%d, m_ccV:%d\n", m_V, m_cV, m_ccV);
	printf("m_E:%d, m_cE:%d, m_ccE:%d\n", m_E, m_cE, m_ccE);
	printf("m_N:%d, m_N_Neighs:%d, m_ccN:%d\n", m_N, m_N_Neighs, m_ccN);
*/
	int cV = 0;

	for (sVertexBlock *vblock = m_first_vblock; vblock != NULL; vblock = vblock->m_next)
	{
		if (m_cV - cV > m_V_block)
		{
			for (int i = 0; i < m_V_block; ++i)
			{
				sVertex *vertex = &vblock->m_vertices[i];
				printf("%d {h=%d,e=%d}: (", vertex->m_idx, vertex->m_height, vertex->m_excess);

				sNeighborNode *nnode = vertex->m_first_out_node;
				for (int j = 0; j < vertex->m_N_Outs - 1; ++j)
				{
					printf("%d(%d/%d),", nnode->m_vertex->m_idx, nnode->m_edge->m_capacity, nnode->m_edge->m_flow);
					nnode = nnode->m_next;
				}
				if (vertex->m_N_Outs > 0)
				{
					printf("%d(%d/%d)", nnode->m_vertex->m_idx, nnode->m_edge->m_capacity, nnode->m_edge->m_flow);
				}
				printf(") ");

				printf("[");
				nnode = vertex->m_first_in_node;
				for (int j = 0; j < vertex->m_N_Ins - 1; ++j)
				{
					printf("%d(%d/%d),", nnode->m_vertex->m_idx, nnode->m_edge->m_capacity, nnode->m_edge->m_flow);
					nnode = nnode->m_next;
				}
				if (vertex->m_N_Ins > 0)
				{
					printf("%d(%d/%d)", nnode->m_vertex->m_idx, nnode->m_edge->m_capacity, nnode->m_edge->m_flow);
				}
				printf("]\n");
			}
			cV += m_V_block;
		}
		else
		{
			for (int i = 0; i < m_ccV; ++i)
			{
				sVertex *vertex = &vblock->m_vertices[i];
				printf("%d {h=%d,e=%d}: (", vertex->m_idx, vertex->m_height, vertex->m_excess);

				sNeighborNode *nnode = vertex->m_first_out_node;
				for (int j = 0; j < vertex->m_N_Outs - 1; ++j)
				{
					printf("%d(%d/%d),", nnode->m_vertex->m_idx, nnode->m_edge->m_capacity, nnode->m_edge->m_flow);
					nnode = nnode->m_next;
				}
				if (vertex->m_N_Outs > 0)
				{
					printf("%d(%d/%d)", nnode->m_vertex->m_idx, nnode->m_edge->m_capacity, nnode->m_edge->m_flow);
				}
				printf(") ");

				printf("[");
				nnode = vertex->m_first_in_node;
				for (int j = 0; j < vertex->m_N_Ins - 1; ++j)
				{
					printf("%d(%d/%d),", nnode->m_vertex->m_idx, nnode->m_edge->m_capacity, nnode->m_edge->m_flow);
					nnode = nnode->m_next;
				}
				if (vertex->m_N_Ins > 0)
				{
					printf("%d(%d/%d)", nnode->m_vertex->m_idx, nnode->m_edge->m_capacity, nnode->m_edge->m_flow);
				}
				printf("]\n");
			}
			break;
		}
	}
}


void sIncrementalGraph::print_CoveringLayers(void) const
{
	int cV = 0;
	char literal_str[128];

	for (sVertexBlock *vblock = m_first_vblock; vblock != NULL; vblock = vblock->m_next)
	{
		if (m_cV - cV > m_V_block)
		{
			for (int i = 0; i < m_V_block; ++i)
			{
				sVertex *vertex = &vblock->m_vertices[i];
				m_formula->sprint_literal(literal_str, vertex->m_literal);

				printf("%d(%s): {", vertex->m_idx, literal_str);

				for (int j = 0; j < vertex->m_Layer_cnt - 1; ++j)
				{
					printf("%d,", vertex->m_Layers[j]);
				}
				if (vertex->m_Layer_cnt > 0)
				{
					printf("%d", vertex->m_Layers[vertex->m_Layer_cnt - 1]);
				}
				printf("}\n");
			}
			cV += m_V_block;
		}
		else
		{
			for (int i = 0; i < m_ccV; ++i)
			{
				sVertex *vertex = &vblock->m_vertices[i];
				m_formula->sprint_literal(literal_str, vertex->m_literal);

				printf("%d(%s): {", vertex->m_idx, literal_str);

				for (int j = 0; j < vertex->m_Layer_cnt - 1; ++j)
				{
					printf("%d,", vertex->m_Layers[j]);
				}
				if (vertex->m_Layer_cnt > 0)
				{
					printf("%d", vertex->m_Layers[vertex->m_Layer_cnt - 1]);
				}
				printf("}\n");
			}
			break;
		}
	}
}


void sIncrementalGraph::print_flow_specific(void) const
{
	int cV = 0;

	for (sVertexBlock *vblock = m_first_vblock; vblock != NULL; vblock = vblock->m_next)
	{
		if (m_cV - cV > m_V_block)
		{
			for (int i = 0; i < m_V_block; ++i)
			{
				sVertex *vertex = &vblock->m_vertices[i];
				printf("%d  (score:%d) {", vertex->m_idx, vertex->m_score);

				sNeighborNode *nnode = vertex->m_first_nnode;
				for (int j = 0; j < vertex->m_N_Neighs - 1; ++j)
				{
					printf("%d,", nnode->m_vertex->m_idx);
					nnode = nnode->m_next;
				}
				if (vertex->m_N_Neighs > 0)
				{
					printf("%d", nnode->m_vertex->m_idx);
				}
				printf("}\n");
			}
			cV += m_V_block;
		}
		else
		{
			for (int i = 0; i < m_ccV; ++i)
			{
				sVertex *vertex = &vblock->m_vertices[i];
				printf("%d: (score:%d) {", vertex->m_idx, vertex->m_score);

				sNeighborNode *nnode = vertex->m_first_nnode;
				for (int j = 0; j < vertex->m_N_Neighs - 1; ++j)
				{
					printf("%d,", nnode->m_vertex->m_idx);
					nnode = nnode->m_next;
				}
				if (vertex->m_N_Neighs > 0)
				{
					printf("%d", nnode->m_vertex->m_idx);
				}
				printf("}\n");
			}
			break;
		}
	}
}	


void sIncrementalGraph::print_flow_specific_nontrivial(void) const
{
	printf("Graph: flow specific nontrivial --------\n");
	int cV = 0;

	for (sVertexBlock *vblock = m_first_vblock; vblock != NULL; vblock = vblock->m_next)
	{
		if (m_cV - cV > m_V_block)
		{
			for (int i = 0; i < m_V_block; ++i)
			{
				sVertex *vertex = &vblock->m_vertices[i];
				if (vertex->m_N_Neighs > 0)
				{
					printf("%d  (score:%d) {", vertex->m_idx, vertex->m_score);

					sNeighborNode *nnode = vertex->m_first_nnode;
					for (int j = 0; j < vertex->m_N_Neighs - 1; ++j)
					{
						printf("%d,", nnode->m_vertex->m_idx);
						nnode = nnode->m_next;
					}
					if (vertex->m_N_Neighs > 0)
					{
						printf("%d", nnode->m_vertex->m_idx);
					}
					printf("}\n");
				}
			}
			cV += m_V_block;
		}
		else
		{
			for (int i = 0; i < m_ccV; ++i)
			{
				sVertex *vertex = &vblock->m_vertices[i];
				if (vertex->m_N_Neighs > 0)
				{
					printf("%d: (score:%d) {", vertex->m_idx, vertex->m_score);

					sNeighborNode *nnode = vertex->m_first_nnode;
					for (int j = 0; j < vertex->m_N_Neighs - 1; ++j)
					{
						printf("%d,", nnode->m_vertex->m_idx);
						nnode = nnode->m_next;
					}
					if (vertex->m_N_Neighs > 0)
					{
						printf("%d", nnode->m_vertex->m_idx);
					}
					printf("}\n");
				}
			}
			break;
		}
	}
}	


void sIncrementalGraph::export_GR(bool exp_literals, bool exp_clusters, bool exp_layers, bool exp_visits) const
{
	char label[128], label_neighbor[128], data[128];

	printf("<graph>\n");
	printf("    <nodes>\n");

	int cV = 0;

	for (sVertexBlock *vblock = m_first_vblock; vblock != NULL; vblock = vblock->m_next)
	{
		if (m_cV - cV > m_V_block)
		{
			for (int i = 0; i < m_V_block; ++i)
			{
				sVertex *vertex = &vblock->m_vertices[i];

				get_label(label, vertex, exp_literals);
				get_data(data, vertex, exp_clusters, exp_layers, exp_visits);

				printf("        <node label=\"%s\" data=\"%s\">\n", label, data);
				printf("            <position>\n");
				printf("                <2D_point x='0' y='0'>\n");
				printf("            </position>\n");
				printf("        </node>\n");
			}
			cV += m_V_block;
		}
		else
		{
			for (int i = 0; i < m_ccV; ++i)
			{
				sVertex *vertex = &vblock->m_vertices[i];

				get_label(label, vertex, exp_literals);
				get_data(data, vertex, exp_clusters, exp_layers, exp_visits);

				printf("        <node label=\"%s\" data=\"%s\">\n", label, data);
				printf("            <position>\n");
				printf("                <2D_point x='0' y='0'>\n");
				printf("            </position>\n");
				printf("        </node>\n");

			}
			break;
		}
	}
	printf("    </nodes>\n");
	printf("    <edges>\n");

	cV = 0;

	for (sVertexBlock *vblock = m_first_vblock; vblock != NULL; vblock = vblock->m_next)
	{
		if (m_cV - cV > m_V_block)
		{
			for (int i = 0; i < m_V_block; ++i)
			{
				sVertex *vertex = &vblock->m_vertices[i];

				sNeighborNode *nnode = vertex->m_first_nnode;
				for (int j = 0; j < vertex->m_N_Neighs; ++j)
				{
					if (nnode->m_vertex->m_idx > vertex->m_idx)
					{
						get_label(label_neighbor, nnode->m_vertex, exp_literals);
						get_label(label, vertex, exp_literals);

						printf("        <edge>\n");
						printf("            <first_node>%s</first_node>\n", label_neighbor);
						printf("            <second_node>%s</second_node>\n", label);
						printf("        </edge>\n");
					}
					nnode = nnode->m_next;
				}
			}
			cV += m_V_block;
		}
		else
		{
			for (int i = 0; i < m_ccV; ++i)
			{
				sVertex *vertex = &vblock->m_vertices[i];

				sNeighborNode *nnode = vertex->m_first_nnode;
				for (int j = 0; j < vertex->m_N_Neighs; ++j)
				{
					if (nnode->m_vertex->m_idx > vertex->m_idx)
					{
						get_label(label_neighbor, nnode->m_vertex, exp_literals);
						get_label(label, vertex, exp_literals);

						printf("        <edge>\n");
						printf("            <first_node>%s</first_node>\n", label_neighbor);
						printf("            <second_node>%s</second_node>\n", label);
						printf("        </edge>\n");
					}
					nnode = nnode->m_next;
				}
			}
			break;
		}
	}
	printf("    </edges>\n");
	printf("</graph>\n");
}


int sIncrementalGraph::calc_ClusterNeighbors(sVertex *vertex) const
{
	int N_neighbors = 0;
	sNeighborNode *nnode = vertex->m_first_nnode;

	while (nnode != NULL)
	{
		if (nnode->m_vertex->m_cluster < 0)
		{
			++N_neighbors;
		}
		nnode = nnode->m_next;
	}

	return N_neighbors;
}


int sIncrementalGraph::calc_VertexScore(sVertex *vertex) const
{
	return calc_ClusterNeighbors(vertex);
}


int sIncrementalGraph::calc_VertexScore_2(sVertex *vertex) const
{
	int score_2 = 0;
	int score = calc_ClusterNeighbors(vertex);

	sNeighborNode *nnode = vertex->m_first_nnode;
	while (nnode != NULL)
	{
		if (nnode->m_vertex->m_cluster < 0)
		{
			int N_neighbors = calc_ClusterNeighbors(nnode->m_vertex);
			int difference = sMAX(0, score - N_neighbors);
			score_2 += sMAX(0, score - difference - 1);
		}
		nnode = nnode->m_next;
	}

	return score_2;
}


void sIncrementalGraph::get_label(char *label, sVertex *vertex, bool exp_literals) const
{
	if (exp_literals)
	{
		char literal_str[32];
		m_formula->sprint_literal(literal_str, vertex->m_literal);
		sprintf(label, "%d (%s)", vertex->m_idx, literal_str);
	}
	else
	{
		sprintf(label, "%d", vertex->m_idx);
	}
}


void sIncrementalGraph::get_data(char *data, sVertex *vertex, bool exp_clusters, bool exp_layers, bool exp_visits) const
{
	if (exp_clusters)
	{
		if (exp_layers)
		{
			if (exp_visits)
			{
				sprintf(data, "c=%d l=%d v=%d", vertex->m_cluster, vertex->m_layer, vertex->m_max_visits);
			}
			else
			{
				sprintf(data, "c=%d l=%d", vertex->m_cluster, vertex->m_layer);
			}
		}
		else
		{
			if (exp_visits)
			{
				sprintf(data, "c=%d v=%d", vertex->m_cluster, vertex->m_max_visits);
			}
			else
			{
				sprintf(data, "c=%d", vertex->m_cluster);
			}
		}
	}
	else
	{
		if (exp_layers)
		{
			if (exp_visits)
			{
				sprintf(data, "l=%d v=%d", vertex->m_layer, vertex->m_max_visits);
			}
			else
			{
				sprintf(data, "l=%d", vertex->m_layer);
			}
		}
		else
		{
			if (exp_visits)
			{
				sprintf(data , "v=%d", vertex->m_max_visits);
			}
			else
			{
				data[0] = '\0';
			}
		}
	}
}


int sIncrementalGraph::alloc_vertices(int cV)
{
	int required = m_cV + cV;

	while (required > m_V)
	{
		m_last_vblock = &(*m_last_vblock)->m_next;

		*m_last_vblock = (sVertexBlock*)malloc(sizeof(sVertexBlock));
		if (*m_last_vblock == NULL)
		{
			return -1;
		}
		assert(*m_last_vblock != NULL);

		(*m_last_vblock)->m_vertices = new sVertex[m_V_block];
		if ((*m_last_vblock)->m_vertices == NULL)
		{
			return -1;
		}
		assert((*m_last_vblock)->m_vertices != NULL);

		(*m_last_vblock)->m_next = NULL;
		m_V += m_V_block;
	}

	return 0;
}


int sIncrementalGraph::alloc_edge(sEdge **edge)
{
	if (m_cE + 1 > m_E)
	{
		m_last_eblock = &(*m_last_eblock)->m_next;

		*m_last_eblock = (sEdgeBlock*)malloc(sizeof(sEdgeBlock));
		if (*m_last_eblock == NULL)
		{
			return -1;
		}
		assert(*m_last_eblock != NULL);

		(*m_last_eblock)->m_edges = (sEdge*)malloc(m_E_block * sizeof(sEdge));
		if ((*m_last_eblock)->m_edges == NULL)
		{
			return -1;
		}
		assert((*m_last_eblock)->m_edges != NULL);

		(*m_last_eblock)->m_next = NULL;
		m_E += m_E_block;

		m_ccE = 0;
	}

	*edge = &(*m_last_eblock)->m_edges[m_ccE++];
	++m_cE;

	return 0;
}


int sIncrementalGraph::alloc_neighbor(sNeighborNode **nnode)
{
	if (m_cN + 1 > m_N)
	{
		m_last_nblock = &(*m_last_nblock)->m_next;

		*m_last_nblock = (sNeighborBlock*)malloc(sizeof(sNeighborBlock));
		if (*m_last_nblock == NULL)
		{
			return -1;
		}
		assert(*m_last_nblock != NULL);

		(*m_last_nblock)->m_nnodes = (sNeighborNode*)malloc(m_N_block * sizeof(sNeighborNode));
		if ((*m_last_nblock)->m_nnodes == NULL)
		{
			return -1;
		}
		assert((*m_last_nblock)->m_nnodes != NULL);

		(*m_last_nblock)->m_next = NULL;
		m_N += m_N_block;

		m_ccN = 0;
	}

	*nnode = &(*m_last_nblock)->m_nnodes[m_ccN++];
	++m_cN;

	return 0;
}


/*----------------------------------------------------------------------------*/
// Clustering class

int Clustering::init(const sBooleanFormula *formula, sClause **path, int length, sIncrementalGraph *graph)
{
	m_formula = formula;
	m_graph = graph;
	m_cC = length;

	m_clusters = (Cluster*)malloc(length * sizeof(Cluster));
	if (m_clusters == NULL)
	{
		return -1;
	}
	assert(m_clusters != NULL);
	
	for (int i = 0; i < length; ++i)
	{
		Cluster *cluster = &m_clusters[i];
		cluster->m_cV = path[i]->m_cL;
		cluster->m_vertices = (sVertex**)malloc(path[i]->m_cL * sizeof(sVertex*));

		if (cluster->m_vertices == NULL)
		{
			return -1;
		}
		assert(cluster->m_vertices != NULL);
	}

	return 0;
}


void Clustering::destroy(void)
{	
	for (int i = 0; i < m_cC; ++i)
	{
		Cluster *cluster = &m_clusters[i];
		free(cluster->m_vertices);
	}

	free(m_clusters);
}


void Clustering::print(void) const
{
	printf("Clustering:\n");
	for (int i = 0; i < m_cC; ++i)
	{
		Cluster *cluster = &m_clusters[i];
		printf("%d: ", i);

		int j;

		for (j = 0; j < cluster->m_cV - 1; ++j)
		{
			printf("%d(", cluster->m_vertices[j]->m_idx);
			m_formula->print_literal(cluster->m_vertices[j]->m_literal);
			printf("),");
		}
		if (cluster->m_cV > 0)
		{
			printf("%d(", cluster->m_vertices[j]->m_idx);
			m_formula->print_literal(cluster->m_vertices[j]->m_literal);
			printf(")");
		}
		printf("\n");
	}
}


void Clustering::print_distances(void) const
{
	printf("Clustering:\n");
	for (int i = 0; i < m_cC; ++i)
	{
		Cluster *cluster = &m_clusters[i];
		printf("%d: ", i);

		int j;

		for (j = 0; j < cluster->m_cV - 1; ++j)
		{
			printf("%d(", cluster->m_vertices[j]->m_idx);
			m_formula->print_literal(cluster->m_vertices[j]->m_literal);
			printf(",%d),", cluster->m_vertices[j]->m_distance);
		}
		if (cluster->m_cV > 0)
		{
			printf("%d(", cluster->m_vertices[j]->m_idx);
			m_formula->print_literal(cluster->m_vertices[j]->m_literal);
			printf(",%d)", cluster->m_vertices[j]->m_distance);
		}
		printf("\n");
	}
}


void Clustering::print_visit_matrices(void) const
{
	printf("Clustering:\n");
	for (int i = 0; i < m_cC; ++i)
	{
		Cluster *cluster = &m_clusters[i];
		printf("%d:\n", i);

		int j;

		for (j = 0; j < cluster->m_cV; ++j)
		{
			printf("%d(", cluster->m_vertices[j]->m_idx);
			m_formula->print_literal(cluster->m_vertices[j]->m_literal);
			printf(") layer:%d max_visits:%d\n", cluster->m_vertices[j]->m_layer, cluster->m_vertices[j]->m_max_visits);
			cluster->m_vertices[j]->m_vmatrix.print();
		}
		printf("\n");
	}
}


/*----------------------------------------------------------------------------*/
// sIncrementalClustering class

int sIncrementalClustering::init(const sBooleanFormula *formula, sIncrementalGraph *graph, int C_block, int V_block)
{
	m_formula = formula;
	m_graph = graph;

	m_last_idx = 0;
	m_cC = m_cV = 0;
	m_ccC = m_ccV = 0;

	m_C_block = C_block;
	m_V_block = V_block;

	m_first_vblock = (sPtr_VertexBlock*)malloc(sizeof(sPtr_VertexBlock));
	if (m_first_vblock == NULL)
	{
		return -1;
	}
	assert(m_first_vblock != NULL);

	m_first_vblock->m_Vertices = (sVertex**)malloc(V_block * sizeof(sVertex*));
	if (m_first_vblock->m_Vertices == NULL)
	{
		return -1;
	}
	assert(m_first_vblock->m_Vertices != NULL);

	m_first_vblock->m_next = NULL;
	m_V = m_V_block;

	m_first_cblock = (sClusterBlock*)malloc(sizeof(sClusterBlock));
	if (m_first_cblock == NULL)
	{
		return -1;
	}
	assert(m_first_cblock != NULL);

	m_first_cblock->m_clusters = (sCluster*)malloc(C_block * sizeof(sCluster));
	if (m_first_cblock->m_clusters == NULL)
	{
		return -1;
	}
	assert(m_first_cblock->m_clusters != NULL);

	m_first_cblock->m_next = NULL;
	m_C = m_C_block;

	m_last_vblock = &m_first_vblock;
	m_last_cblock = &m_first_cblock;

	return 0;
}


int sIncrementalClustering::get_ClusterCount(void) const
{
	return m_cC;
}


sCluster* sIncrementalClustering::get_cluster(int id)
{
	sClusterBlock *cblock = m_first_cblock;

	while (id >= m_C_block)
	{
		cblock = cblock->m_next;
		id -= m_C_block;
	}

	return &cblock->m_clusters[id];
}


int sIncrementalClustering::add_cluster(int nVertices)
{
	int result;

	sCluster *cluster;

	if ((result = alloc_cluster(&cluster)) < 0)
	{
		return result;
	}

	if ((result = alloc_cluster_vertices(nVertices, &cluster->m_Vertices)) < 0)
	{
		return result;
	}

	cluster->m_idx = m_last_idx++;
	cluster->m_N_Vertices = nVertices;

	return 0;
}


void sIncrementalClustering::print_cluster(const sCluster *cluster) const
{
	int i;
	printf("%d: ", cluster->m_idx);

	for (i = 0; i < cluster->m_N_Vertices - 1; ++i)
	{
		sVertex *vertex = cluster->m_Vertices[i];

		printf("%d(", vertex->m_idx);
		m_formula->print_literal(vertex->m_literal);
		printf(",%d),", vertex->m_score);
	}

	sVertex *vertex = cluster->m_Vertices[i];
	
	printf("%d(", vertex->m_idx);
	m_formula->print_literal(vertex->m_literal);
	printf(",%d)", vertex->m_score);
}


void sIncrementalClustering::print(void) const
{
	int cC = 0;

	for (sClusterBlock *cblock = m_first_cblock; cblock != NULL; cblock = cblock->m_next)
	{
		if (m_cC - cC > m_C_block)
		{
			for (int i = 0; i < m_C_block; ++i)
			{
				sCluster *cluster = &cblock->m_clusters[i];
				print_cluster(cluster);
				printf("\n");
			}
			cC += m_C_block;
		}
		else
		{
			for (int i = 0; i < m_ccC; ++i)
			{
				sCluster *cluster = &cblock->m_clusters[i];
				print_cluster(cluster);
				printf("\n");
			}
			break;
		}
	}
}


void sIncrementalClustering::print_nontrivial(void) const
{
	printf("Non-trivial clustering --------\n");
	int cC = 0;

	for (sClusterBlock *cblock = m_first_cblock; cblock != NULL; cblock = cblock->m_next)
	{
		if (m_cC - cC > m_C_block)
		{
			for (int i = 0; i < m_C_block; ++i)
			{
				sCluster *cluster = &cblock->m_clusters[i];
				if (cluster->m_N_Vertices > 1)
				{
					print_cluster(cluster);
					printf("\n");
				}
			}
			cC += m_C_block;
		}
		else
		{
			for (int i = 0; i < m_ccC; ++i)
			{
				sCluster *cluster = &cblock->m_clusters[i];
				if (cluster->m_N_Vertices > 1)
				{
					print_cluster(cluster);
					printf("\n");
				}
			}
			break;
		}
	}
}


void sIncrementalClustering::destroy(void)
{
	sPtr_VertexBlock *vblock_delete;
	sPtr_VertexBlock *vblock = m_first_vblock;

	while (vblock != NULL)
	{
		free(vblock->m_Vertices);
		vblock_delete = vblock;
		vblock = vblock->m_next;

		free(vblock_delete);
	}

	sClusterBlock *cblock_delete;
	sClusterBlock *cblock = m_first_cblock;

	while (cblock != NULL)
	{
		free(cblock->m_clusters);
		cblock_delete = cblock;
		cblock = cblock->m_next;

		free(cblock_delete);
	}
}


int sIncrementalClustering::alloc_cluster(sCluster **cluster)
{
	if (m_cC + 1 > m_C)
	{
		m_last_cblock = &(*m_last_cblock)->m_next;

		*m_last_cblock = (sClusterBlock*)malloc(sizeof(sClusterBlock));
		if (*m_last_cblock == NULL)
		{
			return -1;
		}
		assert(*m_last_cblock != NULL);

		(*m_last_cblock)->m_clusters = (sCluster*)malloc(m_C_block * sizeof(sCluster));
		if ((*m_last_cblock)->m_clusters == NULL)
		{
			return -1;
		}
		assert((*m_last_cblock)->m_clusters != NULL);

		(*m_last_cblock)->m_next = NULL;
		m_C += m_C_block;

		m_ccC = 0;
	}
	*cluster = &(*m_last_cblock)->m_clusters[m_ccC++];
	++m_cC;

	return 0;
}


int sIncrementalClustering::alloc_cluster_vertices(int nVertices, sVertex ***Vertices)
{
	if (nVertices > m_V_block)
	{
		return -2;
	}
	assert(nVertices <= m_V_block);

	if (nVertices > m_V_block - m_ccV)
	{
		m_last_vblock = &(*m_last_vblock)->m_next;

		*m_last_vblock = (sPtr_VertexBlock*)malloc(sizeof(sPtr_VertexBlock));
		if (*m_last_vblock == NULL)
		{
			return -1;
		}
		assert(*m_last_vblock != NULL);

		(*m_last_vblock)->m_Vertices = (sVertex**)malloc(m_V_block * sizeof(sVertex*));
		if ((*m_last_vblock)->m_Vertices == NULL)
		{
			return -1;
		}
		assert((*m_last_vblock)->m_Vertices != NULL);

		(*m_last_vblock)->m_next = NULL;
		m_V += m_V_block;

		*Vertices = (*m_last_vblock)->m_Vertices;
		m_ccV += nVertices;
		m_cV += nVertices;
	}
	else
	{
		*Vertices = &(*m_last_vblock)->m_Vertices[m_ccV];
		m_ccV += nVertices;
		m_cV += nVertices;
	}

	return 0;
}


/*----------------------------------------------------------------------------*/
// Layering class

int Layering::init(const sBooleanFormula *formula, sIncrementalGraph *graph)
{
	m_formula = formula;
	m_graph = graph;

	m_N_Indices = 0;
	m_N_Vertices = 0;

	int N_Vertices = graph->get_VertexCount();
	for (int i = 0; i < N_Vertices; ++i)
	{
		sVertex *vertex = graph->get_Vertex(i);
		m_N_Indices += vertex->m_N_Neighs + 1;
		m_N_Vertices += 2 * vertex->m_N_Neighs + 1;
	}

	m_layer_Indices = (int*)malloc(m_N_Indices * sizeof(int));
	if (m_layer_Indices == NULL)
	{
		return -1;
	}
	assert(m_layer_Indices != NULL);
	m_Index_cnt = 0;

	m_layer_Vertices = (sVertex**)malloc(m_N_Vertices * sizeof(sVertex*));
	if (m_layer_Vertices == NULL)
	{
		return -1;
	}
	assert(m_layer_Vertices != NULL);
	m_Vertex_cnt = 0;

	m_L = graph->get_EdgeCount();
	m_Layers = (Layer*)malloc(m_L * sizeof(Layer));

	if (m_Layers == NULL)
	{
		return -1;
	}
	assert(m_Layers != NULL);

	m_cL = 0;

	return 0;
}


int Layering::init(const sBooleanFormula *formula, sClause **path, int length, sIncrementalGraph *graph)
{
	m_formula = formula;
	m_graph = graph;

	m_L = m_cL = 0;
	for (int l = 0; l < length; ++l)
	{
		if (path[l]->m_cL > m_L)
		{
			m_L = path[l]->m_cL;
		}
	}

	m_Layers = (Layer*)malloc(m_L * sizeof(Layer));
	if (m_Layers == NULL)
	{
		return -1;
	}
	assert(m_Layers != NULL);

	for (int i = 0; i < m_L; ++i)
	{
		Layer *layer = &m_Layers[i];
		layer->m_V = length;
		layer->m_cV = 0;
		layer->m_path_selects = 0;

		layer->m_vertices = (sVertex**)malloc(length * sizeof(sVertex*));
		if (layer->m_vertices == NULL)
		{
			return -1;
		}
		assert(layer->m_vertices != NULL);
	}

	return 0;
}


void Layering::destroy(void)
{
	free(m_layer_Indices);
	free(m_layer_Vertices);
	free(m_Layers);
}


void Layering::reset(void)
{
	for (int i = 0; i < m_cL; ++i)
	{
		Layer *layer = &m_Layers[i];
		layer->m_path_selects = 0;
	}
}


void Layering::print(void) const
{
	printf("Layering:\n");
	for (int i = 0; i < m_cL; ++i)
	{
		Layer *layer = &m_Layers[i];
		printf("%d: ", i);

		int j;

		for (j = 0; j < layer->m_cV - 1; ++j)
		{
			printf("%d (", layer->m_vertices[j]->m_idx);
			m_formula->print_literal(layer->m_vertices[j]->m_literal);
			printf("), ");
		}
		if (layer->m_cV > 0)
		{
			printf("%d (", layer->m_vertices[j]->m_idx);
			m_formula->print_literal(layer->m_vertices[j]->m_literal);
			printf(")");
		}
		printf("\n");
	}
}


void Layering::print_visits(void) const
{
	printf("Layering:\n");
	for (int i = 0; i < m_cL; ++i)
	{
		Layer *layer = &m_Layers[i];
		printf("%d: ", i);

		int j;

		for (j = 0; j < layer->m_cV - 1; ++j)
		{
			printf("%d (", layer->m_vertices[j]->m_idx);
			m_formula->print_literal(layer->m_vertices[j]->m_literal);
			printf(")-%d, ", layer->m_vertices[j]->m_max_visits);
		}
		if (layer->m_cV > 0)
		{
			printf("%d (", layer->m_vertices[j]->m_idx);
			m_formula->print_literal(layer->m_vertices[j]->m_literal);
			printf(")-%d", layer->m_vertices[j]->m_max_visits);
		}
		printf("\n");
	}
}


int Layering::init_visit_matrices(int L, int max_visits)
{
	int result;

	for (int i = 0; i < m_cL; ++i)
	{
		Layer &layer = m_Layers[i];

		for (int j = 0; j < layer.m_cV; ++j)
		{
			sVertex *vertex = layer.m_vertices[j];
			if ((result = vertex->m_vmatrix.init(L, max_visits)) < 0)
			{
				return result;
			}
		}
	}

	return 0;
}


void Layering::destroy_visit_matrices(void)
{
	for (int i = 0; i < m_cL; ++i)
	{
		Layer &layer = m_Layers[i];

		for (int j = 0; j < layer.m_cV; ++j)
		{
			sVertex *vertex = layer.m_vertices[j];
			vertex->m_vmatrix.destroy();
		}
	}
}


/*----------------------------------------------------------------------------*/
// VMatrix class

int VMatrix::init(int L, int max_visits)
{
	m_L = L;
	m_max_visits = max_visits;

	m_cPaths = (int**)malloc(L * sizeof(int*));
	if (m_cPaths == NULL)
	{
		return -1;
	}
	assert(m_cPaths != NULL);

	for (int i = 0; i < L; ++i)
	{
		m_cPaths[i] = (int*)malloc((max_visits + 1) * sizeof(int));
		if (m_cPaths[i] == NULL)
		{
			return -1;
		}
		assert(m_cPaths[i] != NULL);
	}
	reset();

	return 0;
}


void VMatrix::destroy(void)
{
	for (int i = 0; i < m_L; ++i)
	{
		free(m_cPaths[i]);
	}
	free(m_cPaths);
}


void VMatrix::reset(void)
{
	m_total_paths = 0;

	for (int i = 0; i < m_L; ++i)
	{
		for (int j = 0; j <= m_max_visits; ++j)
		{
			m_cPaths[i][j] = 0;
		}
	}
}


void VMatrix::print(void)
{
	printf("VMatrix:%d\n", m_total_paths);

	for (int i = 0; i < m_L; ++i)
	{
		for (int j = 0; j <= m_max_visits; ++j)
		{
			printf("%d\t", m_cPaths[i][j]);
		}
		printf("\n");
	}
}


/*----------------------------------------------------------------------------*/
// sVertexHeap class

int sVertexHeap::init(int N_Vertices)
{
	m_N_Vertices = N_Vertices;

	m_Vertices = (sVertex**)malloc((N_Vertices + 1) * sizeof(sVertex*));
	if (m_Vertices == NULL)
	{
		return -1;
	}
	assert(m_Vertices != NULL);

	m_Vertex_cnt = 0;

	return 0;
}


void sVertexHeap::destroy(void)
{
	free(m_Vertices);
}


int sVertexHeap::get_VertexCount(void)
{
	return m_Vertex_cnt;
}


void sVertexHeap::insert_Vertex(sVertex *vertex)
{
	m_Vertices[m_Vertex_cnt++ + 1] = vertex;
	vertex->m_heap_idx = m_Vertex_cnt;

	maintain_OrderingIncreased(m_Vertex_cnt);
}


void sVertexHeap::remove_Vertex(sVertex *vertex)
{
	if (m_Vertex_cnt > 1)
	{
		sVertex *last_vertex = m_Vertices[m_Vertex_cnt];
	
		if (vertex->m_score > last_vertex->m_score)
		{
			last_vertex->m_heap_idx = vertex->m_heap_idx;
			m_Vertices[vertex->m_heap_idx] = last_vertex;
			--m_Vertex_cnt;
			maintain_OrderingDecreased(vertex->m_heap_idx);
		}
		else
		{
			last_vertex->m_heap_idx = vertex->m_heap_idx;
			m_Vertices[vertex->m_heap_idx] = last_vertex;
			--m_Vertex_cnt;
			maintain_OrderingIncreased(vertex->m_heap_idx);
		}
	}
	else
	{
		m_Vertices[1] = NULL;
		--m_Vertex_cnt;
	}
}


sVertex* sVertexHeap::get_Min(void) const
{
	return m_Vertices[1];
}


void sVertexHeap::remove_Min(void)
{
	if (m_Vertex_cnt > 1)
	{
		m_Vertices[1] = m_Vertices[m_Vertex_cnt--];
		m_Vertices[1]->m_heap_idx = 1;
		
		maintain_OrderingDecreased(1);
	}
	else
	{
		m_Vertices[1] = NULL;
		--m_Vertex_cnt;
	}
}


void sVertexHeap::change_Score(sVertex *vertex, int score)
{
	if (vertex->m_score > score)
	{
		vertex->m_score = score;
		maintain_OrderingDecreased(vertex->m_heap_idx);
	}
	else
	{
		vertex->m_score = score;
		maintain_OrderingIncreased(vertex->m_heap_idx);
	}
}


void sVertexHeap::to_Screen(void) const
{
	for (int i = 1; i <= m_Vertex_cnt; ++i)
	{
		printf("%d (%d)\n", m_Vertices[i]->m_idx, m_Vertices[i]->m_score);
	}
}


void sVertexHeap::maintain_OrderingIncreased(int idx)
{
	while (true)
	{
		int pred = idx / 2;

		if (pred < 1)
		{
			break;
		}
		else
		{
			if (m_Vertices[idx]->m_score > m_Vertices[pred]->m_score)
			{
				sVertex *vertex = m_Vertices[idx];
				m_Vertices[idx] = m_Vertices[pred];
				m_Vertices[pred] = vertex;

				m_Vertices[idx]->m_heap_idx = idx;
				m_Vertices[pred]->m_heap_idx = pred;
				idx = pred;
			}
			else
			{
				break;
			}
		}
	}
}


void sVertexHeap::maintain_OrderingDecreased(int idx)
{
	while (true)
	{
		int succ_1 = 2 * idx;
		int succ_2 = 2 * idx + 1;

		if (succ_1 > m_Vertex_cnt)
		{
			break;
		}
		else
		{
			if (succ_2 > m_Vertex_cnt)
			{
				if (m_Vertices[idx]->m_score < m_Vertices[succ_1]->m_score)
				{
					sVertex *vertex = m_Vertices[idx];
					m_Vertices[idx] = m_Vertices[succ_1];
					m_Vertices[succ_1] = vertex;

					m_Vertices[idx]->m_heap_idx = idx;
					m_Vertices[succ_1]->m_heap_idx = succ_1;
					idx = succ_1;
				}
				else
				{
					break;
				}
			}
			else
			{
				if (m_Vertices[succ_1]->m_score < m_Vertices[succ_2]->m_score)
				{
					if (m_Vertices[idx]->m_score < m_Vertices[succ_2]->m_score)
					{
						sVertex *vertex = m_Vertices[idx];
						m_Vertices[idx] = m_Vertices[succ_2];
						m_Vertices[succ_2] = vertex;
						
						m_Vertices[idx]->m_heap_idx = idx;
						m_Vertices[succ_2]->m_heap_idx = succ_2;
						idx = succ_2;
					}
					else
					{
						break;
					}
				}
				else
				{
					if (m_Vertices[idx]->m_score < m_Vertices[succ_1]->m_score)
					{
						sVertex *vertex = m_Vertices[idx];
						m_Vertices[idx] = m_Vertices[succ_1];
						m_Vertices[succ_1] = vertex;
						
						m_Vertices[idx]->m_heap_idx = idx;
						m_Vertices[succ_1]->m_heap_idx = succ_1;
						idx = succ_1;
					}
					else
					{
						break;
					}
				}
			}
		}
	}
}


/*----------------------------------------------------------------------------*/
// Global functions

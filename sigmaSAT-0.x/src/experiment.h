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
// experiment.h / 0.71-sky
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
// Experimental module of sigmaSAT solver.


#ifndef __EXPERIMENT_H__
#define __EXPERIMENT_H__

#include <list>
#include <set>

#include "system.h"


/*----------------------------------------------------------------------------*/
// Tyde definitions

struct sBinaryClause
{
	sBinaryClause(sLiteral *l1, sLiteral *l2)
	{
		if (l1 < l2)
		{
			m_l1 = l1;
			m_l2 = l2;
		}
		else
		{
			m_l1 = l2;
			m_l2 = l1;
		}
	}

        bool operator==(const sBinaryClause &clause) const
	{
		return (m_l1 == clause.m_l1 && m_l2 == clause.m_l2);
	}
        bool operator<(const sBinaryClause &clause) const
	{
		return (m_l1 < clause.m_l1 || (m_l1 == clause.m_l1 && m_l2 < clause.m_l2));
	}
	sLiteral *m_l1, *m_l2;
};


typedef std::set<sBinaryClause, std::less<sBinaryClause> > sBinaryClauses_set;


/*----------------------------------------------------------------------------*/
// sExternalClause class

class sExternalClause
{
public:
	sExternalClause();
	sExternalClause(int N_Literals);
	sExternalClause(const sClause &clause);
	sExternalClause(const sClause *clause);
	sExternalClause(const sExternalClause &clause);
	const sExternalClause& operator=(const sExternalClause &clause);
	~sExternalClause();

	bool is_Null(void) const;
	int calc_PreviousDepth(int current_depth);
	int calc_MaximumDepth(void);
	int combine_Clauses(const sExternalClause &clause_A, const sExternalClause &clause_B, sAssignment *current_assignment);

	void to_Screen(const sBooleanFormula *formula);

public:
	int m_N_Literals;
	sLiteral **m_Literals;
};


/*----------------------------------------------------------------------------*/
// sExperimentalBooleanFormula class

class sExperimentalBooleanFormula
  : public sBooleanFormula
{
public:
	bool solve_Experimental_1(void);
	bool solve_Experimental_1_rec(int depth);

	bool solve_Experimental_2(void);
	bool solve_Experimental_2_rec(int depth, sExternalClause &conflict_clause, int &bj_depth);

	sVariable* select_VariableExperimental_1(sVariable *hint);

	bool is_Conflicting(sLiteral *literal_1, sLiteral *literal_2, sIncrementalGraph *literal_conflict);
	int calc_ExcludedPairs(sClause **path, int path_length, sIncrementalGraph *literal_conflict, sClause *clause, int &N_pairs);
	int calc_ExcludedPairs(sClause **path, int start, int path_length, sIncrementalGraph *literal_conflict, sClause *clause, int &N_pairs);

	int calc_ExcludedPairs_sparse(sClause **path, int path_length, sIncrementalGraph *literal_conflict, sClause *clause, int &N_pairs, double ratio = 0.1);
	int calc_ExcludedPairs_sparse(sClause **path, int start, int path_length, sIncrementalGraph *literal_conflict, sClause *clause, int &N_pairs, double ratio = 0.1);

	int find_ConflictingPath_3(sClause *origin, sIncrementalGraph *literal_conflict, sClause **path, int length);
	int find_ConflictingPath_4(sClause *origin, sIncrementalGraph *literal_conflict, sClause **path, int length);
	int find_ConflictingPath_5(sClause *origin, sIncrementalGraph *literal_conflict, sClause **path, int length);
	int find_ConflictingPath_6(sClause *origin, sIncrementalGraph *literal_conflict, sClause **path, int length);

	int find_conflicting_path(sClause *origin, sClause **path, int length);	

	void increase_affect(sClause *clause, int increase);
	void reset_affects(void);
	void reset_path(sClause **path, int length) const;

	int calc_LiteralConflictIndex(const sLiteral *literal) const;

	int build_ClauseConflictGraph(sIncrementalGraph *graph);
	int build_LiteralConflictGraph(sIncrementalGraph *graph);

	int find_ConflictingPath(sClause *origin, sIncrementalGraph *clause_cnf_graph, sClause **path, int length);
	int build_OccurenceConflictGraph(sIncrementalGraph *graph);

	int build_sparse_conflict_graph(sIncrementalGraph *graph);
	int build_partial_sparse_conflict_graph(sIncrementalGraph *graph);
	int build_occurence_clustering_1(sIncrementalGraph *graph, sIncrementalClustering *clustering);
	int build_occurence_clustering_2(sIncrementalGraph *graph, sIncrementalClustering *clustering);
	int build_occurence_clustering_3(sIncrementalGraph *graph, sIncrementalClustering *clustering);
	int build_occurence_clustering_4(sIncrementalGraph *graph, sIncrementalClustering *clustering);

	void calc_ClusteringCapacity(sIncrementalGraph *graph, sIncrementalClustering *clustering);
	int calc_ClusterCapacity_1(sCluster *cluster, sIncrementalGraph *graph);
	int calc_ClusterCapacity_2(sCluster *cluster, sIncrementalGraph *graph);
	
	int calc_UndefinedClauses(void) const;
	int calc_UnassignedLiterals(void) const;
	int calc_UndefinedOccurences(const sLiteral *literal) const;

	int calc_SelectedClauses(void) const;
	int calc_SelectedOccurences(const sLiteral *literal) const;

	double calc_CumulativeThickness(sIncrementalClustering *clustering) const;

	int select_Undefined(void);
	int select_Arity(int arity);
	int select_Clustering(sIncrementalClustering *clustering);
	int select_Thickness(double threshold);
	int select_ArityThickness(int arity, double threshold);

	sVertex* get_best_vertex(sIncrementalGraph *graph);
	void update_NeighborsScore(sIncrementalGraph *graph, sVertex *vertex, sVertexHeap *vertex_heap);

	int build_ConflictPath(sClause **path, int length, sIncrementalGraph *graph, Clustering *clustering);
	int build_ConflictPath(sClause **path, int length, sIncrementalGraph *graph, Clustering *clustering, Layering *layering);
	int build_ConflictPath_1(sClause **path, int length, sIncrementalGraph *graph, Clustering *clustering);

	int build_ConflictPath_2(sClause **path, int length, sIncrementalGraph *literal_conflict, sIncrementalGraph *graph, Clustering *clustering);
	int build_ConflictPath_2(sClause **path, int length, sIncrementalGraph *literal_conflict, sIncrementalGraph *graph, Clustering *clustering, Layering *layering);

	void build_next_layer(int cluster_idx, int vertex_idx, sIncrementalGraph *graph, Clustering *clustering, Layering *layering);
	void build_next_layer_2(sIncrementalGraph *graph, Layering *layering, sVertex **buffer);
	void calculate_MaximumVisits(Layering *layering);

	void check_paths_SPC(Clustering *clustering, Layering *layering);
	bool check_path_SPC(sVertex *first_vertex, sVertex *last_vertex, Clustering *clustering, Layering *layering);

	void check_paths_APC(Clustering *clustering, Layering *layering);
	bool check_path_APC(sVertex *first_vertex, sVertex *last_vertex, Clustering *clustering, Layering *layering);

	void check_paths_BPC(Clustering *clustering, Layering *layering);
	bool check_path_BPC(sVertex *first_vertex, sVertex *last_vertex, Clustering *clustering, Layering *layering);
	bool check_path_BPC(sVertex *last_vertex, int c1, int c2,  Clustering *clustering, Layering *layering);

	void check_paths_EPC(Clustering *clustering);
	bool check_path_EPC(sVertex *first_vertex, sVertex *last_vertex, int last_cluster, Clustering *clustering);
	bool check_path_EPC(sVertex *last_vertex, int c1, int c2,  Clustering *clustering);

	void check_paths_LPC(Clustering *clustering, int limit);
	bool check_path_LPC(sVertex *first_vertex, sVertex *last_vertex, int last_cluster, Clustering *clustering, int limit);
	bool check_path_LPC(sVertex *last_vertex, int c1, int c2,  Clustering *clustering, int &limit_cnt, int limit);

	void check_paths_LPC_2(sIncrementalGraph *literal_conflict, Clustering *clustering, Layering *layering, int limit);
	bool check_path_LPC_2(sVertex *first_vertex, sVertex *last_vertex, int last_cluster, sIncrementalGraph *literal_conflict, Clustering *clustering, Layering *layering, int &limit_cnt, int limit);
	bool check_path_LPC_2(sVertex *last_vertex, int c1, int c2, sIncrementalGraph *literal_conflict, Clustering *clustering, Layering *layering, int &limit_cnt, int limit);

	void print_affects(void) const;
	void print_path(sClause **path, int length) const;
	void print_path_verbose(sClause **path, int length) const;

	void print_DIMACS(void) const;

public:
	sBinaryClauses_set m_binary_Clauses;
};


/*----------------------------------------------------------------------------*/
// sFlowMode_1 class

class sFlowModel_1
{
public:
	typedef struct
	{
		int m_offset;
		int m_occmax;
	} OccurenceInfo;

public:
	int init(sExperimentalBooleanFormula *formula);
	void destroy(void);
	int build_Network(int arity = 3, double thickness = 0.0);

	int calc_ClauseIndex(const sClause *clause) const;
	int calc_ClusterIndex(const sCluster *cluster) const;
	int calc_OccurenceIndex(const sCluster *cluster, int occ_idx) const;
	int calc_SourceIndex(void) const;
	int calc_SinkIndex(void) const;

	void to_Screen(void) const;
	void to_ScreenInfo(void) const;

public:
	sExperimentalBooleanFormula *m_formula;
	
	sIncrementalGraph m_conflict_graph;
	sIncrementalGraph m_network;
	sIncrementalClustering m_clustering;

	int m_clauses_offset;
	int m_clusters_offset;
	OccurenceInfo *m_occurence_Infos;
	int m_source_offset;
	int m_sink_offset;
};

/*----------------------------------------------------------------------------*/
// Global functions

void swap_affects(sClause *clause_1, sClause *clause_2);


#endif /* __EXPERIMENT_H__ */


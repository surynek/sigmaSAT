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
// experiment.cpp / 0.71-sky
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
// Experimental module of the sigmaSAT solver.

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "defs.h"
#include "config.h"
#include "experiment.h"


/*----------------------------------------------------------------------------*/
// Global variables

double LOWER_PATH_INDEPENDENT = sDEFAULT_LOWER_PATH_INDEPENDENT;
double UPPER_PATH_INDEPENDENT = sDEFAULT_UPPER_PATH_INDEPENDENT;
int ACCEPTABLE_LIMIT = sDEFAULT_ACCEPTABLE_LIMIT;


/*----------------------------------------------------------------------------*/
// sExternalClause class

sExternalClause::sExternalClause()
{
	m_Literals = NULL;
}


sExternalClause::sExternalClause(int N_Literals)
{
	m_Literals = new sLiteral*[N_Literals];
	assert(m_Literals != NULL);
	m_N_Literals = N_Literals;
}


sExternalClause::sExternalClause(const sClause *clause)
{
	if (clause == NULL)
	{
		m_Literals = NULL;
	}
	else
	{
		assert(clause->m_cL != 0);
		m_Literals = new sLiteral*[clause->m_cL];
		assert(m_Literals != NULL);

		for (int i = 0; i < clause->m_cL; ++i)
		{
			m_Literals[i] = clause->m_literals[i];
		}
		m_N_Literals = clause->m_cL;
	}
}


sExternalClause::sExternalClause(const sClause &clause)
{
	assert(clause.m_cL != 0);
	m_Literals = new sLiteral*[clause.m_cL];
	assert(m_Literals != NULL);

	for (int i = 0; i < clause.m_cL; ++i)
	{
		m_Literals[i] = clause.m_literals[i];
	}
	m_N_Literals = clause.m_cL;
}


sExternalClause::sExternalClause(const sExternalClause &clause)
{
	assert(clause.m_N_Literals != 0);
	m_Literals = new sLiteral*[clause.m_N_Literals];
	assert(m_Literals != NULL);

	for (int i = 0; i < clause.m_N_Literals; ++i)
	{
		m_Literals[i] = clause.m_Literals[i];
	}
	m_N_Literals = clause.m_N_Literals;
}


const sExternalClause& sExternalClause::operator=(const sExternalClause &clause)
{
	if (m_Literals != NULL)
	{
		delete m_Literals;
	}
	if (clause.m_Literals == NULL)
	{
		m_Literals = NULL;
	}
	else
	{
		assert(clause.m_N_Literals != 0);
		m_Literals = new sLiteral*[clause.m_N_Literals];
		assert(m_Literals != NULL);

		for (int i = 0; i < clause.m_N_Literals; ++i)
		{
			m_Literals[i] = clause.m_Literals[i];
		}
		m_N_Literals = clause.m_N_Literals;
	}

	return *this;
}


sExternalClause::~sExternalClause()
{
	if (m_Literals != NULL)
	{
		delete m_Literals;
	}
}


bool sExternalClause::is_Null(void) const
{
	return (m_Literals == NULL);
}


int sExternalClause::calc_PreviousDepth(int current_depth)
{
	int prev_depth = -1;

	for (int i = 0; i < m_N_Literals; ++i)
	{
		if (m_Literals[i]->m_variable->m_assignment != NULL)
		{
			int depth = m_Literals[i]->m_variable->m_assignment->m_depth;
			if (depth < current_depth && depth > prev_depth)
			{
				prev_depth = depth;
			}
		}
	}
	
	return prev_depth;
}


int sExternalClause::calc_MaximumDepth(void)
{
	int max_depth = -1;

	for (int i = 0; i < m_N_Literals; ++i)
	{
		int depth = m_Literals[i]->m_variable->m_assignment->m_depth;
		if (depth > max_depth)
		{
			max_depth = depth;
		}
	}

	return max_depth;
}


int sExternalClause::combine_Clauses(const sExternalClause &clause_A, const sExternalClause &clause_B, sAssignment *current_assignment)
{
	int N_Literals = 0;

	for (int i = 0; i < clause_A.m_N_Literals; ++i)
	{
		clause_A.m_Literals[i]->m_resolved = false;
	}
	for (int i = 0; i < clause_B.m_N_Literals; ++i)
	{
		clause_B.m_Literals[i]->m_resolved = false;
	}
	for (int i = 0; i < clause_A.m_N_Literals; ++i)
	{
		if (!clause_A.m_Literals[i]->m_resolved && clause_A.m_Literals[i]->m_variable->m_assignment < current_assignment && clause_A.m_Literals[i]->m_variable->m_assignment->m_depth > 0)
		{
			clause_A.m_Literals[i]->m_resolved = true;
			++N_Literals;
		}
	}
	for (int i = 0; i < clause_B.m_N_Literals; ++i)
	{
		if (!clause_B.m_Literals[i]->m_resolved && clause_B.m_Literals[i]->m_variable->m_assignment < current_assignment && clause_B.m_Literals[i]->m_variable->m_assignment->m_depth > 0)
		{
			clause_B.m_Literals[i]->m_resolved = true;
			++N_Literals;
		}
	}

	if (m_Literals != NULL)
	{
		delete m_Literals;
	}
	if (N_Literals > 0)
	{
		m_Literals = new sLiteral*[N_Literals];
		assert(m_Literals != NULL);

		int j = 0;

		for (int i = 0; i < clause_A.m_N_Literals; ++i)
		{
			clause_A.m_Literals[i]->m_resolved = false;
		}
		for (int i = 0; i < clause_B.m_N_Literals; ++i)
		{
			clause_B.m_Literals[i]->m_resolved = false;
		}
		for (int i = 0; i < clause_A.m_N_Literals; ++i)
		{
			if (!clause_A.m_Literals[i]->m_resolved && clause_A.m_Literals[i]->m_variable->m_assignment < current_assignment && clause_A.m_Literals[i]->m_variable->m_assignment->m_depth > 0)
			{
				clause_A.m_Literals[i]->m_resolved = true;
				m_Literals[j++] = clause_A.m_Literals[i];
			}
		}
		for (int i = 0; i < clause_B.m_N_Literals; ++i)
		{
			if (!clause_B.m_Literals[i]->m_resolved && clause_B.m_Literals[i]->m_variable->m_assignment < current_assignment && clause_B.m_Literals[i]->m_variable->m_assignment->m_depth > 0)
			{
				clause_B.m_Literals[i]->m_resolved = true;
				m_Literals[j++] = clause_B.m_Literals[i];
			}
		}
}
	else
	{
		m_Literals = NULL;
	}

	m_N_Literals = N_Literals;

	int bj_depth = -1;

	for (int i = 0; i < N_Literals; ++i)
	{
		int depth = m_Literals[i]->m_variable->m_assignment->m_depth;
		if (bj_depth < depth)
		{
			bj_depth = depth;
		}
	}

	return bj_depth;
}


void sExternalClause::to_Screen(const sBooleanFormula *formula)
{
	if (m_Literals != NULL)
	{
		for (int i = 0; i < m_N_Literals; ++i)
		{
			formula->print_literal(m_Literals[i]);
			if (m_Literals[i]->m_variable->m_assignment != NULL)
			{
				printf("(a:%ld,d:%d)", m_Literals[i]->m_variable->m_assignment - formula->m_Assignments, m_Literals[i]->m_variable->m_assignment->m_depth);
				printf("-%d ", m_Literals[i]->m_value);
			}
			else
			{
				printf("(nil)");
				printf("-%d ", m_Literals[i]->m_value);
			}
		}
		printf("0\n");
	}
	else
	{
		printf("NULL\n");
	}
}


/*----------------------------------------------------------------------------*/
// sExperimentalBooleanFormula class

static double total_RATIO = 0;
static int total_ITER = 0;
static int total_UNSATS = 0;
static int total_BACKTRACKS = 0;


bool sExperimentalBooleanFormula::solve_Experimental_1(void)
{
	if (m_conflicting)
	{
		return false;
	}
	else
	{
		if (!operate_PropagateUnit())
		{
			return false;
		}
		
		return solve_Experimental_1_rec(0);
	}
}


bool sExperimentalBooleanFormula::solve_Experimental_1_rec(int depth)
{
	sClause *head_clause_save = m_head_clause;
	sVariable *head_var_save = m_head_var;
	sAssignment *assignment_end_save = m_assignment_end;

	int result;

	for (int arity = 2; arity < 16; ++arity)
	{
		for (double thickness = 0.2; thickness < 3.0; thickness += 0.1)
		{
			sFlowModel_1 flow_model;
			result = flow_model.init(this);
			if (result < 0)
			{
				fprintf(stderr, "Unable to initialize flow model (result:%d)\n", result);
				return result;
			}
			
			flow_model.build_Network(arity, thickness);
			if (result < 0)
			{
				fprintf(stderr, "Unable to build flow network (result:%d)\n", result);
				return result;
			}

			sVertex *source = flow_model.m_network.get_Vertex(flow_model.calc_SourceIndex());
			sVertex *sink = flow_model.m_network.get_Vertex(flow_model.calc_SinkIndex());

			result = flow_model.m_network.compute_MaximumFlow(source, sink);
			if (result < 0)
			{
				printf("Cannot compute maximum flow (result:%d).\n", result);
				exit(-1);
			}
//	flow_model.m_network.print_DirectedFlow();
//	flow_model.to_ScreenInfo();
			int flow = sink->m_excess;
			int sat = calc_SelectedClauses();

			total_RATIO += (double)sat/flow;
			++total_ITER;
		
			printf("Depth = %d, arity = %d, thickness = %.3f ... clauses x flow: %d x %d: %.3f\n", depth, arity, thickness, sat, flow, (double)sat/flow);
			if (sat > flow)
			{
				printf("----------==========----------> UNSAT:%d ... BACKTRACKS:%d\n", ++total_UNSATS, total_BACKTRACKS);
				exit(0);
			}
			flow_model.destroy();
		}
	}
	printf("\n");

/*
	print_assignments(m_assignment_end);

	sIncrementalGraph graph;
	sIncrementalClustering clustering;
	int result = build_literal_conflict_graph(&graph);

	if (result < 0)
	{
		fprintf(stderr, "Unable to build occurence conflict graph (result:%d)\n", result);
		assert(false);
	}
	graph.print_flow_specific_nontrivial();

	result = build_occurence_clustering_2(&graph, &clustering);
	clustering.print_nontrivial();
	calc_ClusteringCapacity(&graph, &clustering);

	clustering.destroy();
	graph.destroy();

	printf("Depth:%d\n", depth);
*/
	while (calc_Value(*m_head_clause) == V_TRUE)
	{
		++m_head_clause;
		
		if (m_head_clause - m_clauses >= m_cC)
		{
			return true;
		}
	}
	sVariable *selected_variable = select_Variable_1(m_head_var);
	
	sLiteral **check_point = get_CheckPoint();
#ifdef STAT
	++STAT_DECISIONS;
#endif
	if (operate_SetPropagate_TRUE(selected_variable->m_positive))
	{
		if (solve_1_rec(depth + 1))
		{
			return true;
		}
		else
		{
			operate_UnsetUnpropagate(check_point);
		}
	}
	else
	{
		++total_BACKTRACKS;
		operate_UnsetUnpropagate(check_point);
	}
	
#ifdef STAT
	++STAT_DECISIONS;
#endif
	if (operate_SetPropagate_FALSE(selected_variable->m_positive))
	{
		if (solve_1_rec(depth + 1))
		{
			return true;
		}
		else
		{
			operate_UnsetUnpropagate(check_point);
		}
	}
	else
	{
		++total_BACKTRACKS;
		operate_UnsetUnpropagate(check_point);
	}
	
	m_head_clause = head_clause_save;
	m_head_var = head_var_save;
	m_assignment_end = assignment_end_save;

#ifdef STAT
	++STAT_BACKTRACKS;
#endif
	return false;
}


bool sExperimentalBooleanFormula::solve_Experimental_2(void)
{
	if (m_conflicting)
	{
		return false;
	}
	else
	{
		if (!operate_PropagateUnit())
		{
			return false;
		}
		
		int bj_depth;
		sExternalClause final_conflict;
		return solve_Experimental_2_rec(1, final_conflict, bj_depth);
	}
}


bool sExperimentalBooleanFormula::solve_Experimental_2_rec(int depth, sExternalClause &conflict_clause, int &bj_depth)
{
	sClause *head_clause_save = m_head_clause;
	sVariable *head_var_save = m_head_var;
	sAssignment *assignment_end_save = m_assignment_end;

	while (calc_Value(*m_head_clause) == V_TRUE)
	{
		++m_head_clause;
		
		if (m_head_clause - m_clauses >= m_cC)
		{
			return true;
		}
	}
	sVariable *selected_variable = select_Variable_1(m_head_var);
	sLiteral **check_point = get_CheckPoint();

#ifdef STAT
	++STAT_DECISIONS;
#endif
	printf("Decisive (%ld):", m_assignment_end - m_Assignments);
	print_literal(selected_variable->m_positive);
	printf("\n");
	printf("Depth:%d\n", depth);

	printf("  Depth A:%d\n", depth);

	sExternalClause *conflict_TRUE = new sExternalClause();
	printf("Propagating 1: %d\n", depth);

//	if (operate_SetPropagateAnalyze_TRUE(selected_variable->m_positive, conflict_TRUE, depth))
	{
		printf("\n");
		int bj_depth_rec;

		if (solve_Experimental_2_rec(depth + 1, *conflict_TRUE, bj_depth_rec))
		{
			return true;
		}
		else
		{
			printf("Pi 1\n");
			if (bj_depth_rec < depth)
			{
				conflict_TRUE->to_Screen(this);
				printf("A ---------> Backjumping\n");
				bj_depth = bj_depth_rec;
				conflict_clause = *conflict_TRUE;
				printf("X1\n");
				operate_UnsetUnpropagate(check_point);
				return false;
			}
			else
			{
				conflict_TRUE->to_Screen(this);
				printf("Pi 2:%ld\n", selected_variable - m_variables);

				printf("delta 0\n");
				sClause conflict_TRUE_ante;
				printf("delta 1\n");
//				conflict_TRUE_ante.init(*conflict_TRUE);
				printf("delta 2\n");
				sExternalClause *conflict_FALSE = new sExternalClause();
				printf("Depth X:%d\n", depth);

				sLiteral **literal_to_set = NULL;
				sLiteral **literal = conflict_TRUE_ante.m_literals;
				while (*literal != NULL)
				{
					if ((*literal)->m_variable->m_assignment == NULL)
					{
						literal_to_set = literal;
						break;
					}
					literal++;
				}

				printf("Literal to set:");
				print_literal(*literal_to_set);
				printf("\n");

//				exit(0);

//				if (operate_SetPropagateAnalyze_FALSE(*literal_to_set, &conflict_TRUE_ante, *conflict_FALSE, depth))
				{
//					exit(0);
					printf("Ksi 1\n");

					if (solve_Experimental_2_rec(depth + 1, *conflict_FALSE, bj_depth_rec))
					{
						return true;
					}
					else
					{
						printf("Rho 1\n");
						if (bj_depth_rec < depth)
						{
							printf("B ---------> Backjumping\n");
							bj_depth = bj_depth_rec;

							operate_UnsetUnpropagate(check_point);
							conflict_clause = *conflict_TRUE;
							printf("X1\n");
							return false;
						}
						else
						{
							printf("Rho 2\n");
							conflict_FALSE->to_Screen(this);
							int prev_depth = conflict_FALSE->calc_PreviousDepth(depth);
							printf("Previous depth 2:%d\n", prev_depth);

							conflict_clause = *conflict_FALSE;
							bj_depth = prev_depth;
							
							operate_UnsetUnpropagate(check_point);

							return false;
						}
					}
				}
//				else
				{
//					exit(0);
					printf("Ksi 2\n");

					conflict_FALSE->to_Screen(this);
					int prev_depth = conflict_FALSE->calc_PreviousDepth(depth);
					printf("Previous depth 3:%d\n", prev_depth);

					conflict_clause = *conflict_TRUE;
					bj_depth = prev_depth;

					operate_UnsetUnpropagate(check_point);

					return false;
				}
			}
		}
	}
//	else
	{
		conflict_TRUE->to_Screen(this);
		int prev_depth = conflict_TRUE->calc_PreviousDepth(depth);
		printf("Previous depth 4:%d\n", prev_depth);

//	push_ExternalClause(conflict_TRUE, prev_depth);
		conflict_clause = *conflict_TRUE;
		bj_depth = prev_depth;

		printf("chi 1\n");
		operate_UnsetUnpropagate(check_point);
		printf("chi 2\n");
/*
		m_head_clause = head_clause_save;
		m_head_var = head_var_save;
		m_assignment_end = assignment_end_save;
*/
		return false;
	}

/*
	printf("\n");
	printf("  --------->Depth alpha:%d\n", depth);

	conflict_TRUE->to_Screen(this);
	int prev_depth = conflict_TRUE->calc_PreviousDepth(depth);
	printf("Previous depth:%d\n", prev_depth);

	push_ExternalClause(conflict_TRUE, prev_depth);

	bool contains = false;

	for (int i = 0; i < conflict_TRUE->m_N_Literals; ++i)
	{
		if (conflict_TRUE->m_Literals[i]->m_variable == selected_variable)
		{
			contains = true;
			break;
		}
	}

	printf("Selected variable:%ld\n", selected_variable - m_variables);

	if (!contains)
	{
		printf("!!! ---> NOT CONTAINING <--- !!!\n");
		exit(0);
	}
	else
	{
		printf("!!! ---> containing <--- !!!\n");
	}

	sClause conflict_TRUE_ante;
	conflict_TRUE_ante.init(*conflict_TRUE);

	operate_UnsetUnpropagate(check_point);

#ifdef STAT
	++STAT_DECISIONS;
#endif

	sExternalClause *conflict_FALSE = new sExternalClause();

	printf("Propagating 2: %d\n", prev_depth);
	if (operate_SetPropagateAnalyze_AnteFALSE(selected_variable->m_positive, &conflict_TRUE_ante, *conflict_FALSE, prev_depth))
	{
		printf("\n");
		conflict_TRUE->to_Screen(this);
		int bj_depth_rec;

		if (solve_2_rec(depth + 1, *conflict_FALSE, bj_depth_rec))
		{
			return true;
		}
		else
		{
			if (bj_depth_rec < depth)
			{
				printf("---------> Backjumping\n");
				bj_depth = bj_depth_rec;

				operate_UnsetUnpropagate(check_point);
				conflict_clause = *conflict_FALSE;
				printf("X2\n");
				conflict_TRUE_ante.destroy();
				return false;
			}
		}
	}
	printf("\n");

	printf("  Depth beta:%d\n", depth);
	conflict_FALSE->to_Screen(this);

	int max_depth = conflict_FALSE->calc_MaximumDepth();
	printf("Previous depth (backjump):%d\n", prev_depth);

	operate_UnsetUnpropagate(check_point);

	bj_depth = max_depth;
	printf("bj_depth:%d\n", bj_depth);
	exit(0);

	conflict_clause = *conflict_FALSE;
*/
	m_head_clause = head_clause_save;
	m_head_var = head_var_save;
	m_assignment_end = assignment_end_save;

#ifdef STAT
	++STAT_BACKTRACKS;
#endif
//	conflict_TRUE_ante.destroy();

	return false;
}


bool sExperimentalBooleanFormula::is_Conflicting(sLiteral *literal_1, sLiteral *literal_2, sIncrementalGraph *literal_conflict)
{
	return (is_Complementary(literal_1, literal_2) || literal_conflict->is_Adjacent(literal_1->m_vertex_idx, literal_2->m_vertex_idx));
}


int sExperimentalBooleanFormula::calc_ExcludedPairs(sClause **path, int path_length, sIncrementalGraph *literal_conflict, sClause *clause, int &N_pairs)
{
	return calc_ExcludedPairs(path, path_length - 1, path_length, literal_conflict, clause, N_pairs);
}


int sExperimentalBooleanFormula::calc_ExcludedPairs(sClause **path, int start, int path_length, sIncrementalGraph *literal_conflict, sClause *clause, int &N_pairs)
{
	int N_excluded_pairs = 0;
	N_pairs = 0;
	
	for (int p = start; p < path_length; ++p)
	{
		sClause *path_clause = path[p];
		sLiteral **lit = clause->m_literals;
		
		while (*lit != NULL)
		{
			sLiteral **path_lit = path_clause->m_literals;
			
			while (*path_lit != NULL)
			{
				if (is_Conflicting(*lit, *path_lit, literal_conflict))
				{
					++N_excluded_pairs;
				}
				++path_lit;
			}
			++lit;
		}
		N_pairs += clause->m_cL * path_clause->m_cL;
	}
	
	return N_excluded_pairs;
}


int sExperimentalBooleanFormula::calc_ExcludedPairs_sparse(sClause **path, int path_length, sIncrementalGraph *literal_conflict, sClause *clause, int &N_pairs, double ratio)
{
	return calc_ExcludedPairs_sparse(path, path_length - 1, path_length, literal_conflict, clause, N_pairs, ratio);
}


int sExperimentalBooleanFormula::calc_ExcludedPairs_sparse(sClause **path, int start, int path_length, sIncrementalGraph *literal_conflict, sClause *clause, int &N_pairs, double ratio)
{
	int N_excluded_pairs = 0;
	N_pairs = 0;

	int N_checks = 0;

	for (int p = start; p < path_length; ++p)
	{
		sClause *path_clause = path[p];
		N_checks += path_clause->m_cL * clause->m_cL;
	}
	N_checks *= ratio;

	for (int i = 0; i < N_checks; ++i)
	{
		int rnd_cl = rand() % (path_length - start) + start;

		sClause *path_clause = path[rnd_cl];
		
		int rnd_lit_1 = rand() % clause->m_cL;
		int rnd_lit_2 = rand() % path_clause->m_cL;

		sLiteral *lit = clause->m_literals[rnd_lit_1];
		sLiteral *path_lit = path_clause->m_literals[rnd_lit_2];

		if (is_Conflicting(lit, path_lit, literal_conflict))
		{
			++N_excluded_pairs;
		}
		++N_pairs;
	}
	
	return N_excluded_pairs;
}

int sExperimentalBooleanFormula::find_ConflictingPath_3(sClause *origin, sIncrementalGraph *literal_conflict, sClause **path, int length)
{
	path[0] = origin;
	
	int l;
	int total_N_excluded_pairs = 0;
	int total_N_pairs = 0;
	
	for (l = 1; l < length; ++l)
	{
		origin->m_affects = -1;
		sLiteral **literal = origin->m_literals;
		
		while (*literal != NULL)
		{
			if (!is_Assigned(**literal))
			{
				sLiteral **check_point = get_CheckPoint();
				operate_SetPropagate_TRUE(*literal);

				sLiteral **chng_literal = check_point + 1;
				
				while (chng_literal < m_last_chng)
				{
					ONode *onode = (*chng_literal)->m_cmpl->m_occs;
					
					while (onode != NULL)
					{
						if (onode->m_clause->m_affects >= 0)
						{
							increase_affect(onode->m_clause, 1);
						}
						onode = onode->m_next;
					}
					++chng_literal;
				}
				operate_UnsetUnpropagate(check_point);
			}
			++literal;
		}

		if (m_cA > 0)
		{
			sClause *best_aff_clause = m_affcls[1];
			
			int best_N_pairs;
			int best_N_excluded_pairs = calc_ExcludedPairs(path, l, literal_conflict, best_aff_clause, best_N_pairs);

			double best_ratio = (double)(total_N_excluded_pairs + best_N_excluded_pairs) / (double)(total_N_pairs + best_N_pairs);

			double path_tightness = 1.0 / (double)length;
			
			for (int a = 2; a < m_cA; ++a)
			{
				sClause *aff_clause = m_affcls[a];
				
				int N_pairs;
				int N_excluded_pairs = calc_ExcludedPairs(path, l, literal_conflict, aff_clause, N_pairs);
				
				double ratio = (double)(total_N_excluded_pairs + N_excluded_pairs) / (double)(total_N_pairs + N_pairs);
				
				if (ratio < sLOWER_PATH_FACTOR * path_tightness)
				{
					if (best_ratio < sLOWER_PATH_FACTOR * path_tightness)
					{
						if (ratio > best_ratio)
						{
							best_aff_clause = aff_clause;
							best_N_pairs = N_pairs;
							best_N_excluded_pairs = N_excluded_pairs;
							best_ratio = ratio;
						}
					}
				}
				else if (ratio > sUPPER_PATH_FACTOR * path_tightness)
				{
					if (best_ratio > sUPPER_PATH_FACTOR * path_tightness)
					{
						if (ratio < best_ratio)
						{
							best_aff_clause = aff_clause;
							best_N_pairs = N_pairs;
							best_N_excluded_pairs = N_excluded_pairs;
							best_ratio = ratio;
						}
					}
				}
				else if (ratio >= sLOWER_PATH_FACTOR * path_tightness && ratio <= sUPPER_PATH_FACTOR * path_tightness)
				{
					if (best_ratio >= sLOWER_PATH_FACTOR * path_tightness && best_ratio <= sUPPER_PATH_FACTOR * path_tightness)
					{
						if (ratio > best_ratio)
						{
							best_aff_clause = aff_clause;
							best_N_pairs = N_pairs;
							best_N_excluded_pairs = N_excluded_pairs;
							best_ratio = ratio;
						}
					}
					else
					{
						best_aff_clause = aff_clause;
						best_N_pairs = N_pairs;
						best_N_excluded_pairs = N_excluded_pairs;
						best_ratio = ratio;
					}
				}
			}
			best_N_excluded_pairs = calc_ExcludedPairs(path, 0, l, literal_conflict, best_aff_clause, best_N_pairs);

			origin = path[l] = best_aff_clause;
			total_N_pairs += best_N_pairs;
			total_N_excluded_pairs += best_N_excluded_pairs;
		}
		else
		{
			int rnd_clause = rand() % m_cC;
			sClause *clause = &m_clauses[rnd_clause];
			
			while (clause->m_affects < 0)
			{
				rnd_clause = (rnd_clause + 1) % m_cC;
				clause = &m_clauses[rnd_clause];
			}
			origin = path[l] = clause;
			
			int N_pairs;
			int N_excluded_pairs = calc_ExcludedPairs(path, 0, l, literal_conflict, clause, N_pairs);
			
			total_N_pairs += N_pairs;
			total_N_excluded_pairs += N_excluded_pairs;
		}
		reset_affects();
	}
	while (--l >= 0)
	{
		path[l]->m_affects = 0;
	}
	double final_ratio = (double)total_N_excluded_pairs / total_N_pairs;
//	fprintf(stderr, "Final ratio: %.3f,%d\n", final_ratio, length);
	
	return length;
}


int sExperimentalBooleanFormula::find_ConflictingPath_4(sClause *origin, sIncrementalGraph *literal_conflict, sClause **path, int length)
{
	path[0] = origin;
	
	int l;
	int total_N_excluded_pairs = 0;
	int total_N_pairs = 0;
	
	for (l = 1; l < length; ++l)
	{
		origin->m_affects = -1;
		sLiteral **literal = origin->m_literals;
		
		while (*literal != NULL)
		{
			if (!is_Assigned(**literal))
			{
				sLiteral **check_point = get_CheckPoint();
				operate_SetPropagate_TRUE(*literal);

				sLiteral **chng_literal = check_point + 1;
				
				while (chng_literal < m_last_chng)
				{
					ONode *onode = (*chng_literal)->m_cmpl->m_occs;
					
					while (onode != NULL)
					{
						if (onode->m_clause->m_affects >= 0)
						{
							increase_affect(onode->m_clause, 1);
						}
						onode = onode->m_next;
					}
					++chng_literal;
				}
				operate_UnsetUnpropagate(check_point);
			}
			++literal;
		}

		if (m_cA > 0)
		{
			sClause *best_aff_clause = m_affcls[1];
			
			int best_N_pairs;
			int best_N_excluded_pairs = calc_ExcludedPairs(path, l, literal_conflict, best_aff_clause, best_N_pairs);
			
			double best_ratio = (double)(total_N_excluded_pairs + best_N_excluded_pairs) / (double)(total_N_pairs + best_N_pairs);
			
			for (int a = 2; a < m_cA; ++a)
			{
				sClause *aff_clause = m_affcls[a];
				
				int N_pairs;
				int N_excluded_pairs = calc_ExcludedPairs(path, l, literal_conflict, aff_clause, N_pairs);
				
				double ratio = (double)(total_N_excluded_pairs + N_excluded_pairs) / (double)(total_N_pairs + N_pairs);
				
				if (ratio > best_ratio)
				{
					best_aff_clause = aff_clause;
					best_N_pairs = N_pairs;
					best_N_excluded_pairs = N_excluded_pairs;
					best_ratio = ratio;
				}
			}
			best_N_excluded_pairs = calc_ExcludedPairs(path, 0, l, literal_conflict, best_aff_clause, best_N_pairs);

			origin = path[l] = best_aff_clause;
			total_N_pairs += best_N_pairs;
			total_N_excluded_pairs += best_N_excluded_pairs;
		}
		else
		{
			int rnd_clause = rand() % m_cC;
			sClause *clause = &m_clauses[rnd_clause];
			
			while (clause->m_affects < 0)
			{
				rnd_clause = (rnd_clause + 1) % m_cC;
				clause = &m_clauses[rnd_clause];
			}
			origin = path[l] = clause;
			
			int N_pairs;
			int N_excluded_pairs = calc_ExcludedPairs(path, 0, l, literal_conflict, clause, N_pairs);
			
			total_N_pairs += N_pairs;
			total_N_excluded_pairs += N_excluded_pairs;
		}
		reset_affects();
	}
	while (--l >= 0)
	{
		path[l]->m_affects = 0;
	}
	double final_ratio = (double)total_N_excluded_pairs / total_N_pairs;
//	fprintf(stderr, "Final ratio: %.3f,%d\n", final_ratio, length);
	
	return length;
}


int sExperimentalBooleanFormula::find_ConflictingPath_5(sClause *origin, sIncrementalGraph *literal_conflict, sClause **path, int length)
{
	path[0] = origin;

	int N_path_literals = path[0]->m_cL;	
	int l;
	int total_N_excluded_pairs = 0;
	int total_N_pairs = 0;

	double path_factor = path[0]->m_cL;
	double final_ratio, final_independent;
	
	for (l = 1; l < length; ++l)
	{
		origin->m_affects = -1;
		sLiteral **literal = origin->m_literals;
		
		while (*literal != NULL)
		{
			if (!is_Assigned(**literal))
			{
				sLiteral **check_point = get_CheckPoint();
				operate_SetPropagate_TRUE(*literal);

				sLiteral **chng_literal = check_point + 1;
				
				while (chng_literal < m_last_chng)
				{
					ONode *onode = (*chng_literal)->m_cmpl->m_occs;
					
					while (onode != NULL)
					{
						increase_affect(onode->m_clause, 1);
						onode = onode->m_next;
					}
					++chng_literal;
				}
				operate_UnsetUnpropagate(check_point);
			}
			++literal;
		}

//		double rnd = (double)rand() / RAND_MAX;

		if (m_cA > 0)
		{
			sClause *best_aff_clause = m_affcls[1];

			int best_N_pairs = N_path_literals * best_aff_clause->m_cL;
			int best_N_excluded_pairs = best_aff_clause->m_affects;

//			int best_N_pairs;
//			int best_N_excluded_pairs = calc_ExcludedPairs(path, 0, l, literal_conflict, best_aff_clause, best_N_pairs);

/*			int sparse_N_pairs;
			int sparse_N_excluded_pairs = calc_ExcludedPairs_sparse(path, 0, l, literal_conflict, best_aff_clause, sparse_N_pairs, 2.0);
			fprintf(stderr, "Sparse: %.3f x %.3f\n", (double)best_N_excluded_pairs / best_N_pairs, (double)sparse_N_excluded_pairs / sparse_N_pairs);
*/
			double best_ratio = (double)(total_N_excluded_pairs + best_N_excluded_pairs) / (double)(total_N_pairs + best_N_pairs);
			double best_factor = path_factor * best_aff_clause->m_cL;
			double best_independent = best_factor * pow(1 - best_ratio, l * (l - 1) / 2);

			for (int a = 2; a < m_cA; ++a)
			{
				sClause *aff_clause = m_affcls[a];

				int N_pairs = N_path_literals * aff_clause->m_cL;
				int N_excluded_pairs = aff_clause->m_affects;

//				int N_pairs;
//				int N_excluded_pairs = calc_ExcludedPairs(path, 0, l, literal_conflict, aff_clause, N_pairs);
				
				double ratio = (double)(total_N_excluded_pairs + N_excluded_pairs) / (double)(total_N_pairs + N_pairs);
				double factor = path_factor * aff_clause->m_cL;
				double independent = factor * pow(1 - ratio, l * (l - 1) / 2);			
				
				if (independent < LOWER_PATH_INDEPENDENT)
				{
					if (best_independent < LOWER_PATH_INDEPENDENT)
					{
						if (independent > best_independent)
						{
							best_aff_clause = aff_clause;
							best_N_pairs = N_pairs;
							best_N_excluded_pairs = N_excluded_pairs;
							best_ratio = ratio;
							best_factor = factor;
							best_independent = independent;
						}
					}
				}
				else if (independent > UPPER_PATH_INDEPENDENT)
				{
					if (best_independent > UPPER_PATH_INDEPENDENT)
					{
						if (independent < best_independent)
						{
							best_aff_clause = aff_clause;
							best_N_pairs = N_pairs;
							best_N_excluded_pairs = N_excluded_pairs;
							best_ratio = ratio;
							best_factor = factor;
							best_independent = independent;
						}
					}
				}
				else if (independent >= LOWER_PATH_INDEPENDENT && independent <= UPPER_PATH_INDEPENDENT)
				{
					if (best_independent >= LOWER_PATH_INDEPENDENT && best_independent <= UPPER_PATH_INDEPENDENT)
					{
						if (independent < best_independent)
						{
							best_aff_clause = aff_clause;
							best_N_pairs = N_pairs;
							best_N_excluded_pairs = N_excluded_pairs;
							best_ratio = ratio;
							best_factor = factor;
							best_independent = independent;
						}
					}
					else
					{
						best_aff_clause = aff_clause;
						best_N_pairs = N_pairs;
						best_N_excluded_pairs = N_excluded_pairs;
						best_ratio = ratio;
						best_factor = factor;
						best_independent = independent;
					}
				}
			}

			best_N_excluded_pairs = calc_ExcludedPairs(path, 0, l, literal_conflict, best_aff_clause, best_N_pairs);

			origin = path[l] = best_aff_clause;
			total_N_pairs += best_N_pairs;
			total_N_excluded_pairs += best_N_excluded_pairs;

//			fprintf(stderr, "Best independent:%.3f\n", best_independent);
		}
		else
		{
			int rnd_clause = rand() % m_cC;
			sClause *clause = &m_clauses[rnd_clause];
			
			while (clause->m_affects < 0)
			{
				rnd_clause = (rnd_clause + 1) % m_cC;
				clause = &m_clauses[rnd_clause];
			}
			origin = path[l] = clause;
			
			int N_pairs;
			int N_excluded_pairs = calc_ExcludedPairs(path, 0, l, literal_conflict, clause, N_pairs);
			
			total_N_pairs += N_pairs;
			total_N_excluded_pairs += N_excluded_pairs;
		}
		path[l]->m_in_path = true;
		N_path_literals += path[l]->m_cL;

		path_factor *= path[l]->m_cL;
	}
	final_ratio = (double)total_N_excluded_pairs / (double)total_N_pairs;
	final_independent = path_factor * pow(1.0 - final_ratio, l * (l - 1) / 2);

	fprintf(stderr, "Final ratio: %.3f, Final independent:%.3f, Length:%d\n", final_ratio, final_independent, length);

	reset_affects();

	while (--l >= 0)
	{
		path[l]->m_in_path = false;
	}
	
	return length;
}


int sExperimentalBooleanFormula::find_ConflictingPath_6(sClause *origin, sIncrementalGraph *literal_conflict, sClause **path, int length)
{
	int l;
	path[0] = origin;
	path[0]->m_in_path = true;
	
	int N_path_literals = path[0]->m_cL;
	double domain_factor = path[0]->m_cL;
	double edge_factor = 1.0;
	int complement_exponent = length * (length - 1) / 2;

	double best_ratio, best_independent;
	double final_ratio, final_independent;
	sClause *best_aff_clause;
	
	for (l = 1; l < length; ++l)
	{
		origin->m_affects = -1;
		sLiteral **literal = origin->m_literals;
		
		while (*literal != NULL)
		{
			if (!is_Assigned(**literal))
			{
				sLiteral **check_point = get_CheckPoint();
				operate_SetPropagate_TRUE(*literal);

				sLiteral **chng_literal = check_point;
				
				while (chng_literal < m_last_chng)
				{
					ONode *onode = (*chng_literal)->m_cmpl->m_occs;
					
					while (onode != NULL)
					{
						increase_affect(onode->m_clause, 1);
						onode = onode->m_next;
					}
					++chng_literal;
				}
				operate_UnsetUnpropagate(check_point);
			}
			++literal;
		}

		if (m_cA > 0)
		{
			best_aff_clause = m_affcls[1];

			int best_N_pairs = N_path_literals * best_aff_clause->m_cL;
			int best_N_excluded_pairs = best_aff_clause->m_affects;

			best_ratio = (double)best_N_excluded_pairs / (double)best_N_pairs;
			best_independent = domain_factor * edge_factor * pow(1 - best_ratio, complement_exponent) * pow(best_aff_clause->m_cL, length - l);

			for (int a = 2; a < m_cA; ++a)
			{
				sClause *aff_clause = m_affcls[a];
				
				int N_pairs = N_path_literals * aff_clause->m_cL;
				int N_excluded_pairs = aff_clause->m_affects;

				double ratio = (double)N_excluded_pairs / (double)N_pairs;
				double independent = domain_factor * edge_factor * pow(1 - ratio, complement_exponent) * pow(aff_clause->m_cL, length - l);

				if (!aff_clause->m_in_path)
				{
					if (best_aff_clause->m_in_path)
					{
						best_aff_clause = aff_clause;
						best_N_pairs = N_pairs;
						best_N_excluded_pairs = N_excluded_pairs;
						best_ratio = ratio;
						best_independent = independent;
					}
					else
					{
						if (independent < LOWER_PATH_INDEPENDENT)
						{
							if (best_independent < LOWER_PATH_INDEPENDENT)
							{
								if (independent > best_independent)
								{
									best_aff_clause = aff_clause;
									best_N_pairs = N_pairs;
									best_N_excluded_pairs = N_excluded_pairs;
									best_ratio = ratio;
									best_independent = independent;
								}
							}
						}
						else if (independent > UPPER_PATH_INDEPENDENT)
						{
							if (best_independent > UPPER_PATH_INDEPENDENT)
							{
								if (independent < best_independent)
								{
									best_aff_clause = aff_clause;
									best_N_pairs = N_pairs;
									best_N_excluded_pairs = N_excluded_pairs;
									best_ratio = ratio;
									best_independent = independent;
								}
							}
						}
						else if (independent >= LOWER_PATH_INDEPENDENT && independent <= UPPER_PATH_INDEPENDENT)
						{
							if (best_independent >= LOWER_PATH_INDEPENDENT && best_independent <= UPPER_PATH_INDEPENDENT)
							{
/*
								if (independent < best_independent)
								{
									best_aff_clause = aff_clause;
									best_N_pairs = N_pairs;
									best_N_excluded_pairs = N_excluded_pairs;
									best_ratio = ratio;
									best_independent = independent;
								}
*/
							}
							else
							{
								best_aff_clause = aff_clause;
								best_N_pairs = N_pairs;
								best_N_excluded_pairs = N_excluded_pairs;
								best_ratio = ratio;
								best_independent = independent;
							}
						}
					}
				}
			}
			origin = path[l] = best_aff_clause;
		}
		else
		{
			int rnd_clause = rand() % m_cC;
			sClause *clause = &m_clauses[rnd_clause];
			
			while (clause->m_in_path)
			{
				rnd_clause = (rnd_clause + 1) % m_cC;
				clause = &m_clauses[rnd_clause];
			}
			origin = path[l] = clause;

			int best_N_pairs = N_path_literals * clause->m_cL;
			int best_N_excluded_pairs = 0;

			best_ratio = (double)best_N_excluded_pairs / (double)best_N_pairs;
			best_independent = domain_factor * edge_factor * pow(1 - best_ratio, complement_exponent) * pow(clause->m_cL, length - l);
			
			int N_pairs;
			int N_excluded_pairs = calc_ExcludedPairs(path, 0, l, literal_conflict, clause, N_pairs);
		}
//		fprintf(stderr, "Best independent:%.3f\t ratio:%.3f domain_pow:%.3f\n", best_independent, best_ratio, pow(best_aff_clause->m_cL, length - l));
//		fprintf(stderr, "  compl_exp:%d\tdomain_fact:%.3f\tedge_fact:%.3f\n", complement_exponent, domain_factor, edge_factor);

		path[l]->m_in_path = true;
		N_path_literals += path[l]->m_cL;

		domain_factor *= path[l]->m_cL;
		complement_exponent -= l;
		edge_factor *= pow(1 - best_ratio, l);
		
	}
	final_ratio = best_ratio;
	final_independent = best_independent;

//	fprintf(stderr, "Final ratio: %.3f, Final independent:%.3f, Length:%d\n", final_ratio, final_independent, length);
//	fprintf(stderr, "\n");
	reset_affects();

	while (--l >= 0)
	{
		path[l]->m_in_path = false;
	}
	return length;
}


int sExperimentalBooleanFormula::find_conflicting_path(sClause *origin, sClause **path, int length)
{
	for (int i = 1; i <= m_V; ++i)
	{
		m_P_literals[i].m_single = true;
		m_N_literals[i].m_single = true;
	}
	
	path[0] = origin;
	
	int l;

	double q = (double)random() / RAND_MAX;

	for (l = 1; l < length; ++l)
	{
		origin->m_affects = -1;
		sLiteral **literal = origin->m_literals;
		
		while (*literal != NULL)
		{
			if (!is_Assigned(**literal))
			{
				sLiteral **check_point = get_CheckPoint();
				/*bool op = */operate_SetPropagate_TRUE(*literal);

//				if (op)
				{
					sLiteral **chng_literal = check_point;// + 1;
				
					while (chng_literal < m_last_chng)
					{
						(*chng_literal)->m_single = false;
						(*chng_literal)->m_cmpl->m_single = false;
						ONode *onode = (*chng_literal)->m_cmpl->m_occs;
					
						while (onode != NULL)
						{
							if (onode->m_clause->m_affects >= 0)
							{
								increase_affect(onode->m_clause, 1);
							}
							onode = onode->m_next;
						}
						++chng_literal;
					}
				}
/*
				else
				{
//					printf("Warning: Unit clause arised.\n");
				}
*/
				operate_UnsetUnpropagate(check_point);
			}
			++literal;
		}

		if (q < 0.95)
		{
			if (m_cA > 0)
			{
/*
				int pairs = origin->m_cL * m_affcls[1]->m_cL;
				double pair_perm = (double)(pairs - m_affcls[1]->m_affects) / pairs;
				path_perm *= pair_perm;
*/			
				double p = (double)random() / RAND_MAX;

				if (p < 0.9)
				{
					int selected = random() % m_cA + 1;
					int best = selected;
					int best_sngls = 0;

					int sngls = 0;
					for (int ll = 0; ll < m_affcls[best]->m_cL; ++ll)
					{
						if (m_affcls[best]->m_literals[ll]->m_single)
						{
							++sngls;
						}
					}
					best_sngls = sngls;

					for (int ii = 0; ii < 256; ++ii)
					{
/*
						if (origin->m_cL > m_affcls[selected]->m_cL)
						{
							break;
						}
*/
/*				       
						if (m_affcls[selected]->m_cL < m_affcls[best]->m_cL)
						{
						best = selected;
						}
*/

						int sngls = 0;
						for (int ll = 0; ll < m_affcls[selected]->m_cL; ++ll)
						{
							if (m_affcls[selected]->m_literals[ll]->m_single)
							{
								++sngls;
							}
						}
						if (sngls < best_sngls && origin->m_cL <= m_affcls[selected]->m_cL)
						{
							best = selected;
							best_sngls = sngls;
						}
						else
						{
							if (sngls == best_sngls && selected < best)
							{
								best = selected;
								best_sngls = sngls;
							}
						}
						selected = random() % m_cA + 1;
					}
					selected = best;
					origin = path[l] = m_affcls[selected];
				}
				else
				{
					origin = path[l] = m_affcls[1];
				}
				reset_affects();
			}
			else
			{

				int rnd_clause = rand() % m_cC;

				while (true)
				{
					sClause *clause = &m_clauses[rnd_clause];
					if (clause->m_affects >= 0)
					{
						origin = path[l] = &m_clauses[rnd_clause];
						break;
					}
					rnd_clause = (rnd_clause + 1) % m_cC;
				}

				path[l] = NULL;
				reset_affects();
				
				int len = l;
				
				while (--l >= 0)
				{
					path[l]->m_affects = 0;
				}
				return len;
			}
		}
		else
		{
			if (m_cA > 0)
			{
				origin = path[l] = m_affcls[1];
			}
			else
			{
				int rnd_clause = rand() % m_cC;

				while (true)
				{
					sClause *clause = &m_clauses[rnd_clause];
					if (clause->m_affects >= 0)
					{
						origin = path[l] = &m_clauses[rnd_clause];
						break;
					}
					rnd_clause = (rnd_clause + 1) % m_cC;
				}
			}
			reset_affects();
		}
	}
	while (--l >= 0)
	{
		path[l]->m_affects = 0;
	}
	
	return length;
}


sVariable* sExperimentalBooleanFormula::select_VariableExperimental_1(sVariable *sUNUSED(hint))
{
    sIncrementalGraph graph;

    int result = build_LiteralConflictGraph(&graph);
    if (result < 0)
    {
	fprintf(stderr, "Unable to build occurence conflict graph (result:%d)\n", result);
	assert(false);
    }
    // graph.print_flow_specific();
    /*
	result = build_occurence_clustering_2(&graph, &clustering);
	clustering.print_nontrivial();
	calc_ClusteringCapacity(&graph, &clustering);
	clustering.destroy();
    */
    sVertex *highest_vertex = NULL;
    int N_Vertices = graph.get_VertexCount();

    for (int idx = 0; idx < N_Vertices; ++idx)
    {
	sVertex *vertex = graph.get_Vertex(idx);
	if (vertex->m_literal->m_variable->m_positive->m_value == V_UNDEF)
	{
	    if (highest_vertex == NULL)
	    {
		highest_vertex = vertex;
	    }
	    else
	    {
		if (vertex->m_N_Neighs > highest_vertex->m_N_Neighs)
		{
		    highest_vertex = vertex;
		}
	    }
	}
    }
    /*
    printf("Value A:%p\n", highest_vertex->m_literal->m_variable);
    printf("Value B:%p\n", highest_vertex->m_literal->m_variable->m_positive);
    printf("Value C:%d\n", highest_vertex->m_literal->m_variable->m_positive->m_value);
    */
    assert(highest_vertex->m_literal->m_variable->m_positive->m_value == V_UNDEF);
    sVariable *variable = highest_vertex->m_literal->m_variable;
    printf("Degree: %d\n", highest_vertex->m_N_Neighs);
    graph.destroy();

    return variable;
}


void sExperimentalBooleanFormula::increase_affect(sClause *clause, int increase)
{
	int aff, prev_aff;
	
	if (clause->m_sort == NULL)
	{
		clause->m_sort = &m_affcls[++m_cA];
		m_affcls[m_cA] = clause;
		clause->m_affects = increase;
		
		aff = m_cA;
		prev_aff = aff / 2;
	}
	else
	{
		clause->m_affects += increase;
		
		aff = clause->m_sort - m_affcls;
		prev_aff = aff / 2;
	}
	
	while (prev_aff >= 1)
	{
		if (m_affcls[aff]->m_affects > m_affcls[prev_aff]->m_affects)
		{
			swap_affects(m_affcls[aff], m_affcls[prev_aff]);
			aff = prev_aff;
			prev_aff = aff / 2;
		}
		else
		{
			break;
		}
	}
}


void sExperimentalBooleanFormula::reset_affects(void)
{
	for (int i = 1; i <= m_cA; ++i)
	{
		sClause *clause = m_affcls[i];
		
		clause->m_sort = NULL;
		clause->m_affects = 0;
	}
	m_cA = 0;
}


void sExperimentalBooleanFormula::reset_path(sClause **path, int length) const
{
	for (int i = 0; i < length; ++i)
	{
		path[i]->m_affects = 0;
	}
}


int sExperimentalBooleanFormula::calc_LiteralConflictIndex(const sLiteral *literal) const
{
	int lit_diff = literal - literal->m_base;
	return (literal->m_base == m_P_literals) ? lit_diff : lit_diff + m_V + 1;
}


int sExperimentalBooleanFormula::build_ClauseConflictGraph(sIncrementalGraph *graph)
{
	int result;

	reset_affects();

	if ((result = graph->init(this, m_cC, sEDGE_CHUNK, sNEIGHBOR_CHUNK)) < 0)
	{
		return -1;
	}
	if ((result = graph->add_Vertices(m_cC)) < 0)
	{
		return result;
	}
/*
	for (int i = 0; i < m_cC; ++i)
	{
		sClause *origin = &m_clauses[i];
		sLiteral **literal = origin->m_literals;

		while (*literal != NULL)
		{
			sLiteral **check_point = get_CheckPoint();
			bool op = operate_SetPropagate_TRUE(*literal);

			if (op)
			{
				sLiteral **chng_literal = check_point;
				
				while (chng_literal < m_last_chng)
				{
					ONode *onode = (*chng_literal)->m_cmpl->m_occs;
					
					while (onode != NULL)
					{
						graph->increase_EdgeWeight(origin - m_clauses, onode->m_clause - m_clauses, 1);
						onode = onode->m_next;
					}
					++chng_literal;
				}
			}
			operate_UnsetUnpropagate(check_point);
			++literal;
		}
	}
*/
	for (int i = 1; i <= m_V; ++i)
	{
		sLiteral *literal = &m_P_literals[i];

		if (!is_Assigned(*literal))
		{
			sLiteral **check_point = get_CheckPoint();
			bool op = operate_SetPropagate_TRUE(literal);
			
			if (op)
			{
				sLiteral **chng_literal = check_point;

				while (chng_literal < m_last_chng)
				{				
					ONode *onode_1 = literal->m_occs;

					while (onode_1 != NULL)
					{
						ONode *onode_2 = (*chng_literal)->m_cmpl->m_occs;

						while (onode_2 != NULL)
						{
							if (onode_1->m_clause < onode_2->m_clause)
							{
								graph->increase_EdgeWeight(onode_1->m_clause - m_clauses, onode_2->m_clause - m_clauses, 1);
							}
							onode_2 = onode_2->m_next;
						}
						onode_1 = onode_1->m_next;
					}
					++chng_literal;
				}
			}
			operate_UnsetUnpropagate(check_point);
		}
	}
	for (int i = 1; i <= m_V; ++i)
	{
		sLiteral *literal = &m_N_literals[i];

		if (!is_Assigned(*literal))
		{
			sLiteral **check_point = get_CheckPoint();
			bool op = operate_SetPropagate_TRUE(literal);
			
			if (op)
			{
				sLiteral **chng_literal = check_point;

				while (chng_literal < m_last_chng)
				{				
					ONode *onode_1 = literal->m_occs;

					while (onode_1 != NULL)
					{
						ONode *onode_2 = (*chng_literal)->m_cmpl->m_occs;

						while (onode_2 != NULL)
						{
							if (onode_1->m_clause < onode_2->m_clause)
							{
								graph->increase_EdgeWeight(onode_1->m_clause - m_clauses, onode_2->m_clause - m_clauses, 1);
							}
							onode_2 = onode_2->m_next;
						}
						onode_1 = onode_1->m_next;
					}
					++chng_literal;
				}
			}
			operate_UnsetUnpropagate(check_point);
		}
	}

	for (int v = 0; v < m_cC; ++v)
	{
		sVertex *vertex = graph->get_Vertex(v);

		if (vertex->m_N_Neighs > 1)
		{
			bool change;
			sNeighborNode *last = NULL;

			do {
				change = false;
			
				sNeighborNode *nnode_1 = vertex->m_first_nnode;
				sNeighborNode *nnode_2 = nnode_1->m_next;

				sNeighborNode *last_upd = nnode_2;

				while (nnode_2 != last)
				{
					if (nnode_1->m_edge->m_weight < nnode_2->m_edge->m_weight)
					{
						sEdge *tmp_edge = nnode_1->m_edge;
						sVertex *tmp_vertex = nnode_1->m_vertex;

						nnode_1->m_edge = nnode_2->m_edge;
						nnode_1->m_vertex = nnode_2->m_vertex;

						nnode_2->m_edge = tmp_edge;
						nnode_2->m_vertex = tmp_vertex;

						last_upd = nnode_2;
						change = true;
					}
					nnode_1 = nnode_1->m_next;
					nnode_2 = nnode_2->m_next;
				}
				last = last_upd;

			} while (change);
		}
	}

	return 0;
}


int sExperimentalBooleanFormula::build_LiteralConflictGraph(sIncrementalGraph *graph)
{
	int result;

	if ((result = graph->init(this, 2 * m_V + 2, sEDGE_CHUNK, sNEIGHBOR_CHUNK)) < 0)
	{
		return -1;
	}

	if ((result = graph->add_Vertices(2 * m_V + 2)) < 0)
	{
		return result;
	}

	for (int i = 1; i <= m_V; ++i)
	{
		sLiteral *p_literal = &m_P_literals[i];
		p_literal->m_vertex_idx = calc_LiteralConflictIndex(p_literal);

		sLiteral *n_literal = &m_N_literals[i];
		n_literal->m_vertex_idx = calc_LiteralConflictIndex(n_literal);
		
	}

	for (int i = 1; i <= m_V; ++i)
	{
		sLiteral *literal = &m_P_literals[i];

		if (!is_Assigned(*literal))
		{
			sLiteral **check_point = get_CheckPoint();
			operate_SetPropagate_TRUE(literal);
			
			sLiteral **chng_literal = check_point + 1;

			while (chng_literal < m_last_chng)
			{
				if (!is_Complementary(literal, (*chng_literal)->m_cmpl))
				{
					int idx_1 = literal->m_vertex_idx;
					int idx_2 = (*chng_literal)->m_cmpl->m_vertex_idx;
				
					if (idx_1 < idx_2)
					{
						if ((result = graph->add_Edge(idx_1, idx_2)) < 0)
						{
							return result;
						}
					}
				}
				++chng_literal;
			}
			operate_UnsetUnpropagate(check_point);
		}
	}

	for (int i = 1; i <= m_V; ++i)
	{
		sLiteral *literal = &m_N_literals[i];

		if (!is_Assigned(*literal))
		{
			sLiteral **check_point = get_CheckPoint();
			operate_SetPropagate_TRUE(literal);

			sLiteral **chng_literal = check_point + 1;
			
			while (chng_literal < m_last_chng)
			{
				if (!is_Complementary(literal, (*chng_literal)->m_cmpl))
				{
					int idx_1 = literal->m_vertex_idx;
					int idx_2 = (*chng_literal)->m_cmpl->m_vertex_idx;
				
					if (idx_1 < idx_2)
					{
						if ((result = graph->add_Edge(idx_1, idx_2)) < 0)
						{
							return result;
						}
					}
				}
				++chng_literal;
			}
			operate_UnsetUnpropagate(check_point);
		}
	}

	return 0;
}


int sExperimentalBooleanFormula::find_ConflictingPath(sClause *origin, sIncrementalGraph *clause_cnf_graph, sClause **path, int length)
{
	path[0] = origin;
	origin->m_affects = 1;
	
	int l;

	for (l = 1; l < length; ++l)
	{
		sVertex *vertex = clause_cnf_graph->get_Vertex(origin - m_clauses);
		sNeighborNode *best_nnode = vertex->m_first_nnode;

		if (best_nnode != NULL)
		{
			sNeighborNode *nnode = best_nnode->m_next;

			while (nnode != NULL)
			{
				if (nnode->m_vertex->m_component == 0)
				{
					best_nnode = nnode;
					break;
				}
				nnode = nnode->m_next;
			}

			if (best_nnode->m_vertex->m_component == 0)
			{ 
				origin = path[l] = m_clauses + best_nnode->m_vertex->m_idx;
				origin->m_affects = 1;
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}
	reset_path(path, l);

	return l;
}


int sExperimentalBooleanFormula::build_OccurenceConflictGraph(sIncrementalGraph *graph)
{
	int result;

	if ((result = graph->init(this, m_cLO, sEDGE_CHUNK, sNEIGHBOR_CHUNK)) < 0)
	{
		return -1;
	}
	graph->add_Vertices(m_cLO);

	for (int i = 0; i <= m_V; ++i)
	{
		sLiteral &p_literal = m_P_literals[i];
		sLiteral &n_literal = m_N_literals[i];

		p_literal.m_voccs = NULL;
		n_literal.m_voccs = NULL;
	}

	int id = 0;

	for (int i = 0; i < m_cC; ++i)
	{
		sClause *clause = &m_clauses[i];
		sLiteral **literal = clause->m_literals;

		while (*literal != NULL)
		{
			sVertex *vertex = graph->get_Vertex(id++);

			vertex->m_literal = *literal++;
			vertex->m_cluster = -1;
			vertex->m_clause = clause;
			vertex->m_score = 0;

			VONode &vo_node = graph->m_vo_nodes[graph->m_cVO++];
			vo_node.m_next = vertex->m_literal->m_voccs;
			vo_node.m_vertex = vertex;
			vertex->m_literal->m_voccs = &vo_node;
		}
	}

	for (int i = 1; i <= m_V; ++i)
	{
		sLiteral *literal = &m_P_literals[i];

		sLiteral **check_point = get_CheckPoint();
		operate_SetPropagate_TRUE(literal);
			
		sLiteral **chng_literal = check_point;

		while (chng_literal < m_last_chng)
		{
			VONode *vo_node_1 = literal->m_voccs;

			while (vo_node_1 != NULL)
			{
				VONode *vo_node_2 = (*chng_literal)->m_cmpl->m_voccs;

				while (vo_node_2 != NULL)
				{
					if (vo_node_1->m_vertex->m_idx < vo_node_2->m_vertex->m_idx)
					{
						if ((result = graph->add_Edge(vo_node_1->m_vertex->m_idx, vo_node_2->m_vertex->m_idx)) < 0)
						{
							return result;
						}
					}
					vo_node_2 = vo_node_2->m_next;
				}
				vo_node_1 = vo_node_1->m_next;
			}
			++chng_literal;
		}
		operate_UnsetUnpropagate(check_point);
	}

	for (int i = 1; i <= m_V; ++i)
	{
		sLiteral *literal = &m_N_literals[i];

		sLiteral **check_point = get_CheckPoint();
		operate_SetPropagate_TRUE(literal);
			
		sLiteral **chng_literal = check_point;

		while (chng_literal < m_last_chng)
		{
			VONode *vo_node_1 = literal->m_voccs;

			while (vo_node_1 != NULL)
			{
				VONode *vo_node_2 = (*chng_literal)->m_cmpl->m_voccs;

				while (vo_node_2 != NULL)
				{
					if (vo_node_1->m_vertex->m_idx < vo_node_2->m_vertex->m_idx)
					{
						if ((result = graph->add_Edge(vo_node_1->m_vertex->m_idx, vo_node_2->m_vertex->m_idx)) < 0)
						{
							return result;
						}
					}
					vo_node_2 = vo_node_2->m_next;
				}
				vo_node_1 = vo_node_1->m_next;
			}
			++chng_literal;
		}
		operate_UnsetUnpropagate(check_point);
	}

	return 0;
}


int sExperimentalBooleanFormula::build_sparse_conflict_graph(sIncrementalGraph *graph)
{
	int result;

	if ((result = graph->init(this, m_cLO, sEDGE_CHUNK, sNEIGHBOR_CHUNK)) < 0)
	{
		return -1;
	}
	graph->add_Vertices(m_cLO);

	for (int i = 0; i <= m_V; ++i)
	{
		sLiteral &p_literal = m_P_literals[i];
		sLiteral &n_literal = m_N_literals[i];

		p_literal.m_voccs = NULL;
		n_literal.m_voccs = NULL;
	}

	int id = 0;

	for (int i = 0; i < m_cC; ++i)
	{
		sClause *clause = &m_clauses[i];
		sLiteral **literal = clause->m_literals;

		while (*literal != NULL)
		{
			sVertex *vertex = graph->get_Vertex(id++);

			vertex->m_literal = *literal++;
			vertex->m_cluster = -1;
			vertex->m_clause = clause;
			vertex->m_score = 0;

			VONode &vo_node = graph->m_vo_nodes[graph->m_cVO++];
			vo_node.m_next = vertex->m_literal->m_voccs;
			vo_node.m_vertex = vertex;
			vertex->m_literal->m_voccs = &vo_node;
		}
	}

	for (int i = 0; i <= m_V; ++i)
	{
		sLiteral *literal = &m_P_literals[i];

		sLiteral **check_point = get_CheckPoint();
		operate_SetPropagate_TRUE(literal);
			
		sLiteral **chng_literal = check_point + 1;

		while (chng_literal < m_last_chng)
		{
			VONode *vo_node_1 = literal->m_voccs;

			while (vo_node_1 != NULL)
			{
				if (!is_Complementary(literal, *chng_literal))
				{
					VONode *vo_node_2 = (*chng_literal)->m_cmpl->m_voccs;
					
					while (vo_node_2 != NULL)
					{
						if (vo_node_1->m_vertex->m_idx < vo_node_2->m_vertex->m_idx)
						{
							if ((result = graph->add_Edge(vo_node_1->m_vertex->m_idx, vo_node_2->m_vertex->m_idx)) < 0)
							{
								return result;
							}
						}
						vo_node_2 = vo_node_2->m_next;
					}
				}
				vo_node_1 = vo_node_1->m_next;
			}
			++chng_literal;
		}
		operate_UnsetUnpropagate(check_point);
		
		++literal;
	}

	for (int i = 0; i <= m_V; ++i)
	{
		sLiteral *literal = &m_N_literals[i];

		sLiteral **check_point = get_CheckPoint();
		operate_SetPropagate_TRUE(literal);
			
		sLiteral **chng_literal = check_point + 1;

		while (chng_literal < m_last_chng)
		{
			VONode *vo_node_1 = literal->m_voccs;

			while (vo_node_1 != NULL)
			{
				if (!is_Complementary(literal, *chng_literal))
				{
					VONode *vo_node_2 = (*chng_literal)->m_cmpl->m_voccs;

					while (vo_node_2 != NULL)
					{
						if (vo_node_1->m_vertex->m_idx < vo_node_2->m_vertex->m_idx)
						{
							if ((result = graph->add_Edge(vo_node_1->m_vertex->m_idx, vo_node_2->m_vertex->m_idx)) < 0)
							{
								return result;
							}
						}
						vo_node_2 = vo_node_2->m_next;
					}
				}
				vo_node_1 = vo_node_1->m_next;
			}
			++chng_literal;
		}
		operate_UnsetUnpropagate(check_point);
			
		++literal;
	}

	for (int id = 0; id < m_cLO; ++id)
	{
		sVertex *vertex = graph->get_Vertex(id);
		vertex->m_score = graph->calc_VertexScore_2(vertex);
	}

//	graph->print_flow_specific();

	return 0;
}


int sExperimentalBooleanFormula::build_partial_sparse_conflict_graph(sIncrementalGraph *graph)
{
	int result;

	if ((result = graph->init(this, m_cLO, sEDGE_CHUNK, sNEIGHBOR_CHUNK)) < 0)
	{
		return -1;
	}
	graph->add_Vertices(m_cLO);

	for (int i = 0; i <= m_V; ++i)
	{
		sLiteral &p_literal = m_P_literals[i];
		sLiteral &n_literal = m_N_literals[i];

		p_literal.m_voccs = NULL;
		n_literal.m_voccs = NULL;
	}

	int id = 0;

	for (int i = 0; i < m_cC; ++i)
	{		
		sClause *clause = &m_clauses[i];
		if (!is_Satisfied(*clause))
		{			
			sLiteral **literal = clause->m_literals;
			
			while (*literal != NULL)
			{
				if (!is_Assigned(**literal))
				{
					sVertex *vertex = graph->get_Vertex(id++);

					vertex->m_literal = *literal;
					vertex->m_cluster = -1;
					vertex->m_clause = clause;
					vertex->m_score = 0;
					
					VONode &vo_node = graph->m_vo_nodes[graph->m_cVO++];
					vo_node.m_next = vertex->m_literal->m_voccs;
					vo_node.m_vertex = vertex;
					vertex->m_literal->m_voccs = &vo_node;
				}
				++literal;
			}
		}
	}

	for (int i = 0; i <= m_V; ++i)
	{
		sLiteral *literal = &m_P_literals[i];

		if (!is_Assigned(*literal))
		{
			sLiteral **check_point = get_CheckPoint();
			operate_SetPropagate_TRUE(literal);
			
			sLiteral **chng_literal = check_point + 1;

			while (chng_literal < m_last_chng)
			{
				VONode *vo_node_1 = literal->m_voccs;

				while (vo_node_1 != NULL)
				{
					if (!is_Complementary(literal, *chng_literal))
					{
						VONode *vo_node_2 = (*chng_literal)->m_cmpl->m_voccs;
					
						while (vo_node_2 != NULL)
						{
							if (vo_node_1->m_vertex->m_idx < vo_node_2->m_vertex->m_idx)
							{
/*
								printf("Adding Edge:");
								print_literal(literal);
								printf(" ");
								print_literal(*chng_literal);
								printf("\n");
*/
								if ((result = graph->add_Edge(vo_node_1->m_vertex->m_idx, vo_node_2->m_vertex->m_idx)) < 0)
								{
									return result;
								}
							}
							vo_node_2 = vo_node_2->m_next;
						}
					}
					vo_node_1 = vo_node_1->m_next;
				}
				++chng_literal;
			}
			operate_UnsetUnpropagate(check_point);
		}
		++literal;
	}

	for (int i = 0; i <= m_V; ++i)
	{
		sLiteral *literal = &m_N_literals[i];

		if (!is_Assigned(*literal))
		{
			sLiteral **check_point = get_CheckPoint();
			operate_SetPropagate_TRUE(literal);
			
			sLiteral **chng_literal = check_point + 1;
			
			while (chng_literal < m_last_chng)
			{
				VONode *vo_node_1 = literal->m_voccs;
				
				while (vo_node_1 != NULL)
				{
					if (!is_Complementary(literal, *chng_literal))
					{
						VONode *vo_node_2 = (*chng_literal)->m_cmpl->m_voccs;
						
						while (vo_node_2 != NULL)
						{
							if (vo_node_1->m_vertex->m_idx < vo_node_2->m_vertex->m_idx)
							{
								if ((result = graph->add_Edge(vo_node_1->m_vertex->m_idx, vo_node_2->m_vertex->m_idx)) < 0)
								{
									return result;
								}
							}
							vo_node_2 = vo_node_2->m_next;
						}
					}
					vo_node_1 = vo_node_1->m_next;
				}
				++chng_literal;
			}
			operate_UnsetUnpropagate(check_point);
		}
		++literal;
	}

	for (int id = 0; id < m_cLO; ++id)
	{
		sVertex *vertex = graph->get_Vertex(id);
		vertex->m_score = graph->calc_VertexScore_2(vertex);
	}

//	graph->print_flow_specific();

	return 0;
}


int sExperimentalBooleanFormula::build_occurence_clustering_1(sIncrementalGraph *graph, sIncrementalClustering *clustering)
{
	int result;

	if ((result = clustering->init(this, graph, sCLUSTER_CHUNK, graph->m_cV)) < 0)
	{
		return result;
	}

	sVertexHeap vertex_heap;

	if ((result = vertex_heap.init(graph->m_cV)) < 0)
	{
		return result;
	}

	int n_Vertices = graph->get_VertexCount();

	for (int i = 0; i < n_Vertices; ++i)
	{
		sVertex *vertex = graph->get_Vertex(i);
		vertex_heap.insert_Vertex(vertex);
	}
//	vertex_heap.print();	

	int cluster_idx = 0;

	while (true)
	{
		sVertex *best_vertex = vertex_heap.get_Min();
//		sVertex *best_vertex = get_best_vertex(graph);
//		printf("%p x %p\n", best_vertex_1, best_vertex);
/*
		if (best_vertex_1->m_score != best_vertex->m_score)
		{
			printf(" <--: %d,%d\n", best_vertex_1->m_score, best_vertex->m_score);
			vertex_heap.print();
			break;
		}
		else
		{
			printf("\n");
		}
*/
		if (best_vertex != NULL)
		{
			int N_cluster_Verices = graph->calc_ClusterNeighbors(best_vertex) + 1;
			clustering->add_cluster(N_cluster_Verices);
			sCluster *cluster = clustering->get_cluster(cluster_idx);
			cluster->m_N_Vertices = N_cluster_Verices;

			cluster->m_Vertices[0] = best_vertex;
			best_vertex->m_cluster = cluster_idx;

			int vertex_idx = 1;

			sNeighborNode *neighbor_node = best_vertex->m_first_nnode;
//			int n_check = 0;
			while (neighbor_node != NULL)
			{
				if (neighbor_node->m_vertex->m_cluster < 0)
				{
					cluster->m_Vertices[vertex_idx++] = neighbor_node->m_vertex;
					neighbor_node->m_vertex->m_cluster = cluster_idx;
//					n_check++;
				}
				neighbor_node = neighbor_node->m_next;
			}
/*
			if (n_check != best_vertex->m_score)
			{
				printf("<***>: %d,%d\n", n_check, best_vertex->m_score);
			}
			else
			{
				printf("%d,%d\n", n_check, best_vertex->m_score);
			}
*/
			for (int i = 0; i < cluster->m_N_Vertices; ++i)
			{
				update_NeighborsScore(graph, cluster->m_Vertices[i], &vertex_heap);
				vertex_heap.remove_Vertex(cluster->m_Vertices[i]);
			}
			++cluster_idx;
		}
		else
		{
			break;
		}
	}

//	graph->print_flow_specific();
//	clustering->print();

	return 0;
}


int sExperimentalBooleanFormula::build_occurence_clustering_2(sIncrementalGraph *graph, sIncrementalClustering *clustering)
{
	int result;
	int stamp = 1;

	if ((result = clustering->init(this, graph, sCLUSTER_CHUNK, graph->m_cV)) < 0)
	{
		return result;
	}

	int cluster_idx = 0;

	for (int i = 0; i < graph->get_VertexCount(); ++i)
	{
		sVertex *vertex = graph->get_Vertex(i);
		if (vertex->m_cluster < 0)
		{
			int N_cluster_Verices = graph->calc_ComponentSize(vertex, stamp);
			++stamp;
/*
			if (N_cluster_Verices > 1)
			{
				printf("Component size (%d): %d\n", i, N_cluster_Verices);
			}
*/			
			clustering->add_cluster(N_cluster_Verices);
			sCluster *cluster = clustering->get_cluster(cluster_idx);
			cluster->m_N_Vertices = N_cluster_Verices;

			graph->clusterize_Component(cluster, 0, vertex, stamp);
			++stamp;
			++cluster_idx;
		}
	}

	return 0;
}


int sExperimentalBooleanFormula::build_occurence_clustering_3(sIncrementalGraph *graph, sIncrementalClustering *clustering)
{
	int result;
	int stamp = 1;

	if ((result = clustering->init(this, graph, sCLUSTER_CHUNK, graph->m_cV)) < 0)
	{
		return result;
	}

	int cluster_idx = 0;

	while (true)
	{
		sVertex *highest_vertex = NULL;

		for (int i = 0; i < graph->get_VertexCount(); ++i)
		{
			sVertex *vertex = graph->get_Vertex(i);
			if (vertex->m_cluster < 0)
			{
				if (highest_vertex == NULL)
				{
					highest_vertex = vertex;
				}
				else
				{
					if (highest_vertex->m_N_Neighs < vertex->m_N_Neighs)
					{
						highest_vertex = vertex;
					}
				}
			}
		}
		if (highest_vertex == NULL)
		{
			break;
		}
		else
		{
			int N_cluster_Verices = graph->calc_DenseComponentSize(highest_vertex, stamp);
			++stamp;
/*
			if (N_cluster_Verices > 1)
			{
				printf("Component size: %d\n", N_cluster_Verices);
			}
*/
			clustering->add_cluster(N_cluster_Verices);
			sCluster *cluster = clustering->get_cluster(cluster_idx);
			cluster->m_N_Vertices = N_cluster_Verices;

			graph->clusterize_DenseComponent(cluster, 0, highest_vertex, stamp);
			++stamp;
			++cluster_idx;
		}
	}

	return 0;
}


int sExperimentalBooleanFormula::build_occurence_clustering_4(sIncrementalGraph *graph, sIncrementalClustering *clustering)
{
	int result;
	int stamp = 1;

	if ((result = clustering->init(this, graph, sCLUSTER_CHUNK, graph->m_cV)) < 0)
	{
		return result;
	}

	int cluster_idx = 0;

	while (true)
	{
		sVertex *highest_vertex = NULL;

		for (int i = 0; i < graph->get_VertexCount(); ++i)
		{
			sVertex *vertex = graph->get_Vertex(i);
			if (vertex->m_cluster < 0)
			{
				if (highest_vertex == NULL)
				{
					highest_vertex = vertex;
				}
				else
				{
					if (highest_vertex->m_N_Neighs < vertex->m_N_Neighs)
					{
						highest_vertex = vertex;
					}
				}
			}
		}
		if (highest_vertex == NULL)
		{
			break;
		}
		else
		{
			int N_cluster_Verices = graph->calc_CliqueSize(highest_vertex, stamp);
			++stamp;
/*
			if (N_cluster_Verices > 1)
			{
				printf("Component size: %d\n", N_cluster_Verices);
			}
*/
			clustering->add_cluster(N_cluster_Verices);
			sCluster *cluster = clustering->get_cluster(cluster_idx);
			cluster->m_N_Vertices = N_cluster_Verices;

			graph->clusterize_Clique(cluster, 0, highest_vertex, stamp);
			++stamp;
			++cluster_idx;
		}
	}

	return 0;
}


void sExperimentalBooleanFormula::calc_ClusteringCapacity(sIncrementalGraph *graph, sIncrementalClustering *clustering)
{
	int last_id = clustering->get_ClusterCount();

	for (int id = 0; id < last_id; ++id)
	{
		sCluster *cluster = clustering->get_cluster(id);
		if (cluster->m_N_Vertices > 1)
		{
			cluster->m_capacity = calc_ClusterCapacity_1(cluster, graph);

			for (int i = 0; i < cluster->m_N_Vertices; ++i)
			{
				int capacity = calc_ClusterCapacity_2(cluster, graph);
				if (capacity < cluster->m_capacity)
				{
					cluster->m_capacity = capacity;
				}
			}
		}
		else
		{
			cluster->m_capacity = 1;
		}
	}
}


int sExperimentalBooleanFormula::calc_ClusterCapacity_1(sCluster *cluster, sIncrementalGraph *graph)
{
	if (cluster->m_N_Vertices > 2)
	{
//		printf("Calculating capacity:");
	}
	int prev_capacity = 0;
	for (int i = 0; i < cluster->m_N_Vertices; ++i)
	{
		sVertex *vertex_1 = cluster->m_Vertices[i];
		int max_capacity = 0;

		for (int j = 0; j < i; ++j)
		{
			sVertex *vertex_2 = cluster->m_Vertices[j];

			if (!is_Complementary(vertex_1->m_literal, vertex_2->m_literal) && graph->get_Edge(vertex_1->m_idx, vertex_2->m_idx) == NULL)
			{
				if (max_capacity < vertex_2->m_capacity)
				{
					max_capacity = vertex_2->m_capacity;
				}
			}
		}
		vertex_1->m_capacity = sMAX(max_capacity + 1, prev_capacity);
		prev_capacity = vertex_1->m_capacity;
		if (cluster->m_N_Vertices > 2)
		{
//			printf("%d ", vertex_1->m_capacity);
		}
	}
	if (cluster->m_N_Vertices > 2)
	{
//		printf("\n");
	}

	return cluster->m_Vertices[cluster->m_N_Vertices - 1]->m_capacity;
}


int sExperimentalBooleanFormula::calc_ClusterCapacity_2(sCluster *cluster, sIncrementalGraph *graph)
{
	int *ordering = (int*)malloc(cluster->m_N_Vertices * sizeof(int));
	for (int i = 0; i < cluster->m_N_Vertices; ++i)
	{
		ordering[i] = i;
	}
	for (int j = 0; j < cluster->m_N_Vertices * cluster->m_N_Vertices; ++j)
	{
		int i1 = rand() % cluster->m_N_Vertices;
		int i2 = rand() % cluster->m_N_Vertices;

		int o = ordering[i1];
		ordering[i1] = ordering[i2];
		ordering[i2] = o;
	}
//	printf("Calculating capacity:");
	int prev_capacity = 0;
	for (int i = 0; i < cluster->m_N_Vertices; ++i)
	{
		sVertex *vertex_1 = cluster->m_Vertices[ordering[i]];
		int max_capacity = 0;

		for (int j = 0; j < i; ++j)
		{
			sVertex *vertex_2 = cluster->m_Vertices[ordering[j]];

			if (!is_Complementary(vertex_1->m_literal, vertex_2->m_literal) && graph->get_Edge(vertex_1->m_idx, vertex_2->m_idx) == NULL)
			{
				if (max_capacity < vertex_2->m_capacity)
				{
					max_capacity = vertex_2->m_capacity;
				}
			}
		}
		vertex_1->m_capacity = sMAX(max_capacity + 1, prev_capacity);
		prev_capacity = vertex_1->m_capacity;
//		printf("%d ", vertex_1->m_capacity);
	}
//	printf("\n");
	free(ordering);

	return cluster->m_Vertices[ordering[cluster->m_N_Vertices - 1]]->m_capacity;
}


int sExperimentalBooleanFormula::calc_UndefinedClauses(void) const
{
	int undefs = 0;

	for (int i = 0; i < m_C; ++i)
	{
		if (calc_Value(m_clauses[i]) == V_UNDEF)
		{
			++undefs;
		}
	}
	return undefs;
}


int sExperimentalBooleanFormula::calc_UndefinedOccurences(const sLiteral *literal) const
{
	int undefs = 0;
	ONode *onode = literal->m_occs;

	while (onode != NULL)
	{
		if (calc_Value(*onode->m_clause) == V_UNDEF)
		{
			++undefs;
		}
		onode = onode->m_next;
	}
	return undefs;
}


int sExperimentalBooleanFormula::calc_SelectedClauses(void) const
{
	int selects = 0;

	for (int i = 0; i < m_C; ++i)
	{
		if (m_clauses[i].m_selected && calc_Value(m_clauses[i]) == V_UNDEF)
		{
			++selects;
		}
	}
	return selects;
}


int sExperimentalBooleanFormula::calc_SelectedOccurences(const sLiteral *literal) const
{
	int selects = 0;
	ONode *onode = literal->m_occs;

	while (onode != NULL)
	{
		if (onode->m_clause->m_selected && calc_Value(*onode->m_clause) == V_UNDEF)
		{
			++selects;
		}
		onode = onode->m_next;
	}
	return selects;
}


double sExperimentalBooleanFormula::calc_CumulativeThickness(sIncrementalClustering *clustering) const
{
	double cumulative_thickness = 0.0;

	for (int i = 0; i < m_C; ++i)
	{
		m_clauses[i].m_thickness = 0.0;
	}
	int N_Clusters = clustering->get_ClusterCount();

	for (int i = 0; i < N_Clusters; ++i)
	{
		sCluster *cluster = clustering->get_cluster(i);
		double thickness = 1.0 / (double)cluster->m_N_Vertices;

		for (int k = 0; k < cluster->m_N_Vertices; ++k)
		{
			sLiteral *literal = cluster->m_Vertices[k]->m_literal;
			ONode *onode = literal->m_occs;
				
			while (onode != NULL)
			{
				onode->m_clause->m_thickness += thickness;
				cumulative_thickness += thickness;
				onode = onode->m_next;
			}
		}
	}
/*
	printf("Cumulative thickness: %.3f\n", cumulative_thickness);
	for (int i = 0; i < m_C; ++i)
	{
		if (calc_Value(m_clauses[i]) == V_UNDEF)
		{
			printf("%.3f ", m_clauses[i].m_thickness);
		}
	}
	printf("\n");
*/
	return cumulative_thickness;
}


int sExperimentalBooleanFormula::select_Undefined(void)
{
	int undefs = 0;
	for (int i = 0; i < m_C; ++i)
	{
		if (calc_Value(m_clauses[i]) == V_UNDEF)
		{
			m_clauses[i].m_selected = true;
			++undefs;
		}
		else
		{
			m_clauses[i].m_selected = false;
		}
	}
	return undefs;
}


int sExperimentalBooleanFormula::select_Arity(int arity)
{
	int selects = 0;
	for (int i = 0; i < m_C; ++i)
	{
		if (m_clauses[i].m_cL == arity/* || rand() % 100 < 5*/)
//		if (rand() % 100 < 10)
		{
			m_clauses[i].m_selected = true;
			++selects;
		}
		else
		{
			m_clauses[i].m_selected = false;
		}
	}
	return selects;
}


int sExperimentalBooleanFormula::select_Clustering(sIncrementalClustering *clustering)
{
	for (int i = 0; i < m_C; ++i)
	{
		m_clauses[i].m_selected = false;
	}
	int selects = 0;
	int N_Clusters = clustering->get_ClusterCount();

	for (int i = 0; i < N_Clusters; ++i)
	{
		sCluster *cluster = clustering->get_cluster(i);
		if (cluster->m_N_Vertices > 2 /*&& rand() % 100 < 10*/)
		{
			for (int k = 0; k < cluster->m_N_Vertices; ++k)
			{
				sLiteral *literal = cluster->m_Vertices[k]->m_literal;
				ONode *onode = literal->m_occs;
				
				while (onode != NULL)
				{
					if (calc_Value(*onode->m_clause) == V_UNDEF && !onode->m_clause->m_selected/* && rand() % 100 < 50*/)
					{
						onode->m_clause->m_selected = true;
						++selects;
						break;
					}
					onode = onode->m_next;
				}
			}
		}
	}
	return selects;
}


int sExperimentalBooleanFormula::select_Thickness(double threshold)
{
	int selects = 0;

	for (int i = 0; i < m_C; ++i)
	{
		if (m_clauses[i].m_thickness <= threshold/* || rand() % 1000 < 10*/)
		{
			m_clauses[i].m_selected = true;
			++selects;
		}
		else
		{
			m_clauses[i].m_selected = false;
		}
	}

	return selects;
}


int sExperimentalBooleanFormula::select_ArityThickness(int arity, double threshold)
{
	int selects = 0;

	for (int i = 0; i < m_C; ++i)
	{
		if (m_clauses[i].m_thickness <= threshold && m_clauses[i].m_cL == arity)
		{
			m_clauses[i].m_selected = true;
			++selects;
		}
		else
		{
			m_clauses[i].m_selected = false;
		}
	}

	return selects;
}


sVertex* sExperimentalBooleanFormula::get_best_vertex(sIncrementalGraph *graph)
{
	int n_Vertices = graph->get_VertexCount();

	sVertex *best_vertex = NULL;

	int i;

	for (i = 0; i < n_Vertices; ++i)
	{
		sVertex *vertex = graph->get_Vertex(i);

		if (vertex->m_cluster < 0)
		{
			best_vertex = vertex;
			break;
		}
	}
	for (; i < n_Vertices; ++i)
	{
		sVertex *vertex = graph->get_Vertex(i);

		if (vertex->m_cluster < 0)
		{
			if (vertex->m_score > best_vertex->m_score)
			{
				best_vertex = vertex;
			}
		}
	}

	return best_vertex;
}


void sExperimentalBooleanFormula::update_NeighborsScore(sIncrementalGraph *graph, sVertex *vertex, sVertexHeap *vertex_heap)
{
	sNeighborNode *neighbor_node = vertex->m_first_nnode;
	while (neighbor_node != NULL)
	{
		if (neighbor_node->m_vertex->m_cluster < 0)
		{
			int score = graph->calc_VertexScore_2(neighbor_node->m_vertex);
//			neighbor_node->m_vertex->m_score
			vertex_heap->change_Score(neighbor_node->m_vertex, score);
		}
		neighbor_node = neighbor_node->m_next;
	}
}


int sExperimentalBooleanFormula::build_ConflictPath(sClause **path, int length, sIncrementalGraph *graph, Clustering *clustering)
{
	int result;

	if ((result = clustering->init(this, path, length, graph)) < 0)
	{
		return result;
	}

	int V = 0;

	for (int l = 0; l < length; ++l)
	{
		V += path[l]->m_cL;
	}

	if ((result = graph->init(this, V, sEDGE_CHUNK, sNEIGHBOR_CHUNK)) < 0)
	{
		return result;
	}
	if ((result = graph->add_Vertices(V)) < 0)
	{
		return result;
	}

	int id = 0;

	for (int i = 0; i <= m_V; ++i)
	{
		sLiteral &p_literal = m_P_literals[i];
		sLiteral &n_literal = m_N_literals[i];

		p_literal.m_voccs = NULL;
		n_literal.m_voccs = NULL;
	}

	for (int l = 0; l < length; ++l)
	{
		sLiteral **literal = path[l]->m_literals;
		sVertex **cvertex = clustering->m_clusters[l].m_vertices;

		while (*literal != NULL)
		{
			sVertex *vertex = graph->get_Vertex(id++);

			vertex->m_literal = *literal++;
			vertex->m_cluster = l;
			vertex->m_layer = -1;
			(*cvertex++) = vertex;

			VONode &vo_node = graph->m_vo_nodes[graph->m_cVO++];
			vo_node.m_next = vertex->m_literal->m_voccs;
			vo_node.m_vertex = vertex;

			vertex->m_literal->m_voccs = &vo_node;
		}
	}

	for (int l = 0; l < length - 1; ++l)
	{
		Cluster &cluster = clustering->m_clusters[l];

		for (int j = 0; j < cluster.m_cV; ++j)
		{
			sVertex *vertex = cluster.m_vertices[j];
			sLiteral *literal = vertex->m_literal;

			if (!is_Assigned(*literal))
			{
				sLiteral **check_point = get_CheckPoint();
				operate_SetPropagate_TRUE(literal);
			
				sLiteral **chng_literal = check_point + 1;
				
				while (chng_literal < m_last_chng)
				{
					if (!is_Complementary(literal, (*chng_literal)->m_cmpl))
					{
						VONode *vo_node = (*chng_literal)->m_cmpl->m_voccs;
						
						while (vo_node != NULL)
						{
							if (vo_node->m_vertex)
							{
								if (vertex->m_idx < vo_node->m_vertex->m_idx && vertex->m_cluster != vo_node->m_vertex->m_cluster)
								{
									if ((result = graph->add_Edge(vertex->m_idx, vo_node->m_vertex->m_idx)) < 0)
									{
										return result;
									}
								}
							}
							vo_node = vo_node->m_next;
						}
					}
					++chng_literal;
				}
				operate_UnsetUnpropagate(check_point);
			}
		}
	}

	return 0;
}


int sExperimentalBooleanFormula::build_ConflictPath(sClause **path, int length, sIncrementalGraph *graph, Clustering *clustering, Layering *layering)
{
	int result;

	if ((result = build_ConflictPath(path, length, graph, clustering)) < 0)
	{
		return result;
	}

	if ((result = layering->init(this, path, length, graph)) < 0)
	{
		return result;
	}

	for (int i = 0; i < clustering->m_cC; ++i)
	{
		Cluster &cluster = clustering->m_clusters[i];

		for (int j = 0; j < cluster.m_cV; ++j)
		{
			if (cluster.m_vertices[j]->m_layer == -1)
			{
				build_next_layer(i, j, graph, clustering, layering);
			}
		}
	}

	return 0;
}


int sExperimentalBooleanFormula::build_ConflictPath_1(sClause **path, int length, sIncrementalGraph *graph, Clustering *clustering)
{
	int result;

	if ((result = clustering->init(this, path, length, graph)) < 0)
	{
		return result;
	}

	int V = 0;

	for (int l = 0; l < length; ++l)
	{
		V += path[l]->m_cL;
	}

	if ((result = graph->init(this, V, sEDGE_CHUNK, sNEIGHBOR_CHUNK)) < 0)
	{
		return result;
	}
	if ((result = graph->add_Vertices(V)) < 0)
	{
		return result;
	}

	int id = 0;

	for (int i = 0; i <= m_V; ++i)
	{
		sLiteral &p_literal = m_P_literals[i];
		sLiteral &n_literal = m_N_literals[i];

		p_literal.m_voccs = NULL;
		n_literal.m_voccs = NULL;
	}

	for (int l = 0; l < length; ++l)
	{
		sLiteral **literal = path[l]->m_literals;
		sVertex **cvertex = clustering->m_clusters[l].m_vertices;

		while (*literal != NULL)
		{
			sVertex *vertex = graph->get_Vertex(id++);

			vertex->m_literal = *literal++;
			vertex->m_cluster = l;
			vertex->m_layer = -1;
			(*cvertex++) = vertex;

			VONode &vo_node = graph->m_vo_nodes[graph->m_cVO++];
			vo_node.m_next = vertex->m_literal->m_voccs;
			vo_node.m_vertex = vertex;

			vertex->m_literal->m_voccs = &vo_node;
		}
	}

	for (int l = 0; l < length - 1; ++l)
	{
		Cluster &cluster = clustering->m_clusters[l];

		for (int j = 0; j < cluster.m_cV; ++j)
		{
			sVertex *vertex = cluster.m_vertices[j];
			sLiteral *literal = vertex->m_literal;

			if (!is_Assigned(*literal))
			{
				sLiteral **check_point = get_CheckPoint();
				operate_SetPropagate_TRUE(literal);
			
				sLiteral **chng_literal = check_point;
				
				while (chng_literal < m_last_chng)
				{
					VONode *vo_node = (*chng_literal)->m_cmpl->m_voccs;
					
					while (vo_node != NULL)
					{
						if (vo_node->m_vertex)
						{
							if (vertex->m_idx < vo_node->m_vertex->m_idx && vertex->m_cluster != vo_node->m_vertex->m_cluster)
							{
								if ((result = graph->add_Edge(vertex->m_idx, vo_node->m_vertex->m_idx)) < 0)
								{
									return result;
								}
							}
						}
						vo_node = vo_node->m_next;
					}		
					++chng_literal;
				}
				operate_UnsetUnpropagate(check_point);
			}
		}
	}

	return 0;
}


int sExperimentalBooleanFormula::build_ConflictPath_2(sClause **path, int length, sIncrementalGraph *literal_conflict, sIncrementalGraph *graph, Clustering *clustering)
{
	int result;

	if ((result = clustering->init(this, path, length, graph)) < 0)
	{
		return result;
	}

	int V = 0;

	for (int l = 0; l < length; ++l)
	{
		V += path[l]->m_cL;
	}
	if ((result = graph->init(this, V, sEDGE_CHUNK, sNEIGHBOR_CHUNK)) < 0)
	{
		return result;
	}
	if ((result = graph->add_Vertices(V)) < 0)
	{
		return result;
	}

	int id = 0;

	for (int l = 0; l < length; ++l)
	{
		sLiteral **literal = path[l]->m_literals;
		Cluster &cluster = clustering->m_clusters[l];
		sVertex **cvertex = cluster.m_vertices;

		while (*literal != NULL)
		{
			sVertex *vertex = graph->get_Vertex(id++);

			vertex->m_literal = *literal++;
			vertex->m_cluster = l;
			vertex->m_layer = -1;
			(*cvertex++) = vertex;
		}
		cluster.m_cV = path[l]->m_cL;
	}

	for (int l1 = 0; l1 < length - 1; ++l1)
	{
		Cluster &cluster_1 = clustering->m_clusters[l1];

		for (int l2 = l1 + 1; l2 < length; ++l2)
		{
			Cluster &cluster_2 = clustering->m_clusters[l2];

			for (int v1 = 0; v1 < cluster_1.m_cV; ++v1)
			{
				sVertex *vertex_1 = cluster_1.m_vertices[v1];
				int vertex_1_idx = vertex_1->m_literal->m_vertex_idx;

				for (int v2 = 0; v2 < cluster_2.m_cV; ++v2)
				{
					sVertex *vertex_2 = cluster_2.m_vertices[v2];
					int vertex_2_idx = vertex_2->m_literal->m_vertex_idx;

					if (is_Complementary(vertex_1->m_literal, vertex_2->m_literal) || literal_conflict->is_Adjacent(vertex_1_idx, vertex_2_idx)/*literal_conflict->get_Edge(vertex_1_idx, vertex_2_idx) != NULL*/)
					{
						if ((result = graph->add_Edge(vertex_1->m_idx, vertex_2->m_idx)) < 0)
						{
							assert(false);
							return result;
						}

					}
				}
			}
		}
	}
/*
	for (int l = 0; l < length - 1; ++l)
	{
		Cluster &cluster = clustering->m_clusters[l];

		for (int j = 0; j < cluster.m_cV; ++j)
		{
			sVertex *vertex = cluster.m_vertices[j];
			sLiteral *literal = vertex->m_literal;

			if (!is_Assigned(*literal))
			{
				sLiteral **check_point = get_CheckPoint();
				operate_SetPropagate_TRUE(literal);
			
				sLiteral **chng_literal = check_point;
				
				while (chng_literal < m_last_chng)
				{
					VONode *vo_node = (*chng_literal)->m_cmpl->m_voccs;
					
					while (vo_node != NULL)
					{
						if (vo_node->m_vertex)
						{
							if (vertex->m_idx < vo_node->m_vertex->m_idx && vertex->m_cluster != vo_node->m_vertex->m_cluster)
							{
								if ((result = graph->add_Edge(vertex->m_idx, vo_node->m_vertex->m_idx)) < 0)
								{
									assert(false);
									return result;
								}
							}
						}
						vo_node = vo_node->m_next;
					}
					
					++chng_literal;
				}
				operate_UnsetUnpropagate(check_point);
			}
		}
	}
*/
	return 0;
}


int sExperimentalBooleanFormula::build_ConflictPath_2(sClause **path, int length, sIncrementalGraph *literal_conflict, sIncrementalGraph *graph, Clustering *clustering, Layering *layering)
{
	int result;

 	if ((result = build_ConflictPath_2(path, length, literal_conflict, graph, clustering)) < 0)
//	if ((result = build_ConflictPath_1(path, length, graph, clustering)) < 0)
	{
		return result;
	}
	if ((result = layering->init(this, graph)) < 0)
	{
		return result;
	}
	if ((result = graph->build_CoveringLayers(clustering, layering)) < 0)
	{
		return result;
	}
//	layering->print();

	return 0;
}


void sExperimentalBooleanFormula::build_next_layer(int cluster_idx, int vertex_idx, sIncrementalGraph *graph, Clustering *clustering, Layering *layering)
{
	Layer &layer = layering->m_Layers[layering->m_cL];
	sVertex *vertex = clustering->m_clusters[cluster_idx].m_vertices[vertex_idx];
	vertex->m_layer = layering->m_cL;
	layer.m_vertices[layer.m_cV++] = vertex;

	for (int c = cluster_idx + 1; c < clustering->m_cC; ++c)
	{
		Cluster &cluster = clustering->m_clusters[c];

		int best_back = 0;
		sVertex *best_vertex = NULL;

		for (int v = 0; v < cluster.m_cV; ++v)
		{
			vertex = cluster.m_vertices[v];

			if (vertex->m_layer == -1)
			{
				int back = 0;
				for (int i = 0; i < layer.m_cV; ++i)
				{
					if (graph->get_Edge(layer.m_vertices[i]->m_idx, vertex->m_idx) != NULL)
					{
						++back;
					}
				}
				if (best_vertex == NULL || best_back < back)
				{
					best_vertex = vertex;
					best_back = back;
				}
			}
		}

		if (best_vertex != NULL)
		{
			layer.m_vertices[layer.m_cV++] = best_vertex;
			best_vertex->m_layer = layering->m_cL;
		}
	}
	++layering->m_cL;
}


void sExperimentalBooleanFormula::calculate_MaximumVisits(Layering *layering)
{
	for (int i = 0; i < layering->m_cL; ++i)
	{
		Layer &layer = layering->m_Layers[i];
		int visits_1 = 0;

		for (int j = 0; j < layer.m_cV; ++j)
		{
			sVertex *vertex = layer.m_vertices[j];
			int max_prev = 0;

			for (int k = 0; k < j; ++k)
			{
				sVertex *prev_vertex = layer.m_vertices[k];

				if (!is_Complementary(prev_vertex->m_literal, vertex->m_literal) && layering->m_graph->get_Edge(prev_vertex->m_idx, vertex->m_idx) == NULL)
				{
					if (max_prev < prev_vertex->m_max_visits)
					{
						++max_prev;
					}
				}
			}

			int visits_2 = max_prev + 1;

			if (visits_1 > visits_2)
			{
				vertex->m_max_visits = visits_1;
			}
			else
			{
				vertex->m_max_visits = visits_2;
			}
			visits_1 = vertex->m_max_visits;
		}
	}
}


void sExperimentalBooleanFormula::check_paths_SPC(Clustering *clustering, Layering *layering)
{
	Cluster &first_cluster = clustering->m_clusters[0];
	Cluster &last_cluster = clustering->m_clusters[clustering->m_cC - 1];

	for (int v1 = 0; v1 < first_cluster.m_cV; ++v1)
	{
		for (int v2 = 0; v2 < last_cluster.m_cV; ++v2)
		{
			sVertex *vertex_1 = first_cluster.m_vertices[v1];
			sVertex *vertex_2 = last_cluster.m_vertices[v2];

			bool exists = check_path_SPC(vertex_1, vertex_2, clustering, layering);

			if (exists)
			{
				//printf("+(%d,%d)\n", vertex_1->m_idx, vertex_2->m_idx);
			}
			else
			{
				if (!is_Complementary(vertex_1->m_literal, vertex_2->m_literal) && clustering->m_graph->get_Edge(vertex_1->m_idx, vertex_2->m_idx) == NULL)
				{
					sBinaryClause bin_clause(vertex_1->m_literal->m_cmpl, vertex_2->m_literal->m_cmpl);
					m_binary_Clauses.insert(bin_clause);
					fprintf(stderr, ".");
				}
/*
				print_literal(vertex_1->m_literal);
				printf(" ");
				print_literal(vertex_2->m_literal);
				printf(" 0\n");
*/
//				printf("%d %d 0\n", vertex_1->m_idx, vertex_2->m_idx);
			}
		}
	}
}


bool sExperimentalBooleanFormula::check_path_SPC(sVertex *first_vertex, sVertex *last_vertex, Clustering *clustering, Layering *sUNUSED(layering))
{
	if (is_Complementary(first_vertex->m_literal, last_vertex->m_literal) || clustering->m_graph->get_Edge(first_vertex->m_idx, last_vertex->m_idx) != NULL)
	{
		return false;
	}

	Cluster &first_cluster = clustering->m_clusters[0];
	
	last_vertex->m_distance = -1;

	for (int v = 0; v < first_cluster.m_cV; ++v)
	{
		sVertex *vertex = first_cluster.m_vertices[v];
		vertex->m_distance = -1;
	}
	first_vertex->m_distance = 0;
	
	for (int c = 0; c < clustering->m_cC - 1; ++c)
	{
		Cluster &cluster_1 = clustering->m_clusters[c];
		Cluster &cluster_2 = clustering->m_clusters[c + 1];
			
		for (int v2 = 0; v2 < cluster_2.m_cV; ++v2)
		{
			sVertex *vertex_2 = cluster_2.m_vertices[v2];
			vertex_2->m_distance = -1;
		}
		
		for (int v2 = 0; v2 < cluster_2.m_cV; ++v2)
		{
			sVertex *vertex_2 = cluster_2.m_vertices[v2];
			
			for (int v1 = 0; v1 < cluster_1.m_cV; ++v1)
			{
				sVertex *vertex_1 = cluster_1.m_vertices[v1];
				
				if (vertex_1->m_distance >= 0)
				{
					if (!is_Complementary(vertex_1->m_literal, vertex_2->m_literal) && clustering->m_graph->get_Edge(vertex_1->m_idx, vertex_2->m_idx) == NULL)
					{
						vertex_2->m_distance = vertex_1->m_distance + 1;
					}
				}
			}
		}
	}
	
	if (last_vertex->m_distance >= 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}


void sExperimentalBooleanFormula::check_paths_APC(Clustering *clustering, Layering *layering)
{
	Cluster &first_cluster = clustering->m_clusters[0];
	Cluster &last_cluster = clustering->m_clusters[clustering->m_cC - 1];

	for (int v1 = 0; v1 < first_cluster.m_cV; ++v1)
	{
		for (int v2 = 0; v2 < last_cluster.m_cV; ++v2)
		{
			sVertex *vertex_1 = first_cluster.m_vertices[v1];
			sVertex *vertex_2 = last_cluster.m_vertices[v2];

			bool exists = check_path_APC(vertex_1, vertex_2, clustering, layering);

//			clustering->print_visit_matrices();

			if (exists)
			{
//				printf("preserved (%d,%d)\n", vertex_1->m_id, vertex_2->m_id);
			}
			else
			{
				if (!is_Complementary(vertex_1->m_literal, vertex_2->m_literal) && clustering->m_graph->get_Edge(vertex_1->m_idx, vertex_2->m_idx) == NULL)
				{
					sBinaryClause bin_clause(vertex_1->m_literal->m_cmpl, vertex_2->m_literal->m_cmpl);
					m_binary_Clauses.insert(bin_clause);
					fprintf(stderr, ".");
				}
/*
				print_literal(vertex_1->m_literal);
				printf(" ");
				print_literal(vertex_2->m_literal);
				printf(" 0\n");
*/
//				printf("*apc -(%d,%d)\n", vertex_1->m_id, vertex_2->m_id);
			}

//			exit(0);
		}
	}
}


bool sExperimentalBooleanFormula::check_path_APC(sVertex *first_vertex, sVertex *last_vertex, Clustering *clustering, Layering *layering)
{
	if (is_Complementary(first_vertex->m_literal, last_vertex->m_literal) || clustering->m_graph->get_Edge(first_vertex->m_idx, last_vertex->m_idx) != NULL)
	{
		return false;
	}

	Cluster &first_cluster = clustering->m_clusters[0];
	
	for (int v = 0; v < first_cluster.m_cV; ++v)
	{
		sVertex *vertex = first_cluster.m_vertices[v];
		vertex->m_vmatrix.reset();
	}
		
	for (int l = 0; l < layering->m_L; ++l)
	{
		first_vertex->m_vmatrix.m_cPaths[l][0] = 1;
	}
	first_vertex->m_vmatrix.m_total_paths = 1;
		
	first_vertex->m_vmatrix.m_cPaths[first_vertex->m_layer][0] = 0;
	first_vertex->m_vmatrix.m_cPaths[first_vertex->m_layer][1] = 1;

	for (int c = 0; c < clustering->m_cC - 1; ++c)
	{
		Cluster &cluster_1 = clustering->m_clusters[c];
		Cluster &cluster_2 = clustering->m_clusters[c + 1];
		
		for (int v2 = 0; v2 < cluster_2.m_cV; ++v2)
		{
			sVertex *vertex_2 = cluster_2.m_vertices[v2];
			vertex_2->m_vmatrix.reset();
		}
		
		for (int v2 = 0; v2 < cluster_2.m_cV; ++v2)
		{
			sVertex *vertex_2 = cluster_2.m_vertices[v2];
			
			for (int v1 = 0; v1 < cluster_1.m_cV; ++v1)
			{
				sVertex *vertex_1 = cluster_1.m_vertices[v1];
				
				if (vertex_1->m_vmatrix.m_total_paths > 0)
				{
					if (!is_Complementary(vertex_1->m_literal, vertex_2->m_literal) && clustering->m_graph->get_Edge(vertex_1->m_idx, vertex_2->m_idx) == NULL)
					{
						bool acceptable = false;
						
						for (int v = 0; v < vertex_2->m_max_visits; ++v)
						{
							if (vertex_1->m_vmatrix.m_cPaths[vertex_2->m_layer][v] > 0)
							{
								acceptable = true;
								break;
							}
						}
						
						if (acceptable)
						{
							vertex_2->m_vmatrix.m_total_paths = sMIN(sTOTAL_PATH_MAX, vertex_2->m_vmatrix.m_total_paths + vertex_1->m_vmatrix.m_total_paths);
							
							for (int v = 0; v <= vertex_2->m_vmatrix.m_max_visits; ++v)
							{
								for (int l = 0; l < vertex_2->m_vmatrix.m_L; ++l)
								{
									if (l == vertex_2->m_layer)
									{
										if (v == 0)
										{
											vertex_2->m_vmatrix.m_cPaths[vertex_2->m_layer][0] = 0;
										}
										else
										{
											vertex_2->m_vmatrix.m_cPaths[vertex_2->m_layer][v] = sMIN(sTOTAL_PATH_MAX, vertex_2->m_vmatrix.m_cPaths[vertex_2->m_layer][v] + vertex_1->m_vmatrix.m_cPaths[vertex_2->m_layer][v - 1]);
										}
									}
									else
									{
										vertex_2->m_vmatrix.m_cPaths[l][v] = sMIN(sTOTAL_PATH_MAX, vertex_2->m_vmatrix.m_cPaths[l][v] + vertex_1->m_vmatrix.m_cPaths[l][v]);
									}
								}
							}
/*
							printf("%d,%d\n", vertex_2->m_layer, vertex_2->m_max_visits);
							vertex_1->m_vmatrix.print();
							if (vertex_1->m_vmatrix.m_cPaths[vertex_2->m_layer][vertex_2->m_max_visits] > 0)
							{
								printf("=%d= ", vertex_1->m_vmatrix.m_cPaths[vertex_2->m_layer][vertex_2->m_max_visits]);
							}
*/
						}
					}
				}
			}
			
			if (vertex_2->m_vmatrix.m_total_paths > 0)
			{
				bool terminal = true;
				
				for (int v = 0; v <= vertex_2->m_max_visits; ++v)
				{
					if (vertex_2->m_vmatrix.m_cPaths[vertex_2->m_layer][v] > 0)
					{
						terminal = false;
					}
				}
				if (terminal)
				{
					for (int v = 0; v <= vertex_2->m_vmatrix.m_max_visits; ++v)
					{
						for (int l = 0; l < vertex_2->m_vmatrix.m_L; ++l)
						{
							vertex_2->m_vmatrix.m_cPaths[l][v] = 0;
						}
					}
					vertex_2->m_vmatrix.m_total_paths = 0;
				}
			}
		}
	}

	if (last_vertex->m_vmatrix.m_total_paths > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}


void sExperimentalBooleanFormula::check_paths_BPC(Clustering *clustering, Layering *layering)
{
	Cluster &first_cluster = clustering->m_clusters[0];
	Cluster &last_cluster = clustering->m_clusters[clustering->m_cC - 1];

	for (int v1 = 0; v1 < first_cluster.m_cV; ++v1)
	{
		for (int v2 = 0; v2 < last_cluster.m_cV; ++v2)
		{
			sVertex *vertex_1 = first_cluster.m_vertices[v1];
			sVertex *vertex_2 = last_cluster.m_vertices[v2];

			bool exists = check_path_BPC(vertex_1, vertex_2, clustering, layering);

//			clustering->print_visit_matrices();

			if (exists)
			{
//				printf("preserved (%d,%d)\n", vertex_1->m_id, vertex_2->m_id);
			}
			else
			{
				if (!is_Complementary(vertex_1->m_literal, vertex_2->m_literal) && clustering->m_graph->get_Edge(vertex_1->m_idx, vertex_2->m_idx) == NULL)
				{
					sBinaryClause bin_clause(vertex_1->m_literal->m_cmpl, vertex_2->m_literal->m_cmpl);
					m_binary_Clauses.insert(bin_clause);
					fprintf(stderr, ".");
				}
/*
				print_literal(vertex_1->m_literal);
				printf(" ");
				print_literal(vertex_2->m_literal);
				printf(" 0\n");
*/
//				printf("*apc -(%d,%d)\n", vertex_1->m_id, vertex_2->m_id);
			}

//			exit(0);
		}
	}
}


bool sExperimentalBooleanFormula::check_path_BPC(sVertex *first_vertex, sVertex *last_vertex, Clustering *clustering, Layering *layering)
{
	if (is_Complementary(first_vertex->m_literal, last_vertex->m_literal) || clustering->m_graph->get_Edge(first_vertex->m_idx, last_vertex->m_idx) != NULL)
	{
		return false;
	}

	layering->reset();

	Cluster &first_cluster = clustering->m_clusters[0];
	first_cluster.m_path_vertex = first_vertex;

	++layering->m_Layers[first_vertex->m_layer].m_path_selects;
	
	last_vertex->m_distance = -1;
	
	if (check_path_BPC(last_vertex, 1, clustering->m_cC - 2, clustering, layering))
	{
		return true;
	}
	else
	{
		return false;
	}
}


bool sExperimentalBooleanFormula::check_path_BPC(sVertex *last_vertex, int c1, int c2,  Clustering *clustering, Layering *layering)
{
	Cluster &cluster_1 = clustering->m_clusters[c1];
		
	for (int v = 0; v < cluster_1.m_cV; ++v)
	{
		sVertex *vertex = cluster_1.m_vertices[v];
		
		bool allowed = true;
		
		sVertex *prev = clustering->m_clusters[c1 - 1].m_path_vertex;

		if (is_Complementary(prev->m_literal, vertex->m_literal) || clustering->m_graph->get_Edge(prev->m_idx, vertex->m_idx) != NULL)
		{
			allowed = false;
		}
		else
		{
			if (is_Complementary(vertex->m_literal, last_vertex->m_literal) || clustering->m_graph->get_Edge(vertex->m_idx, last_vertex->m_idx) != NULL)
			{
				allowed = false;
			}
			else
			{
				if (layering->m_Layers[vertex->m_layer].m_path_selects >= vertex->m_max_visits)
				{
					allowed = false;
				}
			}
		}
		if (allowed)
		{
			cluster_1.m_path_vertex = vertex;
			++layering->m_Layers[vertex->m_layer].m_path_selects;

			bool success = false;

			if (c1 < c2)
			{
				if (check_path_BPC(last_vertex, c1 + 1, c2, clustering, layering))
				{
					success = true;
				}
			}
			else
			{
				success = true;
			}
			--layering->m_Layers[vertex->m_layer].m_path_selects;

			if (success)
			{
				return true;
			}
		}
	}
	
	return false;
}


void sExperimentalBooleanFormula::check_paths_EPC(Clustering *clustering)
{
	Cluster &first_cluster = clustering->m_clusters[0];

	for (int lc = clustering->m_cC - 1 /*3*/; lc < clustering->m_cC; ++lc)
	{
		Cluster &last_cluster = clustering->m_clusters[lc];

		for (int v1 = 0; v1 < first_cluster.m_cV; ++v1)
		{
			for (int v2 = 0; v2 < last_cluster.m_cV; ++v2)
			{
				sVertex *vertex_1 = first_cluster.m_vertices[v1];
				sVertex *vertex_2 = last_cluster.m_vertices[v2];
				
				bool exists = check_path_EPC(vertex_1, vertex_2, lc, clustering);

				if (exists)
				{
					//printf("+(%d,%d)\n", vertex_1->m_idx, vertex_2->m_idx);
				}
				else
				{
					if (!is_Complementary(vertex_1->m_literal, vertex_2->m_literal) && clustering->m_graph->get_Edge(vertex_1->m_idx, vertex_2->m_idx) == NULL)
					{
						sBinaryClause bin_clause(vertex_1->m_literal->m_cmpl, vertex_2->m_literal->m_cmpl);
						m_binary_Clauses.insert(bin_clause);
						fprintf(stderr, ".");
					}
				}
			}
		}
	}
}


bool sExperimentalBooleanFormula::check_path_EPC(sVertex *first_vertex, sVertex *last_vertex, int last_cluster, Clustering *clustering)
{
	if (   is_Complementary(first_vertex->m_literal, last_vertex->m_literal)
	    || clustering->m_graph->get_Edge(first_vertex->m_idx, last_vertex->m_idx) != NULL)
	{
		return false;
	}

	Cluster &first_cluster = clustering->m_clusters[0];
	first_cluster.m_path_vertex = first_vertex;
	
	last_vertex->m_distance = -1;
	
	if (check_path_EPC(last_vertex, 1, last_cluster, clustering))
	{
		return true;
	}
	else
	{
		return false;
	}
}


bool sExperimentalBooleanFormula::check_path_EPC(sVertex *last_vertex, int c1, int c2,  Clustering *clustering)
{
	Cluster &cluster_1 = clustering->m_clusters[c1];
		
	for (int v = 0; v < cluster_1.m_cV; ++v)
	{
		sVertex *vertex = cluster_1.m_vertices[v];
		
		bool allowed = true;

		if (is_Complementary(vertex->m_literal, last_vertex->m_literal) || clustering->m_graph->get_Edge(vertex->m_idx, last_vertex->m_idx) != NULL)
		{
			allowed = false;
		}
	       	else
		{
			for (int c = 0; c < c1; ++c)
			{
				sVertex *pv = clustering->m_clusters[c].m_path_vertex;
				
				if (is_Complementary(pv->m_literal, vertex->m_literal) || clustering->m_graph->get_Edge(pv->m_idx, vertex->m_idx) != NULL)
				{
					allowed = false;
					break;
				}
			}
		}
		if (allowed)
		{
			cluster_1.m_path_vertex = vertex;

			if (c1 < c2)
			{
				if (check_path_EPC(last_vertex, c1 + 1, c2, clustering))
				{
					return true;
				}
			}
			else
			{
				return true;
			}
		}
	}
	
	return false;
}


void sExperimentalBooleanFormula::check_paths_LPC(Clustering *clustering, int limit)
{
	Cluster &first_cluster = clustering->m_clusters[0];

	for (int lc = clustering->m_cC - 1 /*3*/; lc < clustering->m_cC; ++lc)
	{
		Cluster &last_cluster = clustering->m_clusters[lc];

		for (int v1 = 0; v1 < first_cluster.m_cV; ++v1)
		{
			for (int v2 = 0; v2 < last_cluster.m_cV; ++v2)
			{
				sVertex *vertex_1 = first_cluster.m_vertices[v1];
				sVertex *vertex_2 = last_cluster.m_vertices[v2];

				bool exists = check_path_LPC(vertex_1, vertex_2, lc, clustering, limit);

				if (exists)
				{
					//printf("+(%d,%d)\n", vertex_1->m_idx, vertex_2->m_idx);
				}
				else
				{
					if (!is_Complementary(vertex_1->m_literal, vertex_2->m_literal) && clustering->m_graph->get_Edge(vertex_1->m_idx, vertex_2->m_idx) == NULL)
					{
						sBinaryClause bin_clause(vertex_1->m_literal->m_cmpl, vertex_2->m_literal->m_cmpl);
						m_binary_Clauses.insert(bin_clause);
						fprintf(stderr, ".");
					}
				}
			}
		}
	}
}


bool sExperimentalBooleanFormula::check_path_LPC(sVertex *first_vertex, sVertex *last_vertex, int last_cluster, Clustering *clustering, int limit)
{
	if (   is_Complementary(first_vertex->m_literal, last_vertex->m_literal)
	    || clustering->m_graph->get_Edge(first_vertex->m_idx, last_vertex->m_idx) != NULL)
	{
		return false;
	}

	Cluster &first_cluster = clustering->m_clusters[0];
	first_cluster.m_path_vertex = first_vertex;
	
	last_vertex->m_distance = -1;

	int limit_cnt = 0;
	
	if (check_path_LPC(last_vertex, 1, last_cluster - 1, clustering, limit_cnt, limit))
	{
		return true;
	}
	else
	{
		return false;
	}
}


bool sExperimentalBooleanFormula::check_path_LPC(sVertex *last_vertex, int c1, int c2,  Clustering *clustering, int &limit_cnt, int limit)
{
	if (limit >= 0 && limit_cnt > limit)
	{
		return true;
	}

	Cluster &cluster_1 = clustering->m_clusters[c1];
		
	for (int v = 0; v < cluster_1.m_cV; ++v)
	{
		++limit_cnt;

		sVertex *vertex = cluster_1.m_vertices[v];
		
		bool allowed = true;

		if (is_Complementary(vertex->m_literal, last_vertex->m_literal) || clustering->m_graph->get_Edge(vertex->m_idx, last_vertex->m_idx) != NULL)
		{
			allowed = false;
		}
	       	else
		{
			for (int c = 0; c < c1; ++c)
			{
				sVertex *pv = clustering->m_clusters[c].m_path_vertex;
				
				if (is_Complementary(pv->m_literal, vertex->m_literal) || clustering->m_graph->get_Edge(pv->m_idx, vertex->m_idx) != NULL)
				{
					allowed = false;
					break;
				}
			}
		}
		if (allowed)
		{
			cluster_1.m_path_vertex = vertex;

			if (c1 < c2)
			{
				if (check_path_LPC(last_vertex, c1 + 1, c2, clustering, limit_cnt, limit))
				{
					return true;
				}
			}
			else
			{
				return true;
			}
		}
	}
	
	return false;
}


void sExperimentalBooleanFormula::check_paths_LPC_2(sIncrementalGraph *literal_conflict, Clustering *clustering, Layering *layering, int limit)
{
	Cluster &first_cluster = clustering->m_clusters[0];
	
	int lc = clustering->m_cC - 1;
	Cluster &last_cluster = clustering->m_clusters[lc];

	for (int v1 = 0; v1 < first_cluster.m_cV; ++v1)
	{
		for (int v2 = 0; v2 < last_cluster.m_cV; ++v2)
		{
			sVertex *vertex_1 = first_cluster.m_vertices[v1];
			sVertex *vertex_2 = last_cluster.m_vertices[v2];
			
			sBinaryClause bin_clause(vertex_1->m_literal->m_cmpl, vertex_2->m_literal->m_cmpl);
			
			int lit_idx_1 = vertex_1->m_literal->m_vertex_idx;
			int lit_idx_2 = vertex_2->m_literal->m_vertex_idx;
			
			if (   !is_Complementary(vertex_1->m_literal, vertex_2->m_literal)
			    &&  m_binary_Clauses.find(bin_clause) == m_binary_Clauses.end()
			    && !literal_conflict->is_Adjacent(lit_idx_1, lit_idx_2))
			{
				int limit_cnt;
				bool exists = check_path_LPC_2(vertex_1, vertex_2, lc, literal_conflict, clustering, layering, limit_cnt, limit);
				
				if (!exists && limit_cnt >= ACCEPTABLE_LIMIT)
				{
					m_binary_Clauses.insert(bin_clause);
					fprintf(stderr, ".");
					fprintf(stderr, "Limit_cnt:%d\n", limit_cnt);
				}
			}
		}
	}
}


bool sExperimentalBooleanFormula::check_path_LPC_2(sVertex *first_vertex, sVertex *last_vertex, int last_cluster, sIncrementalGraph *literal_conflict, Clustering *clustering, Layering *layering, int &limit_cnt, int limit)
{
	int lit_idx_1 = first_vertex->m_literal->m_vertex_idx;
	int lit_idx_2 = last_vertex->m_literal->m_vertex_idx;

	if (   is_Complementary(first_vertex->m_literal, last_vertex->m_literal)
	    || literal_conflict->is_Adjacent(lit_idx_1, lit_idx_2))
	{
		return false;
	}

	Cluster &first_cluster = clustering->m_clusters[0];
	first_cluster.m_path_vertex = first_vertex;
	
	last_vertex->m_distance = -1;

	limit_cnt = 0;

	for (int l = 0; l < layering->m_cL; ++l)
	{
		layering->m_Layers[l].m_path_selects = 0;
	}
	for (int l = 0; l < first_vertex->m_Layer_cnt; ++l)
	{
		++layering->m_Layers[first_vertex->m_Layers[l]].m_path_selects;
	}
	for (int l = 0; l < last_vertex->m_Layer_cnt; ++l)
	{
		++layering->m_Layers[last_vertex->m_Layers[l]].m_path_selects;
	}
	
	if (check_path_LPC_2(last_vertex, 1, last_cluster - 1, literal_conflict, clustering, layering, limit_cnt, limit))
	{
		return true;
	}
	else
	{
		return false;
	}
}


bool sExperimentalBooleanFormula::check_path_LPC_2(sVertex *last_vertex, int c1, int c2,  sIncrementalGraph *literal_conflict, Clustering *clustering, Layering *layering, int &limit_cnt, int limit)
{
	int lit_idx_2 = last_vertex->m_literal->m_vertex_idx;

	if (limit >= 0 && limit_cnt > limit)
	{
		return true;
	}
	Cluster &cluster_1 = clustering->m_clusters[c1];
	
	for (int c = c1; c <= c2; ++c)
	{
		Cluster &cluster = clustering->m_clusters[c];
		
		for (int v = 0; v < cluster.m_cV; ++v)
		{
			sVertex *vertex = cluster.m_vertices[v];
			
			for (int l = 0; l < vertex->m_Layer_cnt; ++l)
			{
				layering->m_Layers[vertex->m_Layers[l]].m_checked = false;
			}
		}
	}
	int visits = 0;
	
	for (int c = c1; c <= c2; ++c)
	{
		Cluster &cluster = clustering->m_clusters[c];
		
		bool singleton = false;
		
		for (int v = 0; v < cluster.m_cV; ++v)
		{
			sVertex *vertex = cluster.m_vertices[v];
			
			if (vertex->m_Layer_cnt > 0)
			{
				bool available_vertex = true;
				for (int l = 0; l < vertex->m_Layer_cnt; ++l)
				{
					if (layering->m_Layers[vertex->m_Layers[l]].m_path_selects > 0)
					{
						available_vertex = false;
						break;
					}
				}
				if (available_vertex)
				{
					for (int l = 0; l < vertex->m_Layer_cnt; ++l)
					{
						if (!layering->m_Layers[vertex->m_Layers[l]].m_checked)
						{
							layering->m_Layers[vertex->m_Layers[l]].m_checked = true;
							if (layering->m_Layers[vertex->m_Layers[l]].m_path_selects < 1)
							{
								++visits;
							}
						}
					}
				}
			}
			else
			{
				singleton = true;
			}
		}
		if (singleton)
		{
			++visits;
		}
	}
	int requires = sMAX(0, c2 - c1 + 1);
	
	static int catches = 0;
	static int total_visits = 0;
	static int total_requires = 0;

	total_visits += visits;
	total_requires += requires;
	
//	printf("%d: --->: %d x %d \tsngl:%d\t r:%.3f\n", catches, visits, requires, singles, (double)total_visits/total_requires);

	if (visits < requires)
	{
		catches++;
		if (requires > 2)
		{
//			fprintf(stderr, "%d: -------------->: %d x %d\n", catches, visits, requires);
		}
		++limit_cnt;
		return false;
	}

	static int total_wipe_outs = 0;

	for (int c = c1; c <= c2; ++c)
	{
		Cluster &cluster = clustering->m_clusters[c];

		bool wipe_out = true;
	
		for (int v = 0; v < cluster.m_cV; ++v)
		{
			sVertex *vertex = cluster.m_vertices[v];
			bool available_vertex = true;
			
			for (int l = 0; l < vertex->m_Layer_cnt; ++l)
			{
				if (layering->m_Layers[vertex->m_Layers[l]].m_path_selects > 0)
				{
					available_vertex = false;
					break;
				}
			}
			if (available_vertex)
			{
				wipe_out = false;
				break;
			}
		}
		if (wipe_out)
		{
			++total_wipe_outs;
//			printf("%d: <*******************Domain wipe out occured ***************>\n", total_wipe_outs);
			return false;
		}
	}	
		
	for (int v = 0; v < cluster_1.m_cV; ++v)
	{
		++limit_cnt;

		sVertex *vertex = cluster_1.m_vertices[v];
		bool allowed = true;

		int lit_idx_1 = vertex->m_literal->m_vertex_idx;

		if (is_Complementary(vertex->m_literal, last_vertex->m_literal) || literal_conflict->is_Adjacent(lit_idx_1, lit_idx_2) /*literal_conflict->get_Edge(vertex->m_idx, last_vertex->m_idx) != NULL*/)
		{
			allowed = false;
		}
	       	else
		{
			for (int l = 0; l < vertex->m_Layer_cnt; ++l)
			{
				if (layering->m_Layers[vertex->m_Layers[l]].m_path_selects >= 1)
				{
					allowed = false;
					break;
				}
			}
		}
		if (allowed)
		{
			cluster_1.m_path_vertex = vertex;

			for (int l = 0; l < vertex->m_Layer_cnt; ++l)
			{
				++layering->m_Layers[vertex->m_Layers[l]].m_path_selects;
			}
			bool success = false;

			if (c1 < c2)
			{
				if (check_path_LPC_2(last_vertex, c1 + 1, c2, literal_conflict, clustering, layering, limit_cnt, limit))
				{
					success = true;
				}
			}
			else
			{
				success = true;
			}
			for (int l = 0; l < vertex->m_Layer_cnt; ++l)
			{
				--layering->m_Layers[vertex->m_Layers[l]].m_path_selects;
			}
			if (success)
			{
				return true;
			}
		}
	}
	
	return false;
}


void sExperimentalBooleanFormula::print_affects(void) const
{
	printf("Affects: ");
	
	for (int i = 1; i <= m_cA; ++i)
	{
		printf("%d ", m_affcls[i]->m_affects);
	}
	printf("\n");
}


void sExperimentalBooleanFormula::print_path(sClause **path, int length) const
{
	printf("Path: ");

	for (int i = 0; i < length; ++i)
	{
		if (path[i] != NULL)
		{
			printf("%ld ", path[i] - m_clauses);
		}
		else
		{
			break;
		}
	}
	printf("\n");
}


void sExperimentalBooleanFormula::print_path_verbose(sClause **path, int length) const
{
	printf("Path:\n");
	
	for (int i = 0; i < length; ++i)
	{
		if (path[i] != NULL)
		{
			print_clause(path[i]);
			printf("\n");
		}
		else
		{
			break;
		}
	}
	printf("\n");
}


void sExperimentalBooleanFormula::print_DIMACS(void) const
{
	printf("p cnf %d %ld\n", m_V, m_cC + m_binary_Clauses.size());

	for (sBinaryClauses_set::const_iterator bcl = m_binary_Clauses.begin(); bcl != m_binary_Clauses.end(); ++bcl)
	{
		print_literal(bcl->m_l1);
		printf(" ");
		print_literal(bcl->m_l2);
		printf(" 0\n");
	}

	for (int i = 0; i < m_cC; ++i)
	{
		const sClause &clause = m_clauses[i];
		sLiteral **literal = clause.m_literals;
		
		while (*literal != NULL)
		{
			if ((*literal)->m_base == m_P_literals)
			{
				printf("%ld ", *literal - (*literal)->m_base);
			}
			else
			{
				printf("-%ld ", *literal - (*literal)->m_base);
			}
			++literal;
		}
		printf("0\n");
	}
}


/*----------------------------------------------------------------------------*/
// sFlowModel_1 class

int sFlowModel_1::init(sExperimentalBooleanFormula *formula)
{
	m_formula = formula;

	return 0;
}


void sFlowModel_1::destroy(void)
{
	m_conflict_graph.destroy();
	m_network.destroy();
	m_clustering.destroy();
}


int sFlowModel_1::build_Network(int arity, double thickness)
{
/*
	if (arity != 0)
	{
		m_formula->select_Arity(arity);
	}
*/
//	m_formula->select_Undefined();

	int result = m_formula->build_LiteralConflictGraph(&m_conflict_graph);
	if (result < 0)
	{
		return result;
	}
	result = m_formula->build_occurence_clustering_4(&m_conflict_graph, &m_clustering);
	if (result < 0)
	{
		return result;
	}

	int undefs = m_formula->calc_UndefinedClauses();
	double thn = m_formula->calc_CumulativeThickness(&m_clustering);
	printf("Average thickness: %.3f\n", thn / undefs);
//	m_formula->select_Clustering(&m_clustering);
/*
	if (thickness != 0.0)
	{
		m_formula->select_Thickness(thickness);
	}
*/
//	m_clustering.print_nontrivial();
	m_formula->select_ArityThickness(arity, thickness);

	m_formula->calc_ClusteringCapacity(&m_conflict_graph, &m_clustering);

	result = m_network.init(m_formula, sVERTEX_BLOCK_SIZE, sEDGE_BLOCK_SIZE, sNEIGHBOR_BLOCK_SIZE);
	if (result < 0)
	{
		return result;
	}

	result = m_network.add_Vertices(m_formula->m_C);
	if (result < 0)
	{
		return result;
	}
	result = m_network.add_Vertices(m_clustering.get_ClusterCount());
	if (result < 0)
	{
		return result;
	}

	m_clauses_offset = 0;
	m_clusters_offset = m_clauses_offset + m_formula->m_C;

	int contribution = 0;

	m_occurence_Infos = (OccurenceInfo*)malloc(m_clustering.get_ClusterCount() * sizeof(OccurenceInfo));
	if (m_occurence_Infos == NULL)
	{
		return -1;
	}

	int occurences_offset = m_clusters_offset + m_clustering.get_ClusterCount();
	int N_Clusters = m_clustering.get_ClusterCount();

	int cumulative_occ_diff = 0;

	for (int i = 0; i < N_Clusters; ++i)
	{
		sCluster *cluster = m_clustering.get_cluster(i);
		sLiteral *literal = cluster->m_Vertices[0]->m_literal;
		cluster->m_total_capacity = m_formula->calc_SelectedOccurences(literal);
//		cluster->m_total_capacity = literal->m_cO;
//		printf("Literal occurences: %d x %d\n", literal->m_cO, m_formula->calc_SelectedOccurences(literal));
		int occmax = cluster->m_total_capacity;
		int occmin = cluster->m_total_capacity;

		for (int j = 1; j < cluster->m_N_Vertices; ++j)
		{
			literal = cluster->m_Vertices[j]->m_literal;
			int select_occs = m_formula->calc_SelectedOccurences(literal);
//			int undef_occs = literal->m_cO;
			if (occmax < select_occs)
			{
				occmax = select_occs;
			}
			if (occmin > select_occs)
			{
				occmin = select_occs;
			}
			cluster->m_total_capacity += select_occs;
//			printf("  Literal occurences: %d x %d\n", literal->m_cO, m_formula->calc_SelectedOccurences(literal));
		}
		cumulative_occ_diff += occmax - occmin;

		m_occurence_Infos[i].m_offset = occurences_offset;
		m_occurence_Infos[i].m_occmax = occmax;
//		printf("----> Total: tc:%d occamax:%d c:%d min:%d\n", cluster->m_total_capacity, occmax, cluster->m_capacity, sMIN(cluster->m_total_capacity, cluster->m_capacity * occmax));

		result = m_network.add_Vertices(occmax);
		if (result < 0)
		{
			return result;
		}
		occurences_offset += occmax;
		contribution += sMIN(cluster->m_total_capacity, cluster->m_capacity * occmax);
/*
		if (occmax - occmin > 0)
		{
			printf("Difference: %d x %d ... %d\n", occmin, occmax, occmax - occmin);
		}
		else
		{
			printf("Difference: %d x %d\n", occmin, occmax);
		}
*/
	}
	printf("Average difference: %.3f\n", (double)cumulative_occ_diff / N_Clusters);

	result = m_network.add_Vertices(2);
	if (result < 0)
	{
		return result;
	}
	m_source_offset = occurences_offset;
	m_sink_offset = m_source_offset + 1;

	int source_vertex_idx = calc_SourceIndex();
	int sink_vertex_idx = calc_SinkIndex();

	for (int i = 0; i < N_Clusters; ++i)
	{
		sCluster *cluster = m_clustering.get_cluster(i);
		for (int j = 0; j < m_occurence_Infos[i].m_occmax; ++j)
		{
			int cluster_vertex_idx = calc_ClusterIndex(cluster);
			int occ_vertex_idx = calc_OccurenceIndex(cluster, j);
			int capacity = cluster->m_capacity;

			result = m_network.add_DirectedEdge(cluster_vertex_idx, occ_vertex_idx, capacity);
			if (result < 0)
			{
				return result;
			}
			int edge_capacity = sMIN(cluster->m_total_capacity, capacity * m_occurence_Infos[i].m_occmax);
			if (edge_capacity > 0)
			{
				result = m_network.add_DirectedEdge(source_vertex_idx, cluster_vertex_idx, edge_capacity);
				if (result < 0)
				{
					return result;
				}
			}
		}

		for (int k = 0; k < cluster->m_N_Vertices; ++k)
		{
			int occ = 0;
			sLiteral *literal = cluster->m_Vertices[k]->m_literal;
			ONode *onode = literal->m_occs;
			
			while (onode != NULL)
			{
				if (onode->m_clause->m_selected && m_formula->calc_Value(*onode->m_clause) == V_UNDEF)
				{
					int occ_vertex_idx = calc_OccurenceIndex(cluster, occ);
					int clause_vertex_idx = calc_ClauseIndex(onode->m_clause);
					
					result = m_network.add_DirectedEdge(occ_vertex_idx, clause_vertex_idx, 1);
					if (result < 0)
					{
						return result;
					}
					++occ;
				}
				onode = onode->m_next;
			}
		}
	}
	for (int i = 0; i < m_formula->m_C; ++i)
	{
		sClause *clause = &m_formula->m_clauses[i];
		if (clause->m_selected && m_formula->calc_Value(*clause) == V_UNDEF)
		{
			int clause_vertex_idx = calc_ClauseIndex(clause);
			result = m_network.add_DirectedEdge(clause_vertex_idx, sink_vertex_idx, 1);
			if (result < 0)
			{
				return result;
			}
		}
	}

//	m_network.print_Directed();
//	to_ScreenInfo();
//	printf("Contribution: %d\n", contribution);
//	printf("Selected: %d\n", m_formula->calc_SelectedClauses());

	return 0;
}


int sFlowModel_1::calc_ClauseIndex(const sClause *clause) const
{
	return m_clauses_offset + clause - m_formula->m_clauses;
}


int sFlowModel_1::calc_ClusterIndex(const sCluster *cluster) const
{
	return m_clusters_offset + cluster->m_idx;
}


int sFlowModel_1::calc_OccurenceIndex(const sCluster *cluster, int occ_idx) const
{
	return m_occurence_Infos[cluster->m_idx].m_offset + occ_idx;
}


int sFlowModel_1::calc_SourceIndex(void) const
{
	return m_source_offset;
}


int sFlowModel_1::calc_SinkIndex(void) const
{
	return m_sink_offset;
}


void sFlowModel_1::to_Screen(void) const
{
	m_conflict_graph.print();
	m_network.print();
	m_clustering.print();

	to_ScreenInfo();
}


void sFlowModel_1::to_ScreenInfo(void) const
{
	printf("Clauses offset: %d\n", m_clauses_offset);
	printf("Clusters offset: %d\n", m_clusters_offset);

	printf("Occurence offsets/number of occurences: ");
	for (int i = 0; i < m_clustering.get_ClusterCount(); ++i)
	{
		printf("%d:%d/%d ", i, m_occurence_Infos[i].m_offset, m_occurence_Infos[i].m_occmax);
	}
	printf("\nSource offset: %d\n", m_source_offset);
	printf("Sink offset: %d\n", m_sink_offset);
}


/*----------------------------------------------------------------------------*/
// Global functions

void swap_affects(sClause *clause_1, sClause *clause_2)
{
	sClause *temp, **tmp;

	temp = *clause_1->m_sort;
	*clause_1->m_sort = *clause_2->m_sort;
	*clause_2->m_sort = temp;

	tmp = clause_1->m_sort;
	clause_1->m_sort = clause_2->m_sort;
	clause_2->m_sort = tmp;
}

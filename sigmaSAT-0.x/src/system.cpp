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
// system.cpp / 0.71-sky
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
// Systematic module of the sigmaSAT solver.

#include <stdlib.h>
#include <stdlib.h>
#include <time.h>

#include <list>

#include "defs.h"
#include "config.h"
#include "system.h"
#include "version.h"


int STAT_DECISIONS = 0;
int STAT_PROPAGATIONS = 0;
int STAT_BACKTRACKS = 0;


/*----------------------------------------------------------------------------*/
// sClauseStore class

sClauseStore::sClauseStore()
{
	m_first_node = new sClauseNode();
	assert(m_first_node != NULL);

	m_first_node->m_next = NULL;
	m_first_node->m_Clause_cnt = 0;

	m_first_node->m_Clauses = new sClause[sCLAUSE_BLOCK_SIZE];
	assert(m_first_node->m_Clauses != NULL);

	m_first_node->m_N_Clauses = sCLAUSE_BLOCK_SIZE;
}


sClauseStore::~sClauseStore()
{
	sClauseNode *clause_node = m_first_node;

	while (clause_node != NULL)
	{
		sClauseNode *next_node = clause_node->m_next;
		delete clause_node->m_Clauses;
		delete clause_node;

		clause_node = next_node;
	}
}


sClause* sClauseStore::alloc_Clause(void)
{
	if (m_first_node->m_Clause_cnt >= m_first_node->m_N_Clauses)
	{
		sClauseNode *clause_node =  new sClauseNode();
		assert(clause_node != NULL);

		clause_node->m_next = m_first_node;
		clause_node->m_Clause_cnt = 0;

		clause_node->m_Clauses = new sClause[sCLAUSE_BLOCK_SIZE];
		assert(clause_node->m_Clauses != NULL);

		clause_node->m_N_Clauses = sCLAUSE_BLOCK_SIZE;
		m_first_node = clause_node;
	}

	return &m_first_node->m_Clauses[m_first_node->m_N_Clauses++];
}


/*----------------------------------------------------------------------------*/
// sClause class

sClause::sClause()
{
	m_literals = NULL;
}


sClause::sClause(const sClause &clause)
{
	m_literals = new sLiteral*[clause.m_cL + 1];
	assert(m_literals != NULL);
	m_literals[clause.m_cL] = NULL;

	sLiteral **literal_1 = m_literals;
	sLiteral **literal_2 = clause.m_literals;

	while ((*literal_2) != NULL)
	{
		*(literal_1++) = *(literal_2++);
	}
	m_cL = clause.m_cL;
}


const sClause& sClause::operator=(const sClause &clause)
{
	if (clause.m_literals != NULL)
	{
		if (m_literals != NULL)
		{
			if (m_cL < clause.m_cL)
			{
				delete m_literals;
			
				m_literals = new sLiteral*[clause.m_cL + 1];
				assert(m_literals != NULL);
			}
			m_literals[clause.m_cL] = NULL;
		}
		else
		{
			m_literals = new sLiteral*[clause.m_cL + 1];
			assert(m_literals != NULL);
			m_literals[clause.m_cL] = NULL;
		}

		sLiteral **literal_1 = m_literals;
		sLiteral **literal_2 = clause.m_literals;

		while ((*literal_2) != NULL)
		{
			*(literal_1++) = *(literal_2++);
		}
		m_cL = clause.m_cL;
	}
	else
	{
		if (m_literals != NULL)
		{
			delete m_literals;
		}
		m_literals = NULL;
	}

	return *this;
}


sClause::~sClause()
{
	if (m_literals != NULL)
	{
		delete m_literals;
	}
}


bool sClause::is_Null(void) const
{
	return (m_literals == NULL);
}


bool sClause::calc_Termination(void) const
{
	sLiteral **literal = m_literals;

	while (*literal != NULL)
	{
		sAssignment *assignment = (*literal)->m_variable->m_assignment;
		if (assignment != NULL && assignment->m_depth >= 0)
		{
			return false;
		}
		++literal;
	}
	return true;
}

int sClause::calc_PreviousDepth(int current_depth) const
{
	int prev_depth = -1;
	--current_depth;

	sLiteral **literal = m_literals;
	while (*literal != NULL)
	{
		if ((*literal)->m_variable->m_assignment != NULL)
		{
			int depth = (*literal)->m_variable->m_assignment->m_depth;

			if (depth > prev_depth && depth <= current_depth)
			{
				prev_depth = depth;

				if (prev_depth >= current_depth)
				{
					break;
				}
			}
		}
		++literal;
	}
	
	return prev_depth;
}


void sClause::to_Screen(const sBooleanFormula *formula) const
{
	if (m_literals != NULL)
	{
		for (int i = 0; i < m_cL; ++i)
		{
			formula->print_literal(m_literals[i]);
			if (m_literals[i]->m_variable->m_assignment != NULL)
			{
				printf("(a:%ld,d:%d)", m_literals[i]->m_variable->m_assignment - formula->m_Assignments, m_literals[i]->m_variable->m_assignment->m_depth);
				printf("-%d ", m_literals[i]->m_value);
			}
			else
			{
				printf("(nil)");
				printf("-%d ", m_literals[i]->m_value);
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
// sBooleanFormula

void sBooleanFormula::init(int n_V, int n_C, int n_O)
{
	m_V = n_V;
	m_C = n_C;
	m_O = n_O;
	
	m_clauses = (sClause*)malloc(n_C * sizeof(sClause));
	assert(m_clauses != NULL);
	
	m_affcls = (sClause**)malloc((n_C + 1)* sizeof(sClause*));
	assert(m_affcls != NULL);
	
	m_P_literals = (sLiteral*)malloc((n_V + 1) * sizeof(sLiteral));
	assert(m_P_literals != NULL);

	m_N_literals = (sLiteral*)malloc((n_V + 1) * sizeof(sLiteral));
	assert(m_N_literals != NULL);

	m_variables = (sVariable*)malloc((n_V + 1) * sizeof(sVariable));
	assert(m_variables != NULL);

	m_Assignments = (sAssignment*)malloc(n_V * sizeof(sAssignment));
	assert(m_Assignments != NULL);

	m_ec_Nodes = (sECNode**)malloc(n_V * sizeof(sECNode*));
	assert(m_ec_Nodes != NULL);

	m_assignment_end = m_Assignments;
	
	m_occurences = (sLiteral**)malloc(n_O * sizeof(sLiteral*));
	assert(m_occurences != NULL);
	
	m_L_occurences = (ONode*)malloc(n_O * sizeof(ONode));
	assert(m_L_occurences != NULL);
	
	m_watch_nodes = (WNode*)malloc(2 * n_C * sizeof(WNode));
	assert(m_watch_nodes != NULL);
	
	m_unit_literals = (sLiteral**)malloc(2 * n_V * sizeof(sLiteral*));
	assert(m_unit_literals != NULL);
	
	m_last_unit = m_unit_literals;
	
	m_chng_literals = (sLiteral**)malloc(2 * n_V * sizeof(sLiteral*));
	assert(m_chng_literals != NULL);
	
	m_last_chng = m_chng_literals;
	
	m_cLO = m_cO = m_cC = m_cW = m_cA = 0;
	
	for (int i = 0; i <= n_V; ++i)
	{
		sVariable &variable = m_variables[i];
		sLiteral &p_literal = m_P_literals[i];
		sLiteral &n_literal = m_N_literals[i];
		
		p_literal.m_value = V_UNDEF;
		p_literal.m_variable = &variable;
		p_literal.m_base = m_P_literals;
		
		p_literal.m_watch_1_list.m_next = &p_literal.m_watch_1_list;
		p_literal.m_watch_1_list.m_prev = &p_literal.m_watch_1_list;
		p_literal.m_watch_1_list.m_watch = NULL;
		
		p_literal.m_watch_2_list.m_next = &p_literal.m_watch_2_list;
		p_literal.m_watch_2_list.m_prev = &p_literal.m_watch_2_list;
		p_literal.m_watch_2_list.m_watch = NULL;
		p_literal.m_cO = 0;
		p_literal.m_occs = NULL;
		p_literal.m_cVO = 0;
		p_literal.m_voccs = NULL;
		
		p_literal.m_cmpl = &n_literal;
		
		n_literal.m_value = V_UNDEF;
		n_literal.m_variable = &variable;
		n_literal.m_base = m_N_literals;
		
		n_literal.m_watch_1_list.m_next = &n_literal.m_watch_1_list;
		n_literal.m_watch_1_list.m_prev = &n_literal.m_watch_1_list;
		n_literal.m_watch_1_list.m_watch = NULL;
		
		n_literal.m_watch_2_list.m_next = &n_literal.m_watch_2_list;
		n_literal.m_watch_2_list.m_prev = &n_literal.m_watch_2_list;
		n_literal.m_watch_2_list.m_watch = NULL;
		n_literal.m_cO = 0;
		n_literal.m_occs = NULL;
		n_literal.m_cVO = 0;
		n_literal.m_voccs = NULL;
		
		n_literal.m_cmpl = &p_literal;
		
		variable.m_positive = &p_literal;
		variable.m_negative = &n_literal;
	}
	
	m_head_clause = m_clauses;
	m_head_var = m_variables + 1;

	m_resolvent_A.m_cL = 0;
	m_resolvent_A.m_literals = (sLiteral**)malloc((n_V + 1) * sizeof(sLiteral*));
	assert(m_resolvent_A.m_literals != NULL);

	m_resolvent_B.m_cL = 0;
	m_resolvent_B.m_literals = (sLiteral**)malloc((n_V + 1) * sizeof(sLiteral*));
	assert(m_resolvent_B.m_literals != NULL);

	m_decision_Records = (sDecisionRecord*)malloc(n_V * sizeof(sDecisionRecord));
	assert(m_decision_Records != NULL);
}


void sBooleanFormula::add_2_clause(int l1, int l2)
{
	int lits[] = {l1, l2, 0};
	add_general_clause(lits);
}


void sBooleanFormula::add_3_clause(int l1, int l2, int l3)
{
	int lits[] = {l1, l2, l3, 0};
	
	add_general_clause(lits);
}


void sBooleanFormula::add_general_clause(int *lits)
{
	sClause &clause = m_clauses[m_cC];
	clause.m_literals = &m_occurences[m_cO];
	
	clause.m_sort = NULL;
	clause.m_affects = 0;
	clause.m_in_path = false;
	
	clause.m_cL = 0;
	int *l = lits;
		
	while (*l != 0)
	{
		++m_cO;
		
		sLiteral *&literal = clause.m_literals[clause.m_cL];
		
		if (*l < 0)
		{
			literal = &m_N_literals[-*l];
		}
		else
		{
			literal = &m_P_literals[*l];
		}
		++literal->m_cO;
		
		ONode &onode = m_L_occurences[m_cLO];
		onode.m_next = literal->m_occs;
		
		onode.m_clause = &clause;
		++m_cLO;
		literal->m_occs = &onode;
		
		++l;
		++clause.m_cL;
	}
	
	assert(clause.m_cL >= 1);
	
	if (clause.m_cL == 1)
	{
		if (clause.m_literals[0]->m_value != V_FALSE)
		{
			clause.m_literals[0]->m_value = V_TRUE;
			clause.m_literals[0]->m_cmpl->m_value = V_FALSE;
			clause.m_literals[0]->m_variable->m_assignment = NULL;
			
			*m_last_unit++ = clause.m_literals[0];
			*m_last_chng++ = clause.m_literals[0];
		}
		else
		{
			m_conflicting = true;
		}
	}
	
	clause.m_watch_1.m_clause = &clause;
	clause.m_watch_1.m_literal = &clause.m_literals[0];
	
	m_watch_nodes[m_cW].m_watch = &clause.m_watch_1;
	insert_watch(&(*clause.m_watch_1.m_literal)->m_watch_1_list, &m_watch_nodes[m_cW]);
	++m_cW;
	
	clause.m_watch_2.m_clause = &clause;
	clause.m_watch_2.m_literal = &clause.m_literals[clause.m_cL - 1];
	
	m_watch_nodes[m_cW].m_watch = &clause.m_watch_2;
	insert_watch(&(*clause.m_watch_2.m_literal)->m_watch_2_list, &m_watch_nodes[m_cW]);
	++m_cW;
	
	m_occurences[m_cO] = NULL;
	++m_cO;
	++m_cC;
}


void sBooleanFormula::set_TRUE(sLiteral *literal)
{
	literal->m_value = V_TRUE;
	literal->m_cmpl->m_value = V_FALSE;
}


void sBooleanFormula::set_FALSE(sLiteral *literal)
{
	literal->m_value = V_FALSE;
	literal->m_cmpl->m_value = V_TRUE;
}


void sBooleanFormula::unset(sLiteral *literal)
{
	literal->m_value = V_UNDEF;
	literal->m_cmpl->m_value = V_UNDEF;
}


void sBooleanFormula::operate_Set_TRUE(sLiteral *literal)
{
	set_TRUE(literal);

	*m_last_chng++ = literal;

	m_assignment_end->m_ante_clause = NULL;
	m_assignment_end->m_variable = literal->m_variable;
	literal->m_variable->m_assignment = m_assignment_end++;
}


void sBooleanFormula::operate_Set_FALSE(sLiteral *literal)
{
	set_FALSE(literal);

	*m_last_chng++ = literal->m_cmpl;

	m_assignment_end->m_ante_clause = NULL;
	m_assignment_end->m_variable = literal->m_variable;
	literal->m_variable->m_assignment = m_assignment_end++;
}


void sBooleanFormula::operate_Set_TRUE(sLiteral *literal, sClause *antecedent_clause, int depth)
{
	set_TRUE(literal);

	*m_last_chng++ = literal;

	m_assignment_end->m_depth = depth;
	m_assignment_end->m_ante_clause = antecedent_clause;
	m_assignment_end->m_variable = literal->m_variable;
	literal->m_variable->m_assignment = m_assignment_end++;
}


void sBooleanFormula::operate_Set_FALSE(sLiteral *literal, sClause *antecedent_clause, int depth)
{
	set_FALSE(literal);

	*m_last_chng++ = literal->m_cmpl;

	m_assignment_end->m_depth = depth;
	m_assignment_end->m_ante_clause = antecedent_clause;
	m_assignment_end->m_variable = literal->m_variable;
	literal->m_variable->m_assignment = m_assignment_end++;
}


void sBooleanFormula::operate_Unset(sLiteral *literal)
{
	unset(literal);

	--m_last_chng;
	--m_assignment_end;
	literal->m_variable->m_assignment = NULL;
}


bool sBooleanFormula::operate_SetPropagate_TRUE(sLiteral *literal)
{
	*m_last_unit++ = literal;
	operate_Set_TRUE(literal);
	
	return operate_PropagateUnit();
}


bool sBooleanFormula::operate_SetPropagate_FALSE(sLiteral *literal)
{
	*m_last_unit++ = literal->m_cmpl;
	operate_Set_FALSE(literal);
	
	return operate_PropagateUnit();
}


bool sBooleanFormula::operate_SetPropagateAnalyze_TRUE(sLiteral *literal, sClause **conflict_clause, int depth)
{
	*m_last_unit++ = literal;
	operate_Set_TRUE(literal, NULL, depth);
	
	return operate_PropagateUnitAnalyze(literal->m_variable, depth, conflict_clause);
}


bool sBooleanFormula::operate_SetPropagateAnalyze_FALSE(sLiteral *literal, sClause **conflict_clause, int depth)
{
	*m_last_unit++ = literal->m_cmpl;
	operate_Set_FALSE(literal, NULL, depth);
	
	return operate_PropagateUnitAnalyze(literal->m_variable, depth, conflict_clause);
}


bool sBooleanFormula::operate_SetPropagateAnalyze_TRUE(sLiteral *literal, sClause *antecedent_clause, sClause **conflict_clause, int depth)
{
	assert(antecedent_clause != NULL);
	*m_last_unit++ = literal;
	operate_Set_TRUE(literal, antecedent_clause, depth);

	return operate_PropagateUnitAnalyze(literal->m_variable, depth, conflict_clause);
}


bool sBooleanFormula::operate_SetPropagateAnalyze_FALSE(sLiteral *literal, sClause *antecedent_clause, sClause **conflict_clause, int depth)
{
	assert(antecedent_clause != NULL);
	*m_last_unit++ = literal->m_cmpl;
	operate_Set_FALSE(literal, antecedent_clause, depth);

	return operate_PropagateUnitAnalyze(literal->m_variable, depth, conflict_clause);
}


sLiteral** sBooleanFormula::get_CheckPoint(void) const
{
	return m_last_chng;
}

	
void sBooleanFormula::operate_UnsetUnpropagate(void)
{
	operate_UnsetUnpropagate(m_chng_literals);
}


void sBooleanFormula::operate_UnsetUnpropagate(sLiteral **check_point)
{
	while (m_last_chng > check_point)
	{
		(*(--m_last_chng))->m_variable->m_assignment = NULL;
		unset(*(m_last_chng));

		--m_assignment_end;
	}
	m_last_unit = m_unit_literals;
}


bool sBooleanFormula::operate_PropagateUnit(void)
{
	while (m_last_unit > m_unit_literals)
	{
#ifdef STAT
		++STAT_PROPAGATIONS;
#endif	
		sLiteral *false_literal = (*--m_last_unit)->m_cmpl;
		WNode *wnode_1 = false_literal->m_watch_1_list.m_next;
		
		while (wnode_1->m_watch != NULL)
		{
			sLiteral **literal_0 = wnode_1->m_watch->m_literal;
			sLiteral **literal_1 = literal_0;
			sLiteral **literal_2 = wnode_1->m_watch->m_clause->m_watch_2.m_literal;
			
			WNode *node = wnode_1;
			wnode_1 = wnode_1->m_next;
				
			while (true)
			{
				if (*++literal_1 == NULL)
				{
					literal_1 = node->m_watch->m_clause->m_literals;
				}
				if ((*literal_1)->m_value == V_TRUE)
				{
					break;
				}
				else
				{
					if (literal_1 != literal_2)
					{
						if ((*literal_1)->m_value == V_UNDEF)
						{
							node->m_watch->m_clause->m_watch_1.m_literal = literal_1;
							
							remove_watch(&(*literal_0)->m_watch_1_list, node);
							insert_watch(&(*literal_1)->m_watch_1_list, node);
							break;
						}
					}		
					if (literal_1 == literal_0)
					{
						if ((*literal_2)->m_value != V_TRUE)
						{
							if ((*literal_2)->m_value != V_UNDEF)
							{
								return false;
							}
							*m_last_unit++ = *literal_2;
							operate_Set_TRUE(*literal_2);
						}
						break;
					}
				}
			}
		}
			
		WNode *wnode_2 = false_literal->m_watch_2_list.m_next;
		
		while (wnode_2->m_watch != NULL)
		{
			sLiteral **literal_0 = wnode_2->m_watch->m_literal;
			sLiteral **literal_2 = literal_0;
			sLiteral **literal_1 = wnode_2->m_watch->m_clause->m_watch_1.m_literal;
			
			WNode *node = wnode_2;
			wnode_2 = wnode_2->m_next;
			
			while (true)
			{
				if (*++literal_2 == NULL)
				{
					literal_2 = node->m_watch->m_clause->m_literals;
				}
				if ((*literal_2)->m_value == V_TRUE)
				{
					break;
				}
				else
				{
					if (literal_2 != literal_1)
					{
						if ((*literal_2)->m_value == V_UNDEF)
						{
							node->m_watch->m_clause->m_watch_2.m_literal = literal_2;
							remove_watch(&(*literal_0)->m_watch_2_list, node);
							insert_watch(&(*literal_2)->m_watch_2_list, node);
							break;
						}
					}
					
					if (literal_2 == literal_0)
					{
						if ((*literal_1)->m_value != V_TRUE)
						{
							if ((*literal_1)->m_value != V_UNDEF)
							{
								return false;
							}
							*m_last_unit++ = *literal_1;
							operate_Set_TRUE(*literal_1);
						}
						break;
					}
				}
			}
		}
	}
	
	return true;
}


bool sBooleanFormula::operate_PropagateUnitAnalyze(sVariable *selected_variable, int depth, sClause **conflict_clause)
{
//	const sAssignment *decision_assignment = m_assignment_end - 1;

	while (m_last_unit > m_unit_literals)
	{
#ifdef STAT
		++STAT_PROPAGATIONS;
#endif
		sLiteral *true_literal = *--m_last_unit;
		operate_Set_TRUE(true_literal);
		
		sLiteral *false_literal = true_literal->m_cmpl;
		WNode *wnode_1 = false_literal->m_watch_1_list.m_next;
		
		while (wnode_1->m_watch != NULL)
		{
			sLiteral **literal_0 = wnode_1->m_watch->m_literal;
			sLiteral **literal_1 = literal_0;
			sLiteral **literal_2 = wnode_1->m_watch->m_clause->m_watch_2.m_literal;
			
			WNode *node = wnode_1;
			wnode_1 = wnode_1->m_next;
				
			while (true)
			{
				if (*++literal_1 == NULL)
				{
					literal_1 = node->m_watch->m_clause->m_literals;
				}
				if ((*literal_1)->m_value == V_TRUE || (*literal_1)->m_value == V_TRUE)
				{
					break;
				}
				else
				{
					if (literal_1 != literal_2)
					{
						if ((*literal_1)->m_value == V_UNDEF)
						{
							node->m_watch->m_clause->m_watch_1.m_literal = literal_1;
							
							remove_watch(&(*literal_0)->m_watch_1_list, node);
							insert_watch(&(*literal_1)->m_watch_1_list, node);
							break;
						}
					}		
					if (literal_1 == literal_0)
					{
						if ((*literal_2)->m_value != V_TRUE)
						{
							if ((*literal_2)->m_value != V_UNDEF)
							{
								*conflict_clause = generate_ConflictClause(selected_variable, node->m_watch->m_clause, depth);
								return false;
							}
							
							(*literal_2)->m_value = V_TRUE;
							(*literal_2)->m_cmpl->m_value = V_FALSE;
							
							*m_last_unit++ = *literal_2;
							*m_last_chng++ = *literal_2;

							m_assignment_end->m_depth = depth;
							m_assignment_end->m_ante_clause = node->m_watch->m_clause;
							m_assignment_end->m_variable = (*literal_2)->m_variable;
							(*literal_2)->m_variable->m_assignment = m_assignment_end++;
						}
						break;
					}
				}
			}
		}
			
		WNode *wnode_2 = false_literal->m_watch_2_list.m_next;
		
		while (wnode_2->m_watch != NULL)
		{
			sLiteral **literal_0 = wnode_2->m_watch->m_literal;
			sLiteral **literal_2 = literal_0;
			sLiteral **literal_1 = wnode_2->m_watch->m_clause->m_watch_1.m_literal;
			
			WNode *node = wnode_2;
			wnode_2 = wnode_2->m_next;
			
			while (true)
			{
				if (*++literal_2 == NULL)
				{
					literal_2 = node->m_watch->m_clause->m_literals;
				}
				if ((*literal_2)->m_value == V_TRUE || (*literal_2)->m_value == V_TRUE)
				{
					break;
				}
				else
				{
					if (literal_2 != literal_1)
					{
						if ((*literal_2)->m_value == V_UNDEF)
						{
							node->m_watch->m_clause->m_watch_2.m_literal = literal_2;
							remove_watch(&(*literal_0)->m_watch_2_list, node);
							insert_watch(&(*literal_2)->m_watch_2_list, node);
							break;
						}
					}
					
					if (literal_2 == literal_0)
					{
						if ((*literal_1)->m_value != V_TRUE)
						{
							if ((*literal_1)->m_value != V_UNDEF)
							{
								*conflict_clause = generate_ConflictClause(selected_variable, node->m_watch->m_clause, depth);
								return false;
							}
							
							(*literal_1)->m_value = V_TRUE;
							(*literal_1)->m_cmpl->m_value = V_FALSE;
							
							*m_last_unit++ = *literal_1;
							*m_last_chng++ = *literal_1;

							m_assignment_end->m_depth = depth;
							m_assignment_end->m_ante_clause = node->m_watch->m_clause;
							m_assignment_end->m_variable = (*literal_1)->m_variable;
							(*literal_1)->m_variable->m_assignment = m_assignment_end++;
						}
						break;
					}
				}
			}
		}
	}
	
	return true;
}


bool sBooleanFormula::solve_1(void)
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
		
		return solve_1_rec(0);
	}
}


bool sBooleanFormula::solve_1_rec(int depth)
{
	sClause *head_clause_save = m_head_clause;
	sVariable *head_var_save = m_head_var;
	sAssignment *assignment_end_save = m_assignment_end;

#ifdef STAT
	++STAT_DECISIONS;
#endif

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
		operate_UnsetUnpropagate(check_point);
	}

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


bool sBooleanFormula::solve_2(void)
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
		
		return solve_2_flat();
	}
}


bool sBooleanFormula::solve_2_flat(void)
{
	int depth = 0;

	sDecisionRecord &bt_record = m_decision_Records[depth];
	bt_record.m_phase.m_bt = BT_PHASE_INITIAL;

	while (true)
	{
		sDecisionRecord &bt_record = m_decision_Records[depth];

		switch (bt_record.m_phase.m_bt)
		{
		case BT_PHASE_INITIAL:
		{
#ifdef STAT
			++STAT_DECISIONS;
#endif
			bt_record.m_head_clause = m_head_clause;
			bt_record.m_head_var = m_head_var;

			while (calc_Value(*m_head_clause) == V_TRUE)
			{
				++m_head_clause;
			
				if (m_head_clause - m_clauses >= m_cC)
				{
					return true;
				}
			}
			bt_record.m_selected_var = select_Variable_1(m_head_var);
			bt_record.m_check_point = get_CheckPoint();

			bt_record.m_phase.m_bt = BT_PHASE_POSITIVE;
			break;
		}
		case BT_PHASE_POSITIVE:
		{
			if (operate_SetPropagate_TRUE(bt_record.m_selected_var->m_positive))
			{
				sDecisionRecord &new_bt_record = m_decision_Records[++depth];
				new_bt_record.m_phase.m_bt = BT_PHASE_INITIAL;
			}
			else
			{
				operate_UnsetUnpropagate(bt_record.m_check_point);
			}
			bt_record.m_phase.m_bt = BT_PHASE_NEGATIVE;
			break;
		}
		case BT_PHASE_NEGATIVE:
		{
#ifdef STAT
			++STAT_DECISIONS;
#endif

			if (operate_SetPropagate_FALSE(bt_record.m_selected_var->m_positive))
			{
				sDecisionRecord &new_bt_record = m_decision_Records[++depth];
				new_bt_record.m_phase.m_bt = BT_PHASE_INITIAL;
			}
			else
			{
				operate_UnsetUnpropagate(bt_record.m_check_point);
			}
			bt_record.m_phase.m_bt = BT_PHASE_TERMINAL;
			break;
		}
		case BT_PHASE_TERMINAL:
		{
#ifdef STAT
			++STAT_BACKTRACKS;
#endif

			if (depth > 0)
			{
				sDecisionRecord &old_bt_record = m_decision_Records[--depth];

				m_head_clause = old_bt_record.m_head_clause;
				m_head_var = old_bt_record.m_head_var;

				operate_UnsetUnpropagate(old_bt_record.m_check_point);
			}
			else
			{
				return false;
			}
			break;
		}
		default:
		{
			assert(false);
			break;
		}
		}

	}
}


bool sBooleanFormula::solve_3(void)
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

		return solve_3_flat();
	}
}


bool sBooleanFormula::solve_3_flat(void)
{
	int depth = 0;

	sDecisionRecord &bj_record = m_decision_Records[depth];
	bj_record.m_phase.m_bj = BJ_PHASE_INITIAL;

	while (true)
	{
		sDecisionRecord &bj_record = m_decision_Records[depth];

		switch (bj_record.m_phase.m_bj)
		{
		case BJ_PHASE_INITIAL:
		{
#ifdef STAT
			++STAT_DECISIONS;
#endif
			bj_record.m_head_clause = m_head_clause;
			bj_record.m_head_var = m_head_var;

			while (calc_Value(*m_head_clause) == V_TRUE)
			{
				++m_head_clause;
			
				if (m_head_clause - m_clauses >= m_cC)
				{
					return true;
				}
			}
			bj_record.m_selected_var = select_Variable_1(m_head_var);
			bj_record.m_check_point = get_CheckPoint();
			bj_record.m_phase.m_bj = BJ_PHASE_PROPAGATION;
			break;
		}
		case BJ_PHASE_PROPAGATION:
		{
			sClause *positive_conflict;

			if (!operate_SetPropagateAnalyze_TRUE(bj_record.m_selected_var->m_positive, &positive_conflict, depth))
			{
				if (positive_conflict->is_Null())
				{
					return false;
				}
				int bj_depth = positive_conflict->calc_PreviousDepth(depth);

				sClause *secondary_conflict;
				sClause *secondary_conflict_ante = new sClause();
				*secondary_conflict_ante = *positive_conflict;

				while (true)
				{
					sDecisionRecord &old_bj_record = m_decision_Records[bj_depth + 1];
						
					m_head_clause = old_bj_record.m_head_clause;
					m_head_var = old_bj_record.m_head_var;
						
					operate_UnsetUnpropagate(old_bj_record.m_check_point);
					depth = bj_depth;
				
					sLiteral **literal_to_set = NULL;
					sLiteral **literal = secondary_conflict_ante->m_literals;
					
					while (*literal != NULL)
					{
						if ((*literal)->m_variable->m_assignment == NULL && (*literal)->m_value == V_UNDEF)
						{
							assert((*literal)->m_value == V_UNDEF);
							literal_to_set = literal;
							break;
						}
						literal++;
					}

					assert(literal_to_set != NULL);
				
					if (operate_SetPropagateAnalyze_TRUE(*literal_to_set, secondary_conflict_ante, &secondary_conflict, depth))
					{
						break;
					}
					else
					{
						if (secondary_conflict == NULL)
						{
							return false;
						}
					}
					
					secondary_conflict_ante = new sClause();
					*secondary_conflict_ante = *secondary_conflict;
					bj_depth = secondary_conflict->calc_PreviousDepth(depth);
#ifdef STAT	
					++STAT_BACKTRACKS;
#endif		
				}
			}
			sDecisionRecord &new_bj_record = m_decision_Records[++depth];
			new_bj_record.m_phase.m_bj = BJ_PHASE_INITIAL;
			break;
		}
		default:
		{
			assert(false);
			break;
		}
		}

	}
}


sVariable* sBooleanFormula::select_Variable_1(sVariable *hint) const
{
	sVariable *variable = (hint != NULL) ? hint : m_variables;
	
	while (variable->m_positive->m_value != V_UNDEF)
	{
		++variable;
	}
	
	return variable;
}


sVariable* sBooleanFormula::select_Variable_2(sVariable *hint) const
{
    int rnd = rand() % m_V;
    sVariable *variable = (hint != NULL) ? hint : m_variables;

    while (true)
    {
	if (variable->m_positive->m_value == V_UNDEF)
	{
	    if (rnd-- == 0)
	    {
		return variable;
	    }
	}
	if (++variable == m_variables + m_V)
	{
	    variable = m_variables;
	}
    }
}

/*
void sBooleanFormula::push_ExternalClause(sExternalClause *ext_clause, int depth)
{
	sECNode *ec_node = new sECNode();
	ec_node->m_ext_clause = ext_clause;
	ec_node->m_next = m_ec_Nodes[depth];
	m_ec_Nodes[depth] = ec_node;
}


void sBooleanFormula::erase_ExternalClauses(int depth)
{
	sECNode *ec_node = m_ec_Nodes[depth];
	while (ec_node != NULL)
	{
		sECNode *ec_node_erase = ec_node;
		ec_node = ec_node->m_next;

		delete ec_node_erase->m_ext_clause;
		delete ec_node_erase;
	}
}
*/

sClause* sBooleanFormula::generate_ConflictClause(sClause *conflict, const sAssignment *decision_assignment)
{
	sAssignment *assignment = m_assignment_end - 1;

	const sClause *clause_1 = conflict;
	const sClause *clause_2 = assignment->m_ante_clause;
	sVariable *variable = assignment->m_variable;
	sClause *resolvent = &m_resolvent_A;

	while (true)
	{
		resolve_Clauses(clause_1, clause_2, variable, resolvent);
		clause_1 = resolvent;

		int current = 0;

		sLiteral **resolvent_literal = resolvent->m_literals;
		variable = (*resolvent_literal++)->m_variable;

		if (variable->m_assignment >= decision_assignment)
		{
			++current;
		}
		while (*resolvent_literal != NULL)
		{
			if ((*resolvent_literal)->m_variable->m_assignment >= decision_assignment)
			{
				++current;
			}
			if ((*resolvent_literal)->m_variable->m_assignment > variable->m_assignment)
			{
				variable = (*resolvent_literal)->m_variable;
			}
			++resolvent_literal;
		}
		if (current <= 1)
		{
			return resolvent;
		}
		clause_2 = variable->m_assignment->m_ante_clause;

		if (resolvent == &m_resolvent_A)
		{
			resolvent = &m_resolvent_B;
		}
		else
		{
			resolvent = &m_resolvent_A;
		}
	}
}


sClause* sBooleanFormula::generate_ConflictClause(sVariable *sUNUSED(selected_variable), sClause *conflict, int depth)
{
	if (conflict->calc_Termination())
	{
		return NULL;
	}

	sAssignment *assignment = m_assignment_end - 1;
	sVariable *variable = assignment->m_variable;

	const sClause *clause_1 = conflict;
	const sClause *clause_2 = variable->m_assignment->m_ante_clause;
	sClause *resolvent = &m_resolvent_A;

	while (true)
	{
		assert(clause_1 != NULL);
		assert(clause_2 != NULL);

		resolve_Clauses(clause_1, clause_2, variable, resolvent);
		clause_1 = resolvent;

		int current = 0;

		sLiteral **resolvent_literal = resolvent->m_literals;
		variable = (*resolvent_literal++)->m_variable;

		bool contained = false;
	
		if (variable->m_assignment != NULL)
		{
			if (variable->m_assignment->m_depth >= depth)
			{
				if (variable->m_assignment->m_ante_clause == NULL)
				{
					contained = true;
				}
				++current;
			}
		}
		while (*resolvent_literal != NULL)
		{
			if ((*resolvent_literal)->m_variable->m_assignment != NULL)
			{
				if ((*resolvent_literal)->m_variable->m_assignment->m_depth >= depth)
				{
					if ((*resolvent_literal)->m_variable->m_assignment->m_ante_clause == NULL)
					{
						contained = true;
					}
					++current;
				}
				if ((*resolvent_literal)->m_variable->m_assignment > variable->m_assignment)
				{
					variable = (*resolvent_literal)->m_variable;
				}
			}
			++resolvent_literal;
		}

		if (current <= 1 && contained)
		{
			return resolvent;
		}
		clause_2 = variable->m_assignment->m_ante_clause;

		if (resolvent == &m_resolvent_A)
		{
			resolvent = &m_resolvent_B;
		}
		else
		{
			resolvent = &m_resolvent_A;
		}
	}
}


void sBooleanFormula::resolve_Clauses(const sClause *clause_1, const sClause *clause_2, sVariable *variable, sClause *resolvent)
{
	sLiteral **literal_1 = clause_1->m_literals;
	while (*literal_1 != NULL)
	{
		(*literal_1++)->m_resolved = false;
	}

	sLiteral **literal_2 = clause_2->m_literals;
	while (*literal_2 != NULL)
	{
		(*literal_2++)->m_resolved = false;
	}
	variable->m_positive->m_resolved = variable->m_negative->m_resolved = true;

	resolvent->m_cL = 0;
	sLiteral **resolvent_literal = resolvent->m_literals;

	literal_1 = clause_1->m_literals;
	while (*literal_1 != NULL)
	{
		if (!(*literal_1)->m_resolved)
		{
			(*resolvent_literal++) = *literal_1;
			(*literal_1)->m_resolved = true;
			++resolvent->m_cL;
		}
		++literal_1;
	}

	literal_2 = clause_2->m_literals;
	while (*literal_2 != NULL)
	{
		if (!(*literal_2)->m_resolved)
		{
			(*resolvent_literal++) = *literal_2;
			(*literal_2)->m_resolved = true;
			++resolvent->m_cL;
		}
		++literal_2;
	}
	*resolvent_literal = NULL;
}


sValue sBooleanFormula::calc_Value(const sClause &clause)
{
	sLiteral **literal = clause.m_literals;
	
	int undefs = 0;
	
	while (*literal != NULL)
	{
		switch ((*literal)->m_value)
		{
		case V_FALSE:
		{
			break;
		}
		case V_TRUE:
		{
			return V_TRUE;
			break;
		}
		case V_UNDEF:
		{
			++undefs;
			break;
		}
		default:
		{
			break;
		}
		}
		++literal;
	}
	
	switch (undefs)
	{
	case 0:
	{
		return V_FALSE;
		break;
	}
	case 1:
	{
		return V_UNIT;
		break;
	}
	default:
	{
		return V_UNDEF;
		break;
	}
	}
}


bool sBooleanFormula::is_Satisfied(const sClause &clause)
{
	sValue value = calc_Value(clause);

	switch(value)
	{
	case V_TRUE:
	{
		return true;
	}
	default:
	{
		return false;
		break;
	}
	}
}


bool sBooleanFormula::is_Falsified(const sClause &clause)
{
	sValue value = calc_Value(clause);

	switch(value)
	{
	case V_FALSE:
	{
		return true;
	}
	default:
	{
		return false;
		break;
	}
	}
}


bool sBooleanFormula::is_Satisfied(const sLiteral &literal)
{
	return (literal.m_value == V_TRUE);
}


bool sBooleanFormula::is_Falsified(const sLiteral &literal)
{
	return (literal.m_value == V_FALSE);
}


bool sBooleanFormula::is_Assigned(const sLiteral &literal)
{
	return (literal.m_value != V_UNDEF);
}


bool sBooleanFormula::is_Complementary(const sLiteral *literal_1, const sLiteral *literal_2) const
{
/*
	bool complementary = (literal_1 != literal_2 && (literal_1 - literal_1->m_base) == (literal_2 - literal_2->m_base));

	if (complementary)
	{
		printf("Complementary:");
		print_literal(literal_1);
		printf(" ");
		print_literal(literal_2);
		printf("\n");
	}
	else
	{
		printf("Non-complementary:");
		print_literal(literal_1);
		printf(" ");
		print_literal(literal_2);
		printf("\n");
	}
	return complementary;
*/
	return (literal_1 != literal_2 && (literal_1 - literal_1->m_base) == (literal_2 - literal_2->m_base));
}


void sBooleanFormula::print_literal(const sLiteral *literal) const
{
	if (literal->m_base == m_P_literals)
	{
		printf("%ld", literal - literal->m_base);
	}
	else
	{
		printf("-%ld", literal - literal->m_base);
	}
}


void sBooleanFormula::sprint_literal(char *s, const sLiteral *literal) const
{
	if (literal->m_base == m_P_literals)
	{
		sprintf(s, "%ld", literal - literal->m_base);
	}
	else
	{
		sprintf(s, "-%ld", literal - literal->m_base);
	}
}


void sBooleanFormula::print_literals(sLiteral **literals) const
{
	sLiteral **literal = literals;
	
	while (*literal != NULL)
	{
		print_literal(*literal);
		printf(" ");
		++literal;
	}
	
	printf("\n");
}


void sBooleanFormula::print_units(void) const
{
	printf("Unit literals:");
	
	sLiteral **literal = m_unit_literals;
	
	while (literal < m_last_unit)
	{
		print_literal(*literal);
		++literal;
	}
	
	printf("\n");
}


void sBooleanFormula::print_clause(const sClause *clause, const char *w1_mark, const char *w2_mark) const
{
	sLiteral **literal = clause->m_literals;
	
	printf("%ld: ", clause - m_clauses);
	
	while (*literal != NULL)
	{
		if ((*literal)->m_base == m_P_literals)
		{
			if (literal == clause->m_watch_1.m_literal)
			{
				printf("%ld%s ", *literal - (*literal)->m_base, w1_mark);
			}
			else
			{
				if (literal == clause->m_watch_2.m_literal)
				{
					printf("%ld%s ", *literal - (*literal)->m_base, w2_mark);
				}
				else
				{
						printf("%ld ", *literal - (*literal)->m_base);
				}
			}
		}
		else
		{
			if (literal == clause->m_watch_1.m_literal)
			{
				printf("-%ld%s ", *literal - (*literal)->m_base, w1_mark);
			}
			else
			{
				if (literal == clause->m_watch_2.m_literal)
				{
					printf("-%ld%s ", *literal - (*literal)->m_base, w2_mark);
					}
				else
				{
					printf("-%ld ", *literal - (*literal)->m_base);
				}
			}
		}
		++literal;
	}
	printf("0");
}


void sBooleanFormula::print_clause_DIMACS(const sClause *clause) const
{
	sLiteral **literal = clause->m_literals;
	
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
	printf("0");
}


void sBooleanFormula::print_assignments(const sAssignment *asgn_end) const
{
	print_assignments(m_Assignments, asgn_end);
}


void sBooleanFormula::print_assignments(const sAssignment *asgn_begin, const sAssignment *asgn_end) const
{
	if (asgn_begin < asgn_end)
	{
		while (true)
		{
			if (is_Satisfied(*asgn_begin->m_variable->m_positive))
			{
				printf("%ld=1 ", asgn_begin->m_variable - m_variables);
			}
			else
			{
				printf("%ld=0 ", asgn_begin->m_variable - m_variables);
			}
			
			printf("(");
			if (asgn_begin->m_ante_clause != NULL)
			{
				print_clause(asgn_begin->m_ante_clause);
			}
			if (++asgn_begin < asgn_end)
			{
				printf("); ");
			}
			else
			{
				printf(")");
				break;
			}
		}
		printf("\n");
	}
}


void sBooleanFormula::print(void) const
{
	printf("p cnf %d %d\n", m_V, m_cC);
	
	for (int i = 0; i < m_cC; ++i)
	{
		const sClause *clause = &m_clauses[i];
//		sLiteral **literal = clause->m_literals;
		
		print_clause(clause, "^1", "^2");
		printf("\n");
	}
	
	printf("literals\n");
	
	for (int i = 1; i <= m_V; ++i)
	{
		WNode *wnode_1 = m_P_literals[i].m_watch_1_list.m_next;
		WNode *wnode_2 = m_P_literals[i].m_watch_2_list.m_next;
		
		if (wnode_1->m_watch != NULL || wnode_2->m_watch != NULL)
		{
			printf("%d [", i);
			
			while (wnode_1->m_watch != NULL)
			{
				printf("%ld", wnode_1->m_watch->m_clause - m_clauses);
				wnode_1 = wnode_1->m_next;
				
				if (wnode_1 != NULL)
				{
					printf(",");
				}
			}
			printf("]");
			
			printf("{");
			
			while (wnode_2->m_watch != NULL)
			{
				printf("%ld", wnode_2->m_watch->m_clause - m_clauses);
				wnode_2 = wnode_2->m_next;
				
				if (wnode_2 != NULL)
				{
					printf(",");
				}
			}
			printf("} ");
		}
		else
		{
			printf(" %d ", i);
		}
	}
	printf("\n");
	
	for (int i = 1; i <= m_V; ++i)
	{
		WNode *wnode_1 = m_N_literals[i].m_watch_1_list.m_next;
		WNode *wnode_2 = m_N_literals[i].m_watch_2_list.m_next;
		
		if (wnode_1->m_watch != NULL || wnode_2->m_watch != NULL)
		{
			printf("-%d [", i);
			
			while (wnode_1->m_watch != NULL)
			{
				printf("%ld", wnode_1->m_watch->m_clause - m_clauses);
				wnode_1 = wnode_1->m_next;
				
				if (wnode_1->m_watch != NULL)
				{
					printf(",");
				}
			}
			printf("]");
			
			printf("{");
			
			while (wnode_2->m_watch != NULL)
			{
				printf("%ld", wnode_2->m_watch->m_clause - m_clauses);
				wnode_2 = wnode_2->m_next;
				
				if (wnode_2 != NULL)
				{
					printf(",");
				}
			}
			printf("} ");
		}
		else
		{
			printf("-%d ", i);
		}
	}
	printf("\n");
	
	printf("variables\n");
	for (int i = 1; i <= m_V; ++i)
	{
		printf("%d=%d ", i, m_P_literals[i].m_value);
	}

	printf("\n");
}


void sBooleanFormula::print_DIMACS(void) const
{
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


void sBooleanFormula::load_DIMACS(char *filename)
{
	FILE *fr;
	
	m_conflicting = false;
	
	fr = fopen(filename, "r");
	assert(fr != NULL);
	
	skip_comments_DIMACS(fr);

	int V, C, O, M;
	fscanf(fr, "p cnf %d %d\n", &V, &C);
	O = 0;
	M = 0;
	
	for (int i = 0; i < C; ++i)
	{
		skip_comments_DIMACS(fr);
		
		int l, m = 0;
		
		while (true)
		{
			fscanf(fr, "%d", &l);
			++O;
			++m;
			
			if (l == 0)
			{
				fscanf(fr, "\n");
				break;
			}
		}
		
		if (m > M)
		{
			M = m;
		}
		
		if (feof(fr))
		{
			C = i + 1;
				break;
		}
		}
	
	fclose(fr);
	
	init(V, C, O);
	
	int *cl;
	cl = (int*)malloc(M * sizeof(int));
	assert(cl != NULL);
	
	fr = fopen(filename, "r");
	assert(fr != NULL);
	
	skip_comments_DIMACS(fr);
	
	fscanf(fr, "p cnf %d %d\n", &V, &C);
	
	for (int i = 0; i < C; ++i)
	{
		skip_comments_DIMACS(fr);
		
		int l, m = 0;
		
		while (true)
		{
			fscanf(fr, "%d", &l);
			cl[m] = l;
			
			if (l == 0)
			{
				fscanf(fr, "\n");
					break;
			}
			
			++m;
			++O;
		}
		
		add_general_clause(cl);
		
		if (feof(fr))
		{
			break;
		}
	}
	free(cl);
	fclose(fr);
}


void sBooleanFormula::save_DIMACS(char *filename)
{
	FILE *fw;
	
	fw = fopen(filename, "w");
	assert(fw != NULL);
	
	fprintf(fw, "p cnf %d %d\n", m_V, m_cC);
	
	for (int i = 0; i < m_cC; ++i)
	{
		const sClause &clause = m_clauses[i];
		sLiteral **literal = clause.m_literals;
		
		while (*literal != NULL)
		{
			if ((*literal)->m_base == m_P_literals)
			{
				fprintf(fw, "%ld ", *literal - (*literal)->m_base);
			}
			else
			{
				fprintf(fw, "-%ld ", *literal - (*literal)->m_base);
			}
			++literal;
		}
		fprintf(fw, "0\n");
	}
}


void sBooleanFormula::skip_comments_DIMACS(FILE *fr)
{
	char ch;
	
	while (true)
	{
		fscanf(fr, "%c", &ch);
		
		if (ch == 'c')
		{
			while (fgetc(fr) != '\n');
		}
		else
		{
			ungetc(ch, fr);
			return;
		}
	}
}



void insert_watch(WNode *head, WNode *insert)
{
	WNode *next = head->m_next;

	next->m_prev = insert;
	head->m_next = insert;

	insert->m_prev = head;
	insert->m_next = next;
}


void remove_watch(WNode *sUNUSED(head), WNode *remove)
{
	WNode *next = remove->m_next;
	WNode *prev = remove->m_prev;

	prev->m_next = next;
	next->m_prev = prev;
}


void skip_comment_DIMACS(FILE *fr)
{
	while (fgetc(fr) != '\n');
}

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
// system.h / 0.71-sky
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
// Systematic module of sigmaSAT solver.


#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <assert.h>
#include <stdio.h>

#include <map>

#include "graph.h"

class sLiteral;
class sWatch;
class sClause;
class sAssignment;

extern int SPC_CLAUSES;
extern int APC_CLAUSES;
extern int EPC_CLAUSES;

struct sVariable
{
	sLiteral *m_positive;
	sLiteral *m_negative;
	sAssignment *m_assignment;
};


struct WNode
{
	sWatch *m_watch;
	WNode *m_next;
	WNode *m_prev;
};


struct ONode
{
	sClause *m_clause;
	ONode *m_next;	
};


enum sValue
{
	V_FALSE      =  0,
	V_TRUE       =  1,
	V_UNIT       =  2,
	V_UNDEF      =  4
};


struct sLiteral
{
	sValue m_value;
	sLiteral *m_base;
	sLiteral *m_cmpl;
	sVariable *m_variable;
	WNode m_watch_1_list;
	WNode m_watch_2_list;
	bool m_resolved;

// Path specific members
	int m_cO;
	ONode *m_occs;

	sVertex *m_vocc;
	int m_cVO;
	VONode *m_voccs;
	bool m_single;
	int m_vertex_idx;
};


struct sWatch
{
	sClause *m_clause;
	sLiteral **m_literal;
};


/*----------------------------------------------------------------------------*/
// sClause class

class sClause
{
public:
	sClause();
	sClause(const sClause &clause);
	const sClause& operator=(const sClause &clause);
	~sClause();

	bool is_Null(void) const;

	bool calc_Termination(void) const;
	int calc_PreviousDepth(int current_depth) const;

	void to_Screen(const sBooleanFormula *formula) const;

public:
	int m_cL;
	sLiteral **m_literals;

	sWatch m_watch_1;
	sWatch m_watch_2;

// Path specific members
	bool m_checked;
	bool m_in_path;
	sClause **m_sort;
	int m_affects;

// Flow specific members
	bool m_selected;
	double m_thickness;
};


void insert_watch(WNode *watch_list, WNode *watch_insert);
void remove_watch(WNode *watch_list, WNode *watch_remove);
void skip_comment_DIMACS(FILE *fr);


struct sAssignment
{
	int m_depth;
	sVariable *m_variable;
	sClause *m_ante_clause;
};


struct sECNode
{
//	sExternalClause *m_ext_clause;
	sECNode *m_next;
};


enum sBacktrackPhase
{
	BT_PHASE_INITIAL  = 0,
	BT_PHASE_POSITIVE = 1,
	BT_PHASE_NEGATIVE = 2,
	BT_PHASE_TERMINAL = 3,
	BT_PHASE_FAILURE  = 4
};


enum sBackjumpPhase
{
	BJ_PHASE_INITIAL     = 0,
	BJ_PHASE_PROPAGATION = 1
};


struct sDecisionRecord
{
	union Phase
	{
		sBacktrackPhase m_bt;
		sBackjumpPhase m_bj;
	};

	Phase m_phase;

	sClause *m_head_clause;
	sVariable *m_head_var;
	sLiteral **m_check_point;
	sVariable *m_selected_var;
};


struct sClauseNode
{
	int m_N_Clauses;
	int m_Clause_cnt;
	sClause *m_Clauses;

	sClauseNode *m_next;
};


struct sOccurenceNode
{
	int m_N_Occurences;
	int m_Occurence_cnt;
	sLiteral **m_Literals;

	sOccurenceNode *m_next;
};


/*----------------------------------------------------------------------------*/
// sClauseStore class

class sClauseStore
{
public:
	sClauseStore();
	~sClauseStore();

	sClause* alloc_Clause(void);

public:
	sClauseNode *m_first_node;
};


/*----------------------------------------------------------------------------*/
// sOccurenceStore class

class sOccurenceStore
{
public:
	sOccurenceStore();
	~sOccurenceStore();

	sLiteral** alloc_Occurences(int N_occurences);

public:
	sOccurenceNode *m_first_node;
};


/*----------------------------------------------------------------------------*/
// sBooleanFormula class

class sBooleanFormula
{
public:
	void init(int n_V, int n_C, int n_O);

	void add_2_clause(int l1, int l2);
	void add_3_clause(int l1, int l2, int l3);
	void add_general_clause(int *lits);

	void set_TRUE(sLiteral *literal);
	void set_FALSE(sLiteral *literal);
	void unset(sLiteral *literal);

	sLiteral** get_CheckPoint(void) const;
	sAssignment get_LastAssignment(void) const;

	void operate_Set_TRUE(sLiteral *literal);
	void operate_Set_FALSE(sLiteral *literal);

	void operate_Set_TRUE(sLiteral *literal, sClause *antecedent_clause, int depth);
	void operate_Set_FALSE(sLiteral *literal, sClause *antecedent_clause, int depth);

	void operate_Unset(sLiteral *literal);

	bool operate_SetPropagate_TRUE(sLiteral *literal);
	bool operate_SetPropagate_FALSE(sLiteral *literal);

	bool operate_SetPropagateAnalyze_TRUE(sLiteral *literal, sClause **conflict_clause, int depth);
	bool operate_SetPropagateAnalyze_FALSE(sLiteral *literal, sClause **conflict_clause, int depth);

	bool operate_SetPropagateAnalyze_TRUE(sLiteral *literal, sClause *antecedent_clause, sClause **conflict_clause, int depth);
	bool operate_SetPropagateAnalyze_FALSE(sLiteral *literal, sClause *antecedent_clause, sClause **conflict_clause, int depth);
	
	void operate_UnsetUnpropagate(void);
	void operate_UnsetUnpropagate(sLiteral **check_point);

	bool operate_PropagateUnit(void);
	bool operate_PropagateUnitAnalyze(sVariable *selected_variable, int depth, sClause **conflict_clause);

	bool solve_1(void);
	bool solve_1_rec(int depth);

	bool solve_2(void);
	bool solve_2_flat(void);

	bool solve_3(void);
	bool solve_3_flat(void);

	sVariable* select_Variable_1(sVariable *hint) const;
	sVariable* select_Variable_2(sVariable *hint) const;

//	void push_ExternalClause(sExternalClause *ext_clause, int depth);
//	void erase_ExternalClauses(int depth);

	sClause* generate_ConflictClause(sClause *conflict, const sAssignment *decision_assignment);
	sClause* generate_ConflictClause(sVariable *selected_variable, sClause *conflict, int depth);
	void resolve_Clauses(const sClause *clause_1, const sClause *clause_2, sVariable *variable, sClause *resolvent);
	static sValue calc_Value(const sClause &clause);

	static bool is_Satisfied(const sClause &clause);
	static bool is_Falsified(const sClause &clause);

	static bool is_Assigned(const sLiteral &literal);
	static bool is_Satisfied(const sLiteral &literal);
	static bool is_Falsified(const sLiteral &literal);

	bool is_Complementary(const sLiteral *literal_1, const sLiteral *literal_2) const;

	void print_literal(const sLiteral *literal) const;
	void sprint_literal(char *s, const sLiteral *literal) const;

	void print_literals(sLiteral **literals) const;
	void print_units(void) const;
	void print_clause(const sClause *clause, const char *w1_mark = "^1", const char *w2_mark = "^2") const;
	void print_clause_DIMACS(const sClause *clause) const;

	void print_assignments(const sAssignment *asgn_end) const;
	void print_assignments(const sAssignment *asgn_begin, const sAssignment *asgn_end) const;

	void print(void) const;
	void print_DIMACS(void) const;
	void load_DIMACS(char *filename);
	void save_DIMACS(char *filename);
	void skip_comments_DIMACS(FILE *fr);

public:
	int m_V;
	sVariable *m_variables;
	sLiteral *m_P_literals;
	sLiteral *m_N_literals;

	int m_O, m_cO;
	sLiteral **m_occurences;

	int m_cLO;
	ONode *m_L_occurences;

	int m_C, m_cC;
	sClause *m_clauses;

	int m_cW;
	WNode *m_watch_nodes;

	bool m_conflicting;

// unit propagation specific
	sLiteral **m_unit_literals;
	sLiteral **m_last_unit;

	sLiteral **m_chng_literals;
	sLiteral **m_last_chng;

// solver specific
	sClause *m_head_clause;
	sVariable *m_head_var;

	sAssignment *m_assignment_end;
	sAssignment *m_Assignments;

	sClause m_resolvent_A;
	sClause m_resolvent_B;	

	sDecisionRecord *m_decision_Records;

// backjumping specific
	sECNode **m_ec_Nodes;

// path specific members
	int m_cA;
	sClause **m_affcls;
};


#endif /* __SYSTEM_H__ */

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
// local.cpp / 0.71-sky
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
// Local (WalkSAT based)  module of sigmaSAT solver.


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#include "version.h"
#include "defs.h"
#include "config.h"

struct clause;

typedef struct onode
{
	clause *cl;
	int il;
	struct onode *next;
	struct onode *prev;
} ONODE;

typedef struct
{
	int l;
	ONODE occ;
	ONODE us_occ;
} LITERAL;

typedef ONODE WNODE;

typedef struct clause
{
	int n_L;
	LITERAL *lits;
	WNODE wn;
	int iuns;
} CLAUSE;

typedef struct
{
	int n_C;
	CLAUSE *cls;

	int n_U;
	int *uns_cls;
} FORMULA;

enum Value
{
	V_FALSE,
	V_TRUE,
	V_UNDEF
};

typedef struct
{
	int val;

	ONODE p_occs;
	ONODE n_occs;

	ONODE p_uns_occs;
	ONODE n_uns_occs;

	ONODE p_sat_occs;
	ONODE n_sat_occs;

	WNODE p_watches;
	WNODE n_watches;
} VARIABLE;

typedef struct
{
	int n_V;
	VARIABLE *vars;
} VALUATION;


int lit_store_size = 0;
LITERAL *lit_store;
LITERAL *lit_store_last;

double current_seconds(void)
{
	struct timeval tv;
	struct timezone tz;

	gettimeofday(&tv, &tz);
	
	return (double)tv.tv_sec + (double)tv.tv_usec / 1000000;
}

void add_occurence(ONODE *head, ONODE *insert, CLAUSE *cl, int il)
{
	ONODE *next = head->next;

	next->prev = insert;
	head->next = insert;

	insert->prev = head;
	insert->next = next;

	insert->cl = cl;
	insert->il = il;
}


void add_occurence(ONODE *head, ONODE *insert)
{
	ONODE *next = head->next;

	next->prev = insert;
	head->next = insert;

	insert->prev = head;
	insert->next = next;
}


void remove_occurence(ONODE *remove)
{
	ONODE *next = remove->next;
	ONODE *prev = remove->prev;

	prev->next = next;
	next->prev = prev;
}


void add_watch(WNODE *head, WNODE *insert, int il)
{
	WNODE *next = head->next;

	next->prev = insert;
	head->next = insert;

	insert->prev = head;
	insert->next = next;

	insert->il = il;
}


void remove_watch(WNODE *remove)
{
	WNODE *next = remove->next;
	WNODE *prev = remove->prev;

	prev->next = next;
	next->prev = prev;
}


void insert_unsatisfied(FORMULA *formula, int uns_cl)
{
//	printf("Inserting:%d (%d)\n", uns_cl, formula->n_U);
	formula->uns_cls[formula->n_U] = uns_cl;
	formula->cls[uns_cl].iuns = formula->n_U;

	++formula->n_U;
}


void delete_unsatisfied(FORMULA *formula, int uns_cl)
{
//	printf("Deleting:%d (%d)\n", uns_cl, formula->n_U);

	--formula->n_U;

	int a = formula->cls[uns_cl].iuns;
	int b = formula->uns_cls[formula->n_U];

	formula->uns_cls[a] = b;
	formula->cls[b].iuns = a;
}


int read_CNF(char *filename, FORMULA *formula, VALUATION *valuation)
{
	FILE *fr;

	fr = fopen(filename, "r");

	if (fr == NULL)
	{
		printf("Cannot open input file %s.\n", filename);
		return -1;
	}

	int V, C;

	char c;

	int L = 0;
	int CC = 0;

	/* skip comments */
	while (1)
	{
		fscanf(fr, "%c", &c);
		if (c == 'p')
		{
			break;
		}
		else
		{
			while (1)
			{
				fscanf(fr, "%c", &c);
				if (c == '\n')
				{
					break;
				}
			}
		}
	}

	fscanf(fr, " cnf %d %d\n", &V, &C);

	/* calculate size of the formula */
	while (!feof(fr))
	{
		/* read clause */
		while (1)
		{
			int l;
			fscanf(fr, "%d ", &l);

			if (l == 0)
			{
				break;
			}
			else
			{
				++L;
			}
		}
		++CC;
	}

	++V;
	lit_store_size = L + V;
	lit_store_last = lit_store = (LITERAL*)malloc(lit_store_size * sizeof(LITERAL));

	if (lit_store == NULL)
	{
		printf("Cannot alocate memory.\n");
		return -3;
	}

	valuation->vars = (VARIABLE*)malloc(V * sizeof(VARIABLE));
	valuation->n_V = V;

	if (valuation->vars == NULL)
	{
		printf("Cannot alocate memory.\n");
		return -5;
	}

	VARIABLE *var = valuation->vars;
	for (int i = 0; i < V; ++i)
	{
		var->val = V_FALSE;

		var->p_occs.next = var->p_occs.prev = &var->p_occs;
		var->n_occs.next = var->n_occs.prev = &var->n_occs;

		var->p_uns_occs.next = var->p_uns_occs.prev = &var->p_uns_occs;
		var->n_uns_occs.next = var->n_uns_occs.prev = &var->n_uns_occs;

		var->p_sat_occs.next = var->p_sat_occs.prev = &var->p_sat_occs;
		var->n_sat_occs.next = var->n_sat_occs.prev = &var->n_sat_occs;

		var->p_watches.next = var->p_watches.prev = &var->p_watches;
		var->n_watches.next = var->n_watches.prev = &var->n_watches;

		++var;
	}

	fseek(fr, 0, SEEK_SET);

	/* skip comments */
	while (1)
	{
		fscanf(fr, "%c", &c);
		if (c == 'p')
		{
			break;
		}
		else
		{
			while (1)
			{
				fscanf(fr, "%c", &c);
				if (c == '\n')
				{
					break;
				}
			}
		}
	}

	fscanf(fr, " cnf %d %d\n", &V, &C);

//	formula->n_C = C;
	formula->n_C = CC;  /* CC is more accurate; C may be wrong */
	formula->cls = (CLAUSE*)malloc(C * sizeof(CLAUSE));
	formula->uns_cls = (int*)malloc(C * sizeof(int));

	if (formula->cls == NULL)
	{
		printf("Cannot alocate memory.\n");
		return -2;
	}

	/* read formula */
	int i = 0;
	while (!feof(fr))
	{
		formula->cls[i].n_L = 0;
		formula->cls[i].lits = lit_store_last;

		formula->cls[i].wn.cl = &formula->cls[i];
		formula->cls[i].wn.il = -1;
/*
		formula->cls[i].wn.next = NULL;
		formula->cls[i].wn.prev = NULL;
*/
		/* read clause */
		int j = 0;
//		printf("%d:", i);
		while (1)
		{
			int l;
			fscanf(fr, "%d ", &l);
//			printf("%d ", l);

			if (l == 0)
			{
//				printf("\n");
				break;
			}
			else
			{
				formula->cls[i].lits[j].l = l;
				++formula->cls[i].n_L;
				++lit_store_last;

				if (l > 0)
				{
					add_occurence(&valuation->vars[l].p_occs, &formula->cls[i].lits[j].occ, &formula->cls[i], j);
				}
				else
				{
					add_occurence(&valuation->vars[-l].n_occs, &formula->cls[i].lits[j].occ, &formula->cls[i], j);
				}
			}
			++j;
		}
		++i;
	}
	fclose(fr);

	return 0;
}

bool is_satisfied(CLAUSE *cl, VALUATION *val)
{
	for (int i = 0; i < cl->n_L; ++i)
	{
		if (cl->lits[i].l < 0)
		{
			if (val->vars[-cl->lits[i].l].val == V_FALSE)
			{
				return true;
			}
		}
		else
		{
			if (val->vars[cl->lits[i].l].val == V_TRUE)
			{
				return true;
			}
		}
	}

	return false;
}


int count_satisfied(FORMULA *formula, VALUATION *val)
{
	int S = 0;

	for (int i = 0; i < formula->n_C; ++i)
	{
		bool sat = is_satisfied(&formula->cls[i], val);

		if (sat)
		{
			++S;
		}
	}

	return S;
}

int TOTAL_FLIPS = 0;


void flip_variable(int *var)
{
	if (*var == V_FALSE)
	{
		*var = V_TRUE;
	}
	else
	{
		*var = V_FALSE;
	}

	++TOTAL_FLIPS;
}


bool is_literal_satisfied(int lit, VALUATION *val)
{
	if (lit < 0)
	{
		return (val->vars[-lit].val == V_FALSE);
	}
	else
	{
		return (val->vars[lit].val == V_TRUE);
	}
}


int flip_variable_watch(FORMULA *formula, int ivar, VALUATION *val)
{
	++TOTAL_FLIPS;

	int sat = 0;
	VARIABLE *var = &val->vars[ivar];

	if (var->val == V_FALSE)
	{
		var->val = V_TRUE;

		for (ONODE *o = var->p_uns_occs.next; o != &var->p_uns_occs;)
		{
			add_watch(&var->p_watches, &o->cl->wn, o->il);
			++sat;

			ONODE *ro = o;
			o = o->next;

			for (int k = 0; k < ro->cl->n_L; ++k)
			{
				LITERAL *lit = &ro->cl->lits[k];
				remove_occurence(&lit->us_occ);

				if (lit->l < 0)
				{
					add_occurence(&val->vars[-lit->l].n_sat_occs, &lit->us_occ);
				}
				else
				{
					add_occurence(&val->vars[lit->l].p_sat_occs, &lit->us_occ);
				}
			}

			delete_unsatisfied(formula, ro->cl - formula->cls);
		}

		for (WNODE *w = var->n_watches.next; w != &var->n_watches;)
		{
			int i;

			for (i = 0; i < w->cl->n_L; ++i)
			{
				if (is_literal_satisfied(w->cl->lits[i].l, val))
				{
					break;
				}
			}			
			if (i == w->cl->n_L)
			{
				for (int k = 0; k < w->cl->n_L; ++k)
				{
					LITERAL *lit = &w->cl->lits[k];
					remove_occurence(&lit->us_occ);

					if (lit->l < 0)
					{
						add_occurence(&val->vars[-lit->l].n_uns_occs, &lit->us_occ);
					}
					else
					{
						add_occurence(&val->vars[lit->l].p_uns_occs, &lit->us_occ);
					}
				}

				WNODE *rw = w;
				w = w->next;

				remove_watch(rw);
				rw->il = -1;
				--sat;

				insert_unsatisfied(formula, rw->cl - formula->cls);
			}
			else
			{
				WNODE *rw = w;
				w = w->next;

				remove_watch(rw);

				if (rw->cl->lits[i].l < 0)
				{
					add_watch(&val->vars[-rw->cl->lits[i].l].n_watches, rw, i);
				}
				else
				{
					add_watch(&val->vars[rw->cl->lits[i].l].p_watches, rw, i);
				}
			}
		}
	}
	else
	{
		var->val = V_FALSE;

		for (ONODE *o = var->n_uns_occs.next; o != &var->n_uns_occs; )
		{
			add_watch(&var->n_watches, &o->cl->wn, o->il);
			++sat;

			ONODE *ro = o;
			o = o->next;

			for (int k = 0; k < ro->cl->n_L; ++k)
			{
				LITERAL *lit = &ro->cl->lits[k];
				remove_occurence(&lit->us_occ);

				if (lit->l < 0)
				{
					add_occurence(&val->vars[-lit->l].n_sat_occs, &lit->us_occ);
				}
				else
				{
					add_occurence(&val->vars[lit->l].p_sat_occs, &lit->us_occ);
				}
			}

			delete_unsatisfied(formula, ro->cl - formula->cls);
		}

		for (WNODE *w = var->p_watches.next; w != &var->p_watches;)
		{
			int i;
			for (i = 0; i < w->cl->n_L; ++i)
			{
				if (is_literal_satisfied(w->cl->lits[i].l, val))
				{
					break;
				}
			}
			if (i == w->cl->n_L)
			{
				for (int k = 0; k < w->cl->n_L; ++k)
				{
					LITERAL *lit = &w->cl->lits[k];
					remove_occurence(&lit->us_occ);

					if (lit->l < 0)
					{
						add_occurence(&val->vars[-lit->l].n_uns_occs, &lit->us_occ);
					}
					else
					{
						add_occurence(&val->vars[lit->l].p_uns_occs, &lit->us_occ);
					}
				}

				WNODE *rw = w;
				w = w->next;

				remove_watch(rw);
				rw->il = -1;
				--sat;

				insert_unsatisfied(formula, rw->cl - formula->cls);
			}
			else
			{
				WNODE *rw = w;
				w = w->next;

				remove_watch(rw);

				if (rw->cl->lits[i].l < 0)
				{
					add_watch(&val->vars[-rw->cl->lits[i].l].n_watches, rw, i);
				}
				else
				{
					add_watch(&val->vars[rw->cl->lits[i].l].p_watches, rw, i);
				}
			}
		}
	}

	return sat;
}


int try_variable_watch(int ivar, VALUATION *val)
{
	TOTAL_FLIPS += 2;

	int sat = 0;
	VARIABLE *var = &val->vars[ivar];

	if (var->val == V_FALSE)
	{
		var->val = V_TRUE;

		for (ONODE *o = var->p_uns_occs.next; o != &var->p_uns_occs; o = o->next)
		{
			++sat;
		}

		for (WNODE *w = var->n_watches.next; w != &var->n_watches; w = w->next)
		{
			int i;

			for (i = 0; i < w->cl->n_L; ++i)
			{
				if (is_literal_satisfied(w->cl->lits[i].l, val))
				{
					break;
				}
			}			
			if (i == w->cl->n_L)
			{
				--sat;
			}
		}

		var->val = V_FALSE;
	}
	else
	{
		var->val = V_FALSE;

		for (ONODE *o = var->n_uns_occs.next; o != &var->n_uns_occs; o = o->next)
		{
			++sat;
		}

		for (WNODE *w = var->p_watches.next; w != &var->p_watches; w = w->next)
		{
			int i;

			for (i = 0; i < w->cl->n_L; ++i)
			{
				if (is_literal_satisfied(w->cl->lits[i].l, val))
				{
					break;
				}
			}
			if (i == w->cl->n_L)
			{
				--sat;
			}
		}

		var->val = V_TRUE;
	}

	return sat;
}


int initialize_watches(FORMULA *formula, VALUATION *val)
{
	int sat = 0;

	VARIABLE *var = val->vars;
	formula->n_U = 0;

	for (int i = 0; i < val->n_V; ++i)
	{
		var->p_uns_occs.next = var->p_uns_occs.prev = &var->p_uns_occs;
		var->n_uns_occs.next = var->n_uns_occs.prev = &var->n_uns_occs;

		var->p_sat_occs.next = var->p_sat_occs.prev = &var->p_sat_occs;
		var->n_sat_occs.next = var->n_sat_occs.prev = &var->n_sat_occs;

		var->p_watches.next = var->p_watches.prev = &var->p_watches;
		var->n_watches.next = var->n_watches.prev = &var->n_watches;

		++var;
	}

	for (int i = 0; i < formula->n_C; ++i)
	{
		int j;

		for (j = 0; j < formula->cls[i].n_L; ++j)
		{
			if (is_literal_satisfied(formula->cls[i].lits[j].l, val))
			{
				break;
			}
		}
		
		if (j < formula->cls[i].n_L)
		{
			if (formula->cls[i].lits[j].l < 0)
			{
				add_watch(&val->vars[-formula->cls[i].lits[j].l].n_watches, &formula->cls[i].wn, j);
			}
			else
			{
				add_watch(&val->vars[formula->cls[i].lits[j].l].p_watches, &formula->cls[i].wn, j);
			}
			++sat;

			for (int k = 0; k < formula->cls[i].n_L; ++k)
			{
				int l = formula->cls[i].lits[k].l;

				if (l < 0)
				{
					add_occurence(&val->vars[-l].n_sat_occs, &formula->cls[i].lits[k].us_occ, &formula->cls[i], k);
				}
				else
				{
					add_occurence(&val->vars[l].p_sat_occs, &formula->cls[i].lits[k].us_occ, &formula->cls[i], k);
				}
			}
		}
		else
		{
			formula->cls[i].wn.il = -1;

			for (int k = 0; k < formula->cls[i].n_L; ++k)
			{
				int l = formula->cls[i].lits[k].l;

				if (l < 0)
				{
					add_occurence(&val->vars[-l].n_uns_occs, &formula->cls[i].lits[k].us_occ, &formula->cls[i], k);
				}
				else
				{
					add_occurence(&val->vars[l].p_uns_occs, &formula->cls[i].lits[k].us_occ, &formula->cls[i], k);
				}
			}

			insert_unsatisfied(formula, i);
		}
	}

	return sat;
}


void flip_literal(int lit, VALUATION *val)
{
	if (lit < 0)
	{
		flip_variable(&val->vars[-lit].val);
	}
	else
	{
		flip_variable(&val->vars[lit].val);
	}
}


int flip_literal_watch(FORMULA *formula, int lit, VALUATION *val)
{
	if (lit < 0)
	{
		return flip_variable_watch(formula, -lit, val);
	}
	else
	{
		return flip_variable_watch(formula, lit, val);
	}
}


int try_literal_watch(int lit, VALUATION *val)
{
	if (lit < 0)
	{
		return try_variable_watch(-lit, val);
	}
	else
	{
		return try_variable_watch(lit, val);
	}
}


int flip_literal_occ(int lit, VALUATION *val)
{
	if (lit < 0)
	{
		int sat_1 = 0;

		for (ONODE *o = val->vars[-lit].n_occs.next; o != &val->vars[-lit].n_occs; o = o->next)
		{
			if (is_satisfied(o->cl, val))
			{
				++sat_1;
			}
		}
		for (ONODE *o = val->vars[-lit].p_occs.next; o != &val->vars[-lit].p_occs; o = o->next)
		{
			if (is_satisfied(o->cl, val))
			{
				++sat_1;
			}
		}

		flip_variable(&val->vars[-lit].val);

		int sat_2 = 0;

		for (ONODE *o = val->vars[-lit].n_occs.next; o != &val->vars[-lit].n_occs; o = o->next)
		{
			if (is_satisfied(o->cl, val))
			{
				++sat_2;
			}
		}
		for (ONODE *o = val->vars[-lit].p_occs.next; o != &val->vars[-lit].p_occs; o = o->next)
		{
			if (is_satisfied(o->cl, val))
			{
				++sat_2;
			}
		}

		return sat_2 - sat_1;
	}
	else
	{
		int sat_1 = 0;

		for (ONODE *o = val->vars[lit].n_occs.next; o != &val->vars[lit].n_occs; o = o->next)
		{
			if (is_satisfied(o->cl, val))
			{
				++sat_1;
			}
		}
		for (ONODE *o = val->vars[lit].p_occs.next; o != &val->vars[lit].p_occs; o = o->next)
		{
			if (is_satisfied(o->cl, val))
			{
				++sat_1;
			}
		}

		flip_variable(&val->vars[lit].val);

		int sat_2 = 0;

		for (ONODE *o = val->vars[lit].n_occs.next; o != &val->vars[lit].n_occs; o = o->next)
		{
			if (is_satisfied(o->cl, val))
			{
				++sat_2;
			}
		}
		for (ONODE *o = val->vars[lit].p_occs.next; o != &val->vars[lit].p_occs; o = o->next)
		{
			if (is_satisfied(o->cl, val))
			{
				++sat_2;
			}
		}

		return sat_2 - sat_1;
	}
}


void random_valuation(VALUATION *val)
{
	for (int i = 0; i < val->n_V; ++i)
	{
		int r = random() % 2;

		if (r == 0)
		{
			val->vars[i].val = V_FALSE;
		}
		else
		{
			val->vars[i].val = V_TRUE;
		}
	}
}


const int GREEDY = 57;
int SATISFIED = 0;

bool walk_sat(FORMULA *formula, VALUATION *val, int max_flips, int max_restarts)
{
        printf("Walk SAT 1.0\n");

	double time_begin = current_seconds();

	for (int r = 0; r < max_restarts; ++r)
	{
	        random_valuation(val);

		for (int f = 0; f < max_flips; ++f)
		{
			int S = count_satisfied(formula, val);
			int U = formula->n_C - S;

			if (S > SATISFIED)
			{
				SATISFIED = S;

				double time_end = current_seconds();
				double difference = time_end - time_begin;

				printf("Satisfied:%d (%.6f)\t ", SATISFIED, (double)SATISFIED / formula->n_C);
				printf("time:%.6fs\t flips:%d\t ", difference, TOTAL_FLIPS);
				printf("flips/second:%.3f\n", (double)TOTAL_FLIPS/difference);
			}

			if (U == 0)
			{
				return true;
			}

			int ru = random() % formula->n_C;		
			int i = ru;
//			int ru = 0;
		
			/* randomly select unsatisfied clause */
			while (1)
			{
				if (!is_satisfied(&formula->cls[i], val))
				{
					break;
				}

				++i;
				i %= formula->n_C;
			}	

			int gr = random() % 100;

			if (gr < GREEDY) /* greedy step: flip variable in the clause to maximize the number of satisfied clauses */
			{
				int best = formula->cls[i].lits[0].l;
				flip_literal(best, val);
				int best_sat = count_satisfied(formula, val);
				flip_literal(best, val);
				
				for (int j = 1; j < formula->cls[i].n_L; ++j)
				{
					int curr = formula->cls[i].lits[j].l;
					flip_literal(curr, val);
					int curr_sat = count_satisfied(formula, val);
					flip_literal(curr, val);
					
					if (curr_sat > best_sat)
					{
						best = curr;
						best_sat = curr_sat;
					}
				}
				if (best_sat > S)
				{
					flip_literal(best, val);
				}
			}
			else /* random walk: flip randomly selected variable in the clause */
			{
				int rl = random() % formula->cls[i].n_L;
				flip_literal(formula->cls[i].lits[rl].l, val);
			}
		}
	}

	return false;
}


bool walk_sat_2(FORMULA *formula, VALUATION *val, int max_flips, int max_restarts)
{
        printf("Walk SAT 2.0\n");

	double time_begin = current_seconds();

	for (int r = 0; r < max_restarts; ++r)
	{
	        random_valuation(val);
		int S = count_satisfied(formula, val);

		for (int f = 0; f < max_flips; ++f)
		{
			int U = formula->n_C - S;

			if (S > SATISFIED)
			{
				SATISFIED = S;

				double time_end = current_seconds();
				double difference = time_end - time_begin;

				printf("Satisfied:%d (%.6f)\t ", SATISFIED, (double)SATISFIED / formula->n_C);
				printf("time:%.6fs\t flips:%d\t ", difference, TOTAL_FLIPS);
				printf("flips/second:%.3f\n", (double)TOTAL_FLIPS/difference);
			}

			if (U <= 0)
			{
				return true;
			}
			
			int ru = random() % formula->n_C;
			int i = ru;
//			int ru = 0;
		
			/* randomly select unsatisfied clause */
			while (1)
			{
				if (!is_satisfied(&formula->cls[i], val))
				{
					break;
				}

				++i;
				i %= formula->n_C;
			}
			
			int gr = random() % 100;

			if (gr < GREEDY) /* greedy step: flip variable in the clause to maximize the number of satisfied clauses */
			{
				int best = formula->cls[i].lits[0].l;
				int best_diff = flip_literal_occ(best, val);
				flip_literal(best, val);
				
				for (int j = 1; j < formula->cls[i].n_L; ++j)
				{
					int curr = formula->cls[i].lits[j].l;
					int curr_diff = flip_literal_occ(curr, val);
					flip_literal(curr, val);
					
					if (curr_diff > best_diff)
					{
						best = curr;
						best_diff = curr_diff;
					}
				}
				
				if (best_diff > 0)
				{
					flip_literal(best, val);
					S += best_diff;
				}
			}
			else /* random walk: flip randomly selected variable in the clause */
			{
				int rl = random() % formula->cls[i].n_L;
				int diff = flip_literal_occ(formula->cls[i].lits[rl].l, val);
				S += diff;
			}
		}
	}

	return false;
}


bool walk_sat_3(FORMULA *formula, VALUATION *val, int max_flips, int max_restarts)
{
        printf("Walk SAT 3.0\n");

	double time_begin = current_seconds();

	for (int r = 0; r < max_restarts; ++r)
	{
	        random_valuation(val);
		int S = initialize_watches(formula, val);

		for (int f = 0; f < max_flips; ++f)
		{
			if (S > SATISFIED)
			{
				SATISFIED = S;

				double time_end = current_seconds();
				double difference = time_end - time_begin;

				printf("SAT/UNS:%d/%d (%.6f)\t ", SATISFIED, formula->n_U, (double)SATISFIED / formula->n_C);
				printf("time:%.6fs\t flips:%d\t ", difference, TOTAL_FLIPS);
				printf("flips/second:%.3f\n", (double)TOTAL_FLIPS/difference);
			}
			
			if (formula->n_U <= 0)
			{
				return true;
			}

			int ru = random() % formula->n_U;
			int i = formula->uns_cls[ru];
		
			int gr = random() % 100;

			if (gr < GREEDY) /* greedy step: flip variable in the clause to maximize the number of satisfied clauses */
			{
				int best = formula->cls[i].lits[0].l;
				int best_diff = try_literal_watch(best, val);
				
				for (int j = 1; j < formula->cls[i].n_L; ++j)
				{
					int curr = formula->cls[i].lits[j].l;
					int curr_diff = try_literal_watch(curr, val);

					if (curr_diff > best_diff)
					{
						best = curr;
						best_diff = curr_diff;
					}
				}
				
				if (best_diff > 0)
				{
					int diff = flip_literal_watch(formula, best, val);
					S += diff;
				}
			}
			else /* random walk: flip randomly selected variable in the clause */
			{
				int rl = random() % formula->cls[i].n_L;
				int diff = flip_literal_watch(formula, formula->cls[i].lits[rl].l, val);
				S += diff;
			}
		}
	}

	return false;
}


bool g_sat(FORMULA *formula, VALUATION *val, int max_flips, int max_restarts)
{
        printf("GSAT\n");

	for (int r = 0; r < max_restarts; ++r)
	{
	        random_valuation(val);

		for (int f = 0; f < max_flips; ++f)
		{
		        int S = count_satisfied(formula, val);
			int U = formula->n_C - S;

			if (S > SATISFIED)
			  {
			    SATISFIED = S;
			    printf("Satisfied:%d (%.6f)\n", SATISFIED, (double)SATISFIED / formula->n_C);
			  }

			if (U == 0)
			{
				return true;
			}

			int *best = &val->vars[0].val;
			flip_variable(best);
			int best_sat = count_satisfied(formula, val);
			flip_variable(best);
				
			for (int j = 1; j < val->n_V; ++j)
			{
			    int *curr = &val->vars[j].val;
			    flip_variable(curr);
			    int curr_sat = count_satisfied(formula, val);
			    flip_variable(curr);
					
			    if (curr_sat > best_sat)
			    {
				best = curr;
				best_sat = curr_sat;
			    }
			}
				
			if (best_sat > S)
			{
			    flip_variable(best);
			}			
		}
	}

	return false;
}


FORMULA formula_1;
VALUATION valuation_1;

const int MAX_FLIPS = 16 * 65536;
const int MAX_RESTARTS = 16 * 1024;


void print_intro(void)
{
	printf("sigmaSAT: localSIGMA %s\n", sVERSION);
	printf("Copyright (C) 2009 - 2011 Pavel Surynek\n");
	printf("---------------------------------------\n");
}


int main(int sUNUSED(argc), char **argv)
{
	print_intro();
    
	double s_begin = current_seconds();
        clock_t begin = clock();

	int result = read_CNF(argv[1], &formula_1, &valuation_1);

	if (result == 0)
	{
		bool sat = walk_sat_3(&formula_1, &valuation_1, MAX_FLIPS, MAX_RESTARTS);
		//bool sat = g_sat(&formula_1, &valuation_1, MAX_FLIPS, MAX_RESTARTS);

		if (sat)
		{
			printf("Satisfied !\n");
			char filename[256];
			sprintf(filename, "%s.SAT", argv[1]);
			FILE *res = fopen(filename, "w");
			fclose(res);
		}
		else
		{
			printf("Failure.\n");
		}

		printf("Total flips:%d\n", TOTAL_FLIPS);

		clock_t end = clock();
		printf("Machine seconds: %.6fs\n", (end - begin) / (double)CLOCKS_PER_SEC);
		current_seconds();

		double s_end = current_seconds();
		printf("Real-time seconds:%.6fs\n", s_end - s_begin);
	}

	return 0;
}


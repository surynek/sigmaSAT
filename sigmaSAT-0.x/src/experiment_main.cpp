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
// experiment_main.cpp / 0.71-sky
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
// Main module of the experimental part of the sigmaSAT solver.

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

#include "config.h"
#include "version.h"
#include "defs.h"
#include "experiment.h"


enum COMMAND
{
	UNDEFINED   = 0,
	SOLVE_EXP_1 = 1,
	TEST_SPC    = 2,
	TEST_APC    = 3,
	TEST_EPC    = 4,
	TEST_BPC    = 5,
	TEST_LPC    = 6,
	TEST_LPC2   = 7,
	TEST_FC     = 8,
	TEST_FM     = 9
};


void print_intro(void)
{
	fprintf(stderr, "sigmaSAT: experimentSIGMA %s\n", sVERSION);
	fprintf(stderr, "Copyright (C) 2009 - 2011 Pavel Surynek\n");
	fprintf(stderr, "---------------------------------------\n");
}


int test_path_consistency_SPC(sExperimentalBooleanFormula *formula)
{
	int result;
	int length;
	sClause *path[MAX_LENGTH + 10];

	if (formula->m_conflicting)
	{
		printf("Formula is conflicting.\n");
		return 0;
	}
	else
	{
		if (!formula->operate_PropagateUnit())
		{
			printf("Decided by unit propagation.\n");
			return 0;
		}
	}
	
	for (int pass = 0; pass < MAX_PASSES; ++pass)
	{
		for (int lng = MIN_LENGTH; lng <= MAX_LENGTH; ++lng)
		{
			for (int i = 0; i < formula->m_cC; ++i)
			{
				length = formula->find_conflicting_path(&formula->m_clauses[i], path, lng);
//				formula->print_path(path, length);

				if (length > 1)
				{
					sIncrementalGraph graph;
					Clustering clustering;
					Layering layering;
						
					result = formula->build_ConflictPath(path, length, &graph, &clustering, &layering);
					if (result < 0)
					{
						fprintf(stderr, "Unable to build conflict path (result:%d).\n", result);
						return result;
					}
					formula->check_paths_SPC(&clustering, &layering);
					
					layering.destroy();
					clustering.destroy();
					graph.destroy();
				}	
			}
		}
	}
//	formula->print_DIMACS();
	fprintf(stderr, "Clauses:%ld\n", formula->m_binary_Clauses.size());
	printf("Clauses:%ld\n", formula->m_binary_Clauses.size());

	return 0;
}


int test_path_consistency_APC(sExperimentalBooleanFormula *formula)
{
	int result;
	int length;
	sClause *path[MAX_LENGTH + 10];

	if (formula->m_conflicting)
	{
		printf("Formula is conflicting.\n");
		return 0;
	}
	else
	{
		if (!formula->operate_PropagateUnit())
		{
			printf("Decided by unit propagation.\n");
			return 0;
		}
	}
	
	for (int pass = 0; pass < MAX_PASSES; ++pass)
	{
		for (int lng = MIN_LENGTH; lng <= MAX_LENGTH; ++lng)
		{
			for (int i = 0; i < formula->m_cC; ++i)
			{
				length = formula->find_conflicting_path(&formula->m_clauses[i], path, lng);
				formula->print_path(path, length);

				if (length > 1)
				{
					sIncrementalGraph graph;
					Clustering clustering;
					Layering layering;
					
					result = formula->build_ConflictPath(path, length, &graph, &clustering, &layering);
					if (result < 0)
					{
						fprintf(stderr, "Unable to build conflict path (result:%d).\n", result);
						return result;
					}
					formula->calculate_MaximumVisits(&layering);
					layering.init_visit_matrices(layering.m_cL, clustering.m_cC);
//					layering.print_visits();
					
					formula->check_paths_APC(&clustering, &layering);

					layering.destroy_visit_matrices();
					layering.destroy();
					clustering.destroy();
					graph.destroy();
				}
			}
		}
	}
//	formula->print_DIMACS();
	fprintf(stderr, "Clauses:%ld\n", formula->m_binary_Clauses.size());
	printf("Clauses:%ld\n", formula->m_binary_Clauses.size());

	return 0;
}


int test_path_consistency_BPC(sExperimentalBooleanFormula *formula)
{
	int result;
	int length;
	sClause *path[MAX_LENGTH + 10];

	if (formula->m_conflicting)
	{
		printf("Formula is conflicting.\n");
		return 0;
	}
	else
	{
		if (!formula->operate_PropagateUnit())
		{
			printf("Decided by unit propagation.\n");
			return 0;
		}
	}
	
	for (int pass = 0; pass < MAX_PASSES; ++pass)
	{
		for (int lng = MIN_LENGTH; lng <= MAX_LENGTH; ++lng)
		{
			for (int i = 0; i < formula->m_cC; ++i)
			{
				length = formula->find_conflicting_path(&formula->m_clauses[i], path, lng);
//				formula->print_path(path, length);

				if (length > 1)
				{
					sIncrementalGraph graph;
					Clustering clustering;
					Layering layering;
					
					result = formula->build_ConflictPath(path, length, &graph, &clustering, &layering);
					if (result < 0)
					{
						fprintf(stderr, "Unable to build conflict path (result:%d).\n", result);
						return result;
					}
					formula->calculate_MaximumVisits(&layering);
					layering.init_visit_matrices(layering.m_cL, clustering.m_cC);
//					layering.print_visits();
					
					formula->check_paths_BPC(&clustering, &layering);

					layering.destroy_visit_matrices();
					layering.destroy();
					clustering.destroy();
					graph.destroy();
				}
			}
		}
	}
//	formula->print_DIMACS();
	fprintf(stderr, "Clauses:%ld\n", formula->m_binary_Clauses.size());
	printf("Clauses:%ld\n", formula->m_binary_Clauses.size());

	return 0;
}


int test_path_consistency_EPC(sExperimentalBooleanFormula *formula)
{
	int result;
	int length;
	sClause *path[MAX_LENGTH + 10];
	clock_t begin_1, end_1;

	if (formula->m_conflicting)
	{
		printf("Formula is conflicting.\n");
		return 0;
	}
	else
	{
		if (!formula->operate_PropagateUnit())
		{
			printf("Decided by unit propagation.\n");
			return 0;
		}
	}

/*
        fprintf(stderr, "Building literal conflict graph...\n");
	begin_1 = clock();
	sIncrementalGraph literal_conflict;
	result = formula->build_LiteralConflictGraph(&literal_conflict);
	end_1 = clock();
	fprintf(stderr, "Time (literal conflict graph): %.3f s\n", (end_1 - begin_1) / (double)CLOCKS_PER_SEC);

        fprintf(stderr, "Building clause conflict graph...\n");
	begin_1 = clock();
	sIncrementalGraph clause_conflict;
	result = formula->build_ClauseConflictGraph(&clause_conflict);
	end_1 = clock();
	fprintf(stderr, "Time (clause conflict graph): %.3f s\n", (end_1 - begin_1) / (double)CLOCKS_PER_SEC);
*/
	for (int i = 0; i < formula->m_cC; ++i)
	{
		formula->m_clauses[i].m_checked = false;
	}
	for (int pass = 0; pass < MAX_PASSES; ++pass)
	{
		for (int lng = MIN_LENGTH; lng <= MAX_LENGTH; ++lng)
		{
			int N_paths;
			if (formula->m_V > sLARGE_FORMULA_LIMIT)
			{
				N_paths = sqrt(formula->m_V);
			}
			else
			{
				N_paths = formula->m_V;
			}

			for (int i = 0; i < N_paths; ++i)
			{
				int rnd_clause = rand() % formula->m_cC;
				sClause *first_clause = &formula->m_clauses[rnd_clause];

				while (first_clause->m_checked)
				{
					rnd_clause = (rnd_clause + 1) % formula->m_cC;
					first_clause = &formula->m_clauses[rnd_clause];
				}
				first_clause->m_checked = true;

				length = formula->find_conflicting_path(first_clause, path, lng);
//				formula->print_path_verbose(path, length);

				if (length > 1)
				{
					sIncrementalGraph graph;
					Clustering clustering;

					result = formula->build_ConflictPath(path, length, &graph, &clustering);				
//					result = formula->build_ConflictPath_2(path, length, &literal_conflict, &graph, &clustering);
					if (result < 0)
					{
						fprintf(stderr, "Unable to build conflict path (result:%d).\n", result);
						return result;
					}
					formula->check_paths_EPC(&clustering);
					
					clustering.destroy();
					graph.destroy();
				}
			}
		}
	}
	formula->print_DIMACS();
	fprintf(stderr, "Clauses:%ld\n", formula->m_binary_Clauses.size());
//	printf("Clauses:%ld\n", formula->m_binary_Clauses.size());

	return 0;
}


int test_path_consistency_LPC(sExperimentalBooleanFormula *formula, int lpc_limit)
{
	int result;
	int length;
	sClause *path[MAX_LENGTH + 10];

	if (formula->m_conflicting)
	{
		printf("Formula is conflicting.\n");
		return 0;
	}
	else
	{
		if (!formula->operate_PropagateUnit())
		{
			printf("Decided by unit propagation.\n");
			return 0;
		}
	}

	for (int i = 0; i < formula->m_cC; ++i)
	{
		formula->m_clauses[i].m_checked = false;
	}
	for (int pass = 0; pass < MAX_PASSES; ++pass)
	{
		for (int lng = MIN_LENGTH; lng <= MAX_LENGTH; ++lng)
		{
			int N_paths;
			if (formula->m_V > sLARGE_FORMULA_LIMIT)
			{
				N_paths = sqrt(formula->m_V);
			}
			else
			{
				N_paths = formula->m_V;
			}

			for (int i = 0; i < N_paths; ++i)
			{
				int rnd_clause = rand() % formula->m_cC;
				sClause *first_clause = &formula->m_clauses[rnd_clause];

				while (first_clause->m_checked)
				{
					rnd_clause = (rnd_clause + 1) % formula->m_cC;
					first_clause = &formula->m_clauses[rnd_clause];
				}
				first_clause->m_checked = true;

				length = formula->find_conflicting_path(first_clause, path, lng);
//				formula->print_path_verbose(path, length);

				if (length > 1)
				{
					sIncrementalGraph graph;
					Clustering clustering;
				
					result = formula->build_ConflictPath(path, length, &graph, &clustering);
					if (result < 0)
					{
						fprintf(stderr, "Unable to build conflict path (result:%d).\n", result);
						return result;
					}
					formula->check_paths_LPC(&clustering, lpc_limit);
					
					clustering.destroy();
					graph.destroy();
				}
			}
		}
	}
	formula->print_DIMACS();
//	fprintf(stderr, "Clauses:%ld\n", formula->m_binary_Clauses.size());
//	printf("Clauses:%ld\n", formula->m_binary_Clauses.size());

	return 0;
}


int test_path_consistency_LPC_2(sExperimentalBooleanFormula *formula, int lpc_limit)
{
	int result;
	int length;
	sClause *path[MAX_LENGTH + 10];

	clock_t begin_1, end_1;

	if (formula->m_conflicting)
	{
		printf("Formula is conflicting.\n");
		return 0;
	}
	else
	{
		if (!formula->operate_PropagateUnit())
		{
			printf("Decided by unit propagation.\n");
			return 0;
		}
	}
	
        fprintf(stderr, "Building literal conflict graph...\n");
	begin_1 = clock();
	sIncrementalGraph literal_conflict;
	result = formula->build_LiteralConflictGraph(&literal_conflict);
	end_1 = clock();
	fprintf(stderr, "Time (literal conflict graph): %.3f s\n", (end_1 - begin_1) / (double)CLOCKS_PER_SEC);
/*
        fprintf(stderr, "Building clause conflict graph...\n");
	begin_1 = clock();
	sIncrementalGraph clause_conflict;
	result = formula->build_ClauseConflictGraph(&clause_conflict);
	end_1 = clock();
	fprintf(stderr, "Time (clause conflict graph): %.3f s\n", (end_1 - begin_1) / (double)CLOCKS_PER_SEC);

        fprintf(stderr, "Building occurence conflict graph...\n");
	begin_1 = clock();
	sIncrementalGraph occurence_conflict;
	result = formula->build_OccurenceConflictGraph(&occurence_conflict);
	end_1 = clock();
	fprintf(stderr, "Time (occurence conflict graph): %.3f s\n", (end_1 - begin_1) / (double)CLOCKS_PER_SEC);
*/
//	clause_conflict.print();

	for (int i = 0; i < formula->m_cC; ++i)
	{
		formula->m_clauses[i].m_checked = false;
	}
	
	for (int pass = 0; pass < MAX_PASSES; ++pass)
	{
		int N_paths;
		if (formula->m_V > sLARGE_FORMULA_LIMIT)
		{
			N_paths = sMIN(4.0 * sqrt(formula->m_V) * (1.0 / sINDEPENDENT_HEURISTIC_PROB), formula->m_C);
		}
		else
		{
			N_paths = sMIN(formula->m_V * (1.0 / sINDEPENDENT_HEURISTIC_PROB), formula->m_C);
		}

		for (int i = 0; i < N_paths; ++i)
		{
			int rnd_clause = rand() % formula->m_cC;
			sClause *first_clause = &formula->m_clauses[rnd_clause];
			
			while (first_clause->m_checked)
			{
				rnd_clause = (rnd_clause + 1) % formula->m_cC;
				first_clause = &formula->m_clauses[rnd_clause];
			}
			first_clause->m_checked = true;

			int lng = 3 + rand() % MAX_LENGTH;
			double rnd_heuristic = (double)rand() / RAND_MAX;

			if (sINDEPENDENT_HEURISTIC_PROB < rnd_heuristic)
			{
				length = formula->find_conflicting_path(first_clause, path, lng);
			}
			else
			{
				length = formula->find_ConflictingPath_6(first_clause, &literal_conflict, path, lng);
			}

			if (length > 1)
			{
				sIncrementalGraph graph;
				Clustering clustering;
				Layering layering;
				
				result = formula->build_ConflictPath_2(path, length, &literal_conflict, &graph, &clustering, &layering);
				if (result < 0)
				{
					fprintf(stderr, "Unable to build conflict path (result:%d).\n", result);
					return result;
				}
				formula->check_paths_LPC_2(&literal_conflict, &clustering, &layering, lpc_limit);
				
				layering.destroy();					
				clustering.destroy();
				graph.destroy();
			}
		}
	}
	formula->print_DIMACS();
	fprintf(stderr, "Clauses:%ld\n", formula->m_binary_Clauses.size());
//	printf("Clauses:%ld\n", formula->m_binary_Clauses.size());

	return 0;
}


int test_flow_consistency(sExperimentalBooleanFormula *formula)
{
	int result;
	sIncrementalGraph graph;

	clock_t begin_1, end_1;
	begin_1 = clock();
//	result = formula->build_occurence_conflict_graph(&graph);
//	result = formula->build_sparse_conflict_graph(&graph);

	for (int i = 0; i < 1; ++i)
	{
		sClause *clause = &formula->m_clauses[i];

		if (!sExperimentalBooleanFormula::is_Satisfied(*clause))
		{
			if (!sExperimentalBooleanFormula::is_Falsified(*clause))
			{
				for (int j = 0; j < clause->m_cL; ++j)
				{
					sLiteral *literal = clause->m_literals[j];
					{
						if (!sExperimentalBooleanFormula::is_Assigned(*literal))
						{
							printf("Assigning:%d,%d\n", i, j);
							formula->operate_SetPropagate_TRUE(literal);
							break;
						}
					}
				}
			}
			else
			{
				printf("Falsified\n");
			}
		}
	}

	result = formula->build_partial_sparse_conflict_graph(&graph);
		if (result < 0)
		{
			fprintf(stderr, "Unable to build occurence conflict graph (result:%d)\n", result);
		}
		end_1 = clock();
//	fprintf(stderr, "Time (build_occurence_conflict_graph): %.3f s\n", (end_1 - begin_1) / (double)CLOCKS_PER_SET);
//	fprintf(stderr, "Time (build_sparse_conflict_graph): %.3f s\n", (end_1 - begin_1) / (double)CLOCKS_PER_SEC);
	
		sIncrementalClustering clustering;

		clock_t begin_2, end_2;
		begin_2 = clock();
		result = formula->build_occurence_clustering_1(&graph, &clustering);
		if (result < 0)
		{
			fprintf(stderr, "Unable to build occurence clustering (result:%d)\n", result);
		}
		end_2 = clock();
//	fprintf(stderr, "Time (build_occurence_clustering_1): %.3f s\n", (end_2 - begin_2) / (double)CLOCKS_PER_SEC);

//	graph.print_flow_specific();
		graph.print_flow_specific_nontrivial();
//	graph.export_GR();
//	clustering.print_nontrivial();
	formula->calc_ClusteringCapacity(&graph, &clustering);

		clustering.destroy();
		graph.destroy();
//	}

	return 0;
}


int test_flow_model(sExperimentalBooleanFormula *formula)
{
	int result;

	if (formula->m_conflicting)
	{
		printf("Formula is conflicting.\n");
		return 0;
	}
	else
	{
		if (!formula->operate_PropagateUnit())
		{
			printf("Decided by unit propagation.\n");
			return 0;
		}
	}

	for (int arity = 2; arity < 16; ++arity)
	{
		sFlowModel_1 flow_model;
		result = flow_model.init(formula);
		if (result < 0)
		{
			fprintf(stderr, "Unable to initialize flow model (result:%d)\n", result);
			return result;
		}

		flow_model.build_Network(arity);
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
		int sat = formula->calc_SelectedClauses();
		printf("Arity = %d ... clauses x flow: %d x %d\n", arity, sat, flow);
		if (sat > flow)
		{
			printf("UNSAT !\n");
		}
		else
		{
			printf("Cannot decide...\n");
		}
		flow_model.destroy();
	}

	return 0;
}


int main(int argc, char **argv)
{
	int result;
	COMMAND cmd;
	char *filename;
	int lpc_limit_exp;
	int lpc_limit = sDEFAULT_LPC_LIMIT;

	clock_t begin, end;

	begin = clock();

	print_intro();

	if (argc >= 3)
	{
		if (strcmp(argv[1], "-se1") == 0)
		{
			cmd = SOLVE_EXP_1;
			filename = argv[2];
		}
		else if (strcmp(argv[1], "-tspc") == 0)
		{
			cmd = TEST_SPC;
			filename = argv[2];
		}
		else if (strcmp(argv[1], "-tapc") == 0)
		{
			cmd = TEST_APC;
			filename = argv[2];
		}
		else if (strcmp(argv[1], "-tbpc") == 0)
		{
			cmd = TEST_BPC;
			filename = argv[2];
		}
		else if (strcmp(argv[1], "-tepc") == 0)
		{
			cmd = TEST_EPC;
			filename = argv[2];
		}
		else if (strcmp(argv[1], "-tlpc") == 0)
		{
			cmd = TEST_LPC;

			if (argc >= 4)
			{
				sscanf(argv[2], "-l%d", &lpc_limit_exp);
				if (lpc_limit_exp >= 0)
				{
					lpc_limit = pow(2, lpc_limit_exp);
				}
				filename = argv[3];
			}
			else
			{
				filename = argv[2];
			}
		}
		else if (strcmp(argv[1], "-tlpc2") == 0)
		{
			int acceptable_limit;
			cmd = TEST_LPC2;

			if (argc >= 3)
			{
				sscanf(argv[2], "-l%d", &lpc_limit_exp);
				if (lpc_limit_exp >= 0)
				{
					lpc_limit = pow(2, lpc_limit_exp);
				}
			}
			if (argc >= 3)
			{
				sscanf(argv[3], "-lp%lf", &LOWER_PATH_INDEPENDENT);
			}
			if (argc >= 4)
			{
				sscanf(argv[4], "-up%lf", &UPPER_PATH_INDEPENDENT);
			}
			if (argc >= 5)
			{
				sscanf(argv[5], "-al%d", &acceptable_limit);
			}
			if (acceptable_limit >= 0)
			{
				ACCEPTABLE_LIMIT = pow(2, acceptable_limit);
			}
			else
			{
				ACCEPTABLE_LIMIT = 0;
			}
			filename = argv[argc - 1];
		}
		else if (strcmp(argv[1], "-tfc") == 0)
		{
			cmd = TEST_FC;
			filename = argv[2];
		}
		else if (strcmp(argv[1], "-tfm") == 0)
		{
			cmd = TEST_FM;
			filename = argv[2];
		}
		else
		{
			cmd = UNDEFINED;
		}
	}
	else
	{
		cmd = UNDEFINED;
	}

	switch (cmd)
	{
	case SOLVE_EXP_1:
	{
		sExperimentalBooleanFormula formula;
		formula.load_DIMACS(filename);

//		formula.print();

		bool solved;

		switch (cmd)
		{
		case SOLVE_EXP_1:
		{
		    solved = formula.solve_Experimental_1();
		    break;
		}
		default:
		{
		    break;
		}
		}

		end = clock();

		if (solved)
		{
			printf("SAT\n");
		}
		else
		{
			printf("UNSAT\n");
		}

#ifdef STAT
		fprintf(stderr, "Decisions:%d\n", STAT_DECISIONS);
		fprintf(stderr, "Backtracks:%d\n", STAT_BACKTRACKS);
		fprintf(stderr, "Propagations:%d\n", STAT_PROPAGATIONS);
#endif
		fprintf(stderr, "Time:%.3f\n", (end - begin) / (double)CLOCKS_PER_SEC);
		break;
	}
	case TEST_SPC:
	{
		fprintf(stderr,"%s\n", filename);
		sExperimentalBooleanFormula formula;
		formula.load_DIMACS(filename);

		result = test_path_consistency_SPC(&formula);
		end = clock();

		if (result < 0)
		{
			return result;
		}
		fprintf(stderr, "Time:%.3f\n", (end - begin) / (double)CLOCKS_PER_SEC);
		break;
	}
	case TEST_APC:
	{
		fprintf(stderr, "%s\n", filename);
		sExperimentalBooleanFormula formula;
		formula.load_DIMACS(filename);

		result = test_path_consistency_APC(&formula);
		end = clock();

		if (result < 0)
		{
			return result;
		}
		fprintf(stderr, "Time:%.3f\n", (end - begin) / (double)CLOCKS_PER_SEC);
		break;
	}
	case TEST_BPC:
	{
		fprintf(stderr, "%s\n", filename);
		sExperimentalBooleanFormula formula;
		formula.load_DIMACS(filename);

		result = test_path_consistency_BPC(&formula);
		end = clock();

		if (result < 0)
		{
			return result;
		}
		fprintf(stderr, "Time:%.3f\n", (end - begin) / (double)CLOCKS_PER_SEC);
		break;
	}
	case TEST_EPC:
	{
		fprintf(stderr, "%s\n", filename);
		sExperimentalBooleanFormula formula;
		formula.load_DIMACS(filename);

		result = test_path_consistency_EPC(&formula);
		end = clock();

		if (result < 0)
		{
			return result;
		}
		fprintf(stderr, "Time:%.3f\n", (end - begin) / (double)CLOCKS_PER_SEC);
		break;
	}
	case TEST_LPC:
	{
		fprintf(stderr, "%s\n", filename);
		sExperimentalBooleanFormula formula;
		formula.load_DIMACS(filename);

		result = test_path_consistency_LPC(&formula, lpc_limit);
		end = clock();

		if (result < 0)
		{
			return result;
		}
		fprintf(stderr, "Time:%.3f\n", (end - begin) / (double)CLOCKS_PER_SEC);
		break;
	}
	case TEST_LPC2:
	{
		fprintf(stderr, "%s\n", filename);
		sExperimentalBooleanFormula formula;
		formula.load_DIMACS(filename);

		result = test_path_consistency_LPC_2(&formula, lpc_limit);
		end = clock();

		if (result < 0)
		{
			return result;
		}
		fprintf(stderr, "Time:%.3f\n", (end - begin) / (double)CLOCKS_PER_SEC);
		break;
	}
	case TEST_FC:
	{
		sExperimentalBooleanFormula formula;
		formula.load_DIMACS(filename);

		result = test_flow_consistency(&formula);
		if (result < 0)
		{
			return result;
		}
		break;
	}
	case TEST_FM:
	{
		sExperimentalBooleanFormula formula;
		formula.load_DIMACS(filename);

		result = test_flow_model(&formula);
		if (result < 0)
		{
			return result;
		}
		break;
	}
	default:
	{
		fprintf(stderr, "No command specified.\n");
		break;
	}
	}

	return 0;
}

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
// system_main.cpp / 0.71-sky
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
// Main module of the systematic part of the sigmaSAT solver.

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "version.h"
#include "config.h"
#include "system.h"


enum COMMAND
{
	UNDEFINED = 0,
	SOLVE_1   = 1,
	SOLVE_2   = 2,
	SOLVE_3   = 3
};


void print_intro(void)
{
	fprintf(stderr, "sigmaSAT: systemSIGMA %s\n", sVERSION);
	fprintf(stderr, "Copyright (C) 2009 - 2011 Pavel Surynek\n");
	fprintf(stderr, "---------------------------------------\n");
}


int main(int argc, char **argv)
{
	COMMAND cmd;
	char *filename;

	clock_t begin, end;

	begin = clock();

	print_intro();

	if (argc == 3)
	{
		if (strcmp(argv[1], "-s1") == 0)
		{
			cmd = SOLVE_1;
			filename = argv[2];
		}
		else if (strcmp(argv[1], "-s2") == 0)
		{
			cmd = SOLVE_2;
			filename = argv[2];
		}
		else if (strcmp(argv[1], "-s3") == 0)
		{
			cmd = SOLVE_3;
			filename = argv[2];
		}
		else
		{
			cmd = UNDEFINED;
		}
	}
	else
	{
		if (argc == 2)
		{
			cmd = SOLVE_3;
			filename = argv[1];
		}
		else
		{
			cmd = UNDEFINED;
		}
	}

	switch (cmd)
	{
	case SOLVE_1:
	case SOLVE_2:
	case SOLVE_3:
	{
		sBooleanFormula formula;
		formula.load_DIMACS(filename);

//		formula.print();

		bool solved;

		switch (cmd)
		{
		case SOLVE_1:
		{
		    solved = formula.solve_1();
		    break;
		}
		case SOLVE_2:
		{
		    solved = formula.solve_2();
		    break;
		}
		case SOLVE_3:
		{
		    solved = formula.solve_3();
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
	default:
	{
		fprintf(stderr, "No command specified.\n");
		break;
	}
	}

	return 0;
}

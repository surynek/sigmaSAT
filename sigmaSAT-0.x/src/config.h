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
// config.h / 0.71-sky
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
// Config module sigmaSAT solver.


#ifndef __CONFIG_H__
#define __CONFIG_H__


const int sVERTEX_BLOCK_SIZE = 256; 
const int sEDGE_BLOCK_SIZE = 256;
const int sNEIGHBOR_BLOCK_SIZE = 256;

const int sEDGE_CHUNK = 1024;
const int sNEIGHBOR_CHUNK = 1024;
const int sCLUSTER_CHUNK = 1024;

const int sCLAUSE_BLOCK_SIZE = 256;
const int sOCCURENCE_BLOCK_SIZE = 1024;

const int sTOTAL_PATH_MAX = 65536;
const int sDEFAULT_LPC_LIMIT = 512;

const int sSET_USAGE_LIMIT = 16;
const int sLARGE_FORMULA_LIMIT = 2048;

const double sLOWER_PATH_FACTOR = 3.0;
const double sUPPER_PATH_FACTOR = 6.0;

const double sDEFAULT_LOWER_PATH_INDEPENDENT = 1.0;
const double sDEFAULT_UPPER_PATH_INDEPENDENT = 16.0;

const int MIN_LENGTH = 16;
const int MAX_LENGTH = 16;
const int MAX_PASSES = 1;

const int sDEFAULT_ACCEPTABLE_LIMIT = 1;

const double sINDEPENDENT_HEURISTIC_PROB = 0.25;

extern double LOWER_PATH_INDEPENDENT;
extern double UPPER_PATH_INDEPENDENT;
extern int ACCEPTABLE_LIMIT;


#define STAT	1


#ifdef STAT
  extern int STAT_DECISIONS;
  extern int STAT_PROPAGATIONS;
  extern int STAT_BACKTRACKS;
#endif


#endif /* __CONFIG_H__ */

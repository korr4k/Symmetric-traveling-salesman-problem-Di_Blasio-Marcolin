#ifndef MIPUTILITIES_H_							

#define MIPUTILITIES_H_

#include <cplex.h>
#include "TSP.h"

void mipSetLevelForAllCuts(CPXENVptr env, int level);

void mipTimelimit(CPXENVptr env, double timelimit, instance *inst);

int mipUpdateOIncumbent(CPXENVptr env, CPXLPptr lp, instance *inst);

double mipValue(CPXENVptr env, CPXLPptr lp);

int mipSolutioHash(CPXENVptr env, CPXLPptr lp);

int mipSolutionAvailable(CPXENVptr env, CPXLPptr lp);

#endif   /* MIPUTILITIES_H_ */ 
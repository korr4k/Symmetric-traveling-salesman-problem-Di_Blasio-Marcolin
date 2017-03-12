#include <cplex.h>
#include "TSP.h"
#include "MipUtilities.h"

void mipSetLevelForAllCuts(CPXENVptr env, int level)
{
	CPXsetintparam(env, CPX_PARAM_CLIQUES, level);
	CPXsetintparam(env, CPX_PARAM_COVERS, level);
	CPXsetintparam(env, CPX_PARAM_DISJCUTS, level);
	CPXsetintparam(env, CPX_PARAM_FLOWCOVERS, level);
	CPXsetintparam(env, CPX_PARAM_FLOWPATHS, level);
	CPXsetintparam(env, CPX_PARAM_FRACCUTS, level);
	CPXsetintparam(env, CPX_PARAM_GUBCOVERS, level);
	CPXsetintparam(env, CPX_PARAM_IMPLBD, level);
	CPXsetintparam(env, CPX_PARAM_MIRCUTS, level);
	CPXsetintparam(env, CPX_PARAM_ZEROHALFCUTS, level);
	CPXsetintparam(env, CPX_PARAM_LANDPCUTS, level);
	CPXsetintparam(env, CPX_PARAM_MCFCUTS, level);
}

void mipTimelimit(CPXENVptr env, double timelimit, instance *inst)
{
	double residualTime = inst->tStart + inst->timeLimit - (double)(clock()/CLOCKS_PER_SEC);
	if (residualTime < 0.0) residualTime = 0.0;
	CPXsetintparam(env, CPX_PARAM_CLOCKTYPE, 2);
	CPXsetdblparam(env, CPX_PARAM_TILIM, residualTime); 							// real time
	CPXsetdblparam(env, CPX_PARAM_DETTILIM, TICKS_PER_SECOND*timelimit);			// ticks
}

int mipUpdateOIncumbent(CPXENVptr env, CPXLPptr lp, instance *inst)
{
	int ncols = CPXgetnumcols(env, lp);

	int newsol = 0;

	if (mipValue(env, lp) < inst->zBest - EPSILON)
	{
		inst->tBest = (double)(clock()/CLOCKS_PER_SEC) - inst->tStart;
		inst->zBest = mipValue(env, lp);
		CPXgetx(env, lp, inst->bestSol, 0, ncols - 1);
		if (VERBOSE >= 40) printf("\n >>>>>>>>>> incumbent update of value %lf at time %7.2lf <<<<<<<<\n", inst->zBest, inst->tBest);
		newsol = 1;
	}

	// save the solution in a file (does not work if the callbacks changed it...)
	if (newsol && (VERBOSE >= 10))
	{
		if (VERBOSE >= 100) CPXwritemipstarts(env, lp, "model.mst", 0, 0);
		printf("... New incumbent of value %20.5lf (hash %12d) found after %7.2lf sec.s \n", inst->zBest, mipSolutioHash(env, lp), inst->tBest);
		fflush(NULL);
	}

	return newsol;
}

double mipValue(CPXENVptr env, CPXLPptr lp)
{
	double zz;
	if (CPXgetobjval(env, lp, &zz)) zz = CPX_INFBOUND;
	return zz;
}

int mipSolutioHash(CPXENVptr env, CPXLPptr lp)
{
	if (!mipSolutionAvailable(env, lp)) return -1;

	int nCols = CPXgetnumcols(env, lp);
	double *xStar = (double *)calloc(nCols, sizeof(double));
	char *cType = (char *)calloc(nCols, sizeof(char));
	CPXgetx(env, lp, xStar, 0, nCols - 1);
	CPXgetctype(env, lp, cType, 0, nCols - 1);

	const int maxHash = 123456789;
	int hash = 0;
	for (int i = 0; i < nCols; i++)
	{
		if (cType[i] == 'B' && xStar[i] > 0.5) hash = (hash + i*i + 17 * i) % maxHash;
	}
	free(cType);
	free(xStar);
	return hash;
}

int mipSolutionAvailable(CPXENVptr env, CPXLPptr lp)
{
	double zz;
	if (CPXgetobjval(env, lp, &zz)) return 0;
	return 1;
}
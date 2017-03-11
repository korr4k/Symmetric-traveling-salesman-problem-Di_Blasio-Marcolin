#include "TSP.h"

int VRPopt(instance *inst)
{
	inst->tStart = (double)clock();
	inst->bestLb = -CPX_INFBOUND;

	// open cplex model
	int error;
	CPXENVptr env = CPXopenCPLEX(&error);	//apro env di cplex
	CPXLPptr lp = CPXcreateprob(env, &error, "TSP");	//creo lp relativo all'env creato, error conterrà eventuali errori (se != 0)

	if (error != 0)
		printError("... errore nella definizione iniziale dell'environment e del linear problem");

	// cplex output
	CPXsetintparam(env, CPX_PARAM_MIPDISPLAY, 4);						// 3??
	if (VERBOSE >= 60) CPXsetintparam(env, CPX_PARAM_SCRIND, CPX_ON); // Cplex output on screen

	// precision
	CPXsetdblparam(env, CPX_PARAM_EPINT, 0.0);
	CPXsetdblparam(env, CPX_PARAM_EPGAP, 1e-9);
	CPXsetdblparam(env, CPX_PARAM_EPRHS, 1e-9);

	// heuristics
	//CPXsetintparam(env, CPX_PARAM_FPHEUR, 1); 		// feasibility pump for feasibility (=1)  
	CPXsetintparam(env, CPX_PARAM_RINSHEUR, 10);    	// heuristic RINS frequency (=50 or alike)
														//CPXsetintparam(env, CPX_PARAM_LBHEUR, 1);			// local branching      
														//CPXsetintparam(env, CPX_PARAM_HEURFREQ, 10);    

														//if ( CPXsetintparam(env, CPX_PARAM_BRDIR, inst->branch) ) print_error("wrong branching flag (-1,0,1)");      
														//CPXsetintparam(env, CPX_PARAM_NODESEL, 0);			// depth first      

														//cuts and symmetry
	mipSetLevelForAllCuts(env, 2);
	//mip_set_level_for_all_cuts(env, 3); CPXsetintparam(env, CPX_PARAM_DISJCUTS, 2);
	//CPXsetintparam(env, CPX_PARAM_LOCALIMPLBD, 3);			// aggressive local implied bounds  
	//CPXsetintparam(env, CPX_PARAM_SYMMETRY, 5);			// symmetry

	buildModel(inst, env, lp);

	int nCols = CPXgetnumcols(env, lp);
	inst->bestSol = (double *)calloc(nCols, sizeof(double)); 	// all entries to zero  
	inst->zBest = CPX_INFBOUND;

	mipTimelimit(env, CPX_INFBOUND, inst);
	if (VERBOSE >= 50) CPXsetintparam(env, CPX_PARAM_SCRIND, CPX_ON);
	if (timeLimitExpired(inst)) goto EXIT;
	if (VERBOSE >= 100) CPXwriteprob(env, lp, "final.lp", NULL);
	CPXsetintparam(env, CPX_PARAM_NODELIM, 0); // 0 and not 1      
	for (int k = 0; k < 10; k++) CPXmipopt(env, lp); // 10 restarts 
	CPXsetintparam(env, CPX_PARAM_NODELIM, 2000000000);
	CPXmipopt(env, lp);	//risolve  

EXIT:

	CPXgetbestobjval(env, lp, &inst->bestLb);
	mipUpdateOIncumbent(env, lp, inst);

	CPXfreeprob(env, &lp);
	CPXcloseCPLEX(&env);

	return 0;
}

int timeLimitExpired(instance *inst)
{
	double tspan = (double)(clock() / CLOCKS_PER_SEC) - inst->tStart;
	if (tspan > inst->timeLimit)
	{
		if (VERBOSE >= 100) printf("\n\n$$$ time limit of %10.1lf sec.s expired after %10.1lf sec.s $$$\n\n", inst->timeLimit, tspan);
		return 1;
	}
	return 0;
}
#ifndef TSP_H_							

#define TSP_H_

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h> 
#include <stdio.h>  

#include <cplex.h>  
#include <pthread.h> 

#define VERBOSE				    50		// printing level  (=10 only incumbent, =20 little output, =50-60 good, =70 verbose, >=100 cplex log)

//hard-wired parameters
#define XSMALL		  		  1e-5 		// 1e-4*	// tolerance used to decide ingerality of 0-1 var.s
#define EPSILON		  		  1e-9		// 1e-9		// very small numerical tolerance 
#define TICKS_PER_SECOND 	  1000.0  	// cplex's ticks on Intel Core i7 quadcore @2.3GHZ

//data structures  

typedef struct {

	double x;
	double y;

} point;

typedef struct {

	//input data
	int nNodes;
	point *coord;


	// parameters 
	char edgeType[1000];
	double timeLimit;						// overall time limit, in sec.s
	char inputFile[1000];		  			// input file

	//global data
	double	tStart;
	double zBest;							// best sol. available  
	double tBest;							// time for the best sol. available  
	double *bestSol;						// best sol. available    
	double	bestLb;						// best lower bound available

											// model;     
	int xStart;
	int qStart;
	int bigQStart;
	int sStart;
	int bigSStart;
	int yStart;
	int fStart;
	int zStart;
} instance;

//methods

void buildModel(instance *inst, CPXENVptr env, CPXLPptr lp);
void printError(const char *err);
void printCoord(instance *inst);
void debug(const char *err);
void freeInstance(instance *inst);
int dist(point p1, point p2, char* typeP);

//usefull methods

void printError(const char *err) { printf("\n\n ERROR: %s \n\n", err); fflush(NULL); exit(1); }

void printCoord(instance *inst) 
{
	printf("----------------------------------------------------------------------------------------------\n");
	for (int i = 0; i < inst->nNodes; i++)
		printf("Point #%d: (%.3f,%.3f)\n", i + 1, inst->coord[i].x, inst->coord[i].y);
}

void debug(const char *err) { printf("\nDEBUG: %s \n", err); fflush(NULL); }

void freeInstance(instance *inst)
{
	free(inst->coord);
	//free(inst->loadMin);
	//free(inst->loadMax);
}

int dist(point p1, point p2, char* typeP)
{
	double xD;
	double yD;
	int dIJ;

	if (strcmp(typeP, "EUC_2D") == 0)
	{
		xD = p1.x - p2.x;
		yD = p1.x - p2.y;
		return dIJ = (int)(sqrt(xD*xD + yD*yD) + 0.5);
	}
	else if (strcmp(typeP, "ATT") == 0)
	{
		xD = p1.x - p2.x;
		yD = p1.x - p2.y;
		double tmp1 = sqrt(xD*xD + yD*yD) / 10.0;
		dIJ = (sqrt(xD*xD + yD*yD) / 10.0 + 0.5);
		if (dIJ < tmp1) dIJ = tmp1 + 1;
		else dIJ = tmp1;
		return dIJ;		//always integer
	}

	return -1;
}

#endif   /* TSP_H_ */ 
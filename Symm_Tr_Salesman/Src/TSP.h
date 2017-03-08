#ifndef TSP_H_							// "carico" solo una volta il file vrp.h

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

} points;

typedef struct {

	//input data
	int nNodes;
	points *coord;
	
	

	// parameters 
	char *modelName;
	int modelType;
	int oldBenders;
	int randomSeed;
	int nThreads;
	double timeLimit;						// overall time limit, in sec.s
	char inputFile[1000];		  			// input file
	char nodeFile[1000];		  			// cplex node file
	int availableMemory;
	int maxNodes; 							// max n. of branching nodes in the final run (-1 unlimited)
	double cutOff; 							// cutoff (upper bound) for master
	int integerCosts;

	//global data
	double	tStart;
	double zBest;							// best sol. available  
	double tBest;							// time for the best sol. available  
	double *bestSol;						// best sol. available    
	double	bestLb;						// best lower bound available  
	double *loadMin;						// minimum load when leaving a node
	double *loadMax;						// maximum load when leaving a node

											// model;     
	int xStart;
	int qStart;
	int bigQStart;
	int sStart;
	int bigsStart;
	int yStart;
	int fStart;
	int zStart;
} instance;

//usefull methods

void print_error(const char *err) { printf("\n\n ERROR: %s \n\n", err); fflush(NULL); exit(1); }

void debug(const char *err) { printf("\nDEBUG: %s \n", err); fflush(NULL); }

void free_instance(instance *inst)
{
	free(inst->coord);
	free(inst->loadMin);
	free(inst->loadMax);
}

void emptyLines(instance inst) {

	if (VERBOSE >= 100) {

		int numEmptyLines = number_of_nonempty_lines(inst.inputFile);

		if (numEmptyLines == 0) {

			printf("File %s is empty", inst.inputFile); exit(1);

		}

		printf("%d non-empty lines has been read", numEmptyLines);
	}
}

int number_of_nonempty_lines(const char *file)  // warning: the last line NOT counted if it is does not terminate with \n (as it happens with some editors) 
{
	FILE *fin = fopen(file, "r");
	if (fin == NULL) return 0;
	char line[123456];
	int count = 0;
	while (fgets(line, sizeof(line), fin) != NULL) { printf(" len %4d\n", (int)strlen(line)); if (strlen(line) > 1) count++; }
	fclose(fin);
	return count;
}

void time(clock_t begin, clock_t end) {

	if (VERBOSE >= 1) { printf("\nCOMPLETED IN %.3f SECONDS\n", (double)(end - begin) / CLOCKS_PER_SEC); }

}

//inline
inline int imax(int i1, int i2) { return (i1 > i2) ? i1 : i2; }
inline double dmin(double d1, double d2) { return (d1 < d2) ? d1 : d2; }
inline double dmax(double d1, double d2) { return (d1 > d2) ? d1 : d2; }

#endif   /* TSP_H_ */ 
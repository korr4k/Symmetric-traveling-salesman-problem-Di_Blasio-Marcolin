#include "TSP.h"           

void readInput(instance *inst);
void parseCommandLine(int argc, char** argv, instance *inst);

int main(int argc, char **argv)
{
	if (argc < 2) { printf("Usage: %s -help for help\n", argv[0]); exit(1); }	
	if (VERBOSE >= 2) { for (int a = 0; a < argc; a++) printf("%s ", argv[a]); printf("\n"); }	

	clock_t begin = clock();	//Starting to precess

	instance inst;

	parseCommandLine(argc, argv, &inst);		//initializaion of the instance based on input command line

	emptyLines(inst);	//calls emptyLines() in TSP.h

	readInput(&inst);		//reading the input file

	//if (VRPopt(&inst)) print_error(" error within VRPopt()");		//se !=0 c'è un errore e comunico la cosa

	clock_t end = clock();	//Ending process

	time(begin, end);	//Computing the time to execute the process		

	freeInstance(&inst);

	system("PAUSE");

	return 0;
}

void readInput(instance *inst) // simplified CVRP parser, not all SECTIONs detected  
{

	FILE *fin = fopen(inst->input_file, "r");
	if (fin == NULL) print_error(" input file not found!");

	inst->nnodes = -1;

	char line[180];
	char *par_name;
	char *token1;
	char *token2;

	int reading_coordinates = 0;	//ad 1 significa che siamo nella lettura delle coordinate

	int do_print = (VERBOSE >= 1000);

	while (fgets(line, sizeof(line), fin) != NULL)	//leggo una linea se c'è e la salvo in line
	{
		if (VERBOSE >= 2000) { printf("%s", line); fflush(NULL); }
		if (strlen(line) <= 1) continue; // skip empty lines
		par_name = strtok(line, " :");
		if (VERBOSE >= 3000) { printf("parameter \"%s\" ", par_name); fflush(NULL); }

		if (strncmp(par_name, "NAME", 4) == 0)
		{
			token1 = strtok(NULL, ": \n");
			if (VERBOSE >= 10) printf("\nNAME : %s\n", token1, inst->model_name);
			continue;
		}

		if (strncmp(par_name, "COMMENT", 7) == 0)
		{
			token1 = strtok(NULL, "\n");
			if (VERBOSE >= 10) printf("COMMENT %s\n", token1, inst->model_type);
			continue;
		}

		if (strncmp(par_name, "TYPE", 4) == 0)
		{
			token1 = strtok(NULL, ": \n");
			if (strncmp(token1, "TSP", 3) != 0) print_error(" format error:  only TYPE == TSP implemented so far!!!!!!");
			if (VERBOSE >= 10) printf("TYPE : %s\n", token1);
			continue;
		}

		if (strncmp(par_name, "DIMENSION", 9) == 0)
		{
			if (inst->nnodes >= 0) print_error(" repeated DIMENSION section in input file");
			token1 = strtok(NULL, ": \n");
			inst->nnodes = atoi(token1);
			if (VERBOSE >= 10) printf("DIMENSION : %d\n", inst->nnodes);
			inst->xcoord = (double *)calloc(inst->nnodes, sizeof(double));
			inst->ycoord = (double *)calloc(inst->nnodes, sizeof(double));
			continue;
		}

		if (strncmp(par_name, "EDGE_WEIGHT_TYPE", 16) == 0)
		{
			token1 = strtok(NULL, ": \n");
			if (strncmp(token1, "EUC_2D", 6) != 0) print_error(" format error:  only EDGE_WEIGHT_TYPE == EUC_2D implemented so far!!!!!!");
			if (VERBOSE >= 10) printf("EDGE_WEIGHT_TYPE : %s\n", token1);
			continue;
		}

		if (strncmp(par_name, "NODE_COORD_SECTION", 18) == 0)
		{
			if (inst->nnodes <= 0) print_error(" ... DIMENSION section should appear before NODE_COORD_SECTION section");
			reading_coordinates = 1;
			printf("NODE_COORD_SECTION\n");
			continue;
		}

		if (strncmp(par_name, "EOF", 3) == 0)
		{
			printf("EOF\n");
			break;
		}

		if (reading_coordinates == 1)
		{
			int i = atoi(par_name) - 1;
			if (i < 0 || i >= inst->nnodes) print_error(" ... unknown node in NODE_COORD_SECTION section");
			token1 = strtok(NULL, " ");
			token2 = strtok(NULL, "\n");
			inst->xcoord[i] = atof(token1);
			inst->ycoord[i] = atof(token2);
			if (VERBOSE >= 10) printf("%4d %15.7lf %15.7lf\n", i + 1, inst->xcoord[i], inst->ycoord[i]);
			continue;
		}

		print_error(" ... wrong format for the current simplified parser!!!!!!!!!");

	}

	fclose(fin);

}

void parseCommandLine(int argc, char** argv, instance *inst)
{

	if (VERBOSE >= 100) printf(" running %s with %d parameters \n", argv[0], argc - 1);

	// default   
	inst->modelType = 0;
	inst->oldBenders = 0;
	strcpy(inst->inputFile, "NULL");	
	inst->randomSeed = 0;
	inst->nThreads = 0;
	inst->timeLimit = CPX_INFBOUND;		
	inst->cutOff = CPX_INFBOUND;
	inst->integerCosts = 0;

	inst->availableMemory = 12000;   			// available memory, in MB, for Cplex execution (e.g., 12000)
	inst->maxNodes = -1; 						// max n. of branching nodes in the final run (-1 unlimited)        

	int help = 0; if (argc < 1) help = 1;
	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-file") == 0) { strcpy(inst->inputFile, argv[++i]); continue; } 			// input file
		if (strcmp(argv[i], "-time_limit") == 0) { inst->timeLimit = atof(argv[++i]); continue; }		// total time limit, atof converte stringa a float
		if (strcmp(argv[i], "-input") == 0) { strcpy(inst->inputFile, argv[++i]); continue; } 			// input file
		if (strcmp(argv[i], "-f") == 0) { strcpy(inst->inputFile, argv[++i]); continue; } 				// input file		
		if (strcmp(argv[i], "-model_type") == 0) { inst->modelType = atoi(argv[++i]); continue; } 	// model type, atoi converte stringa a int
		if (strcmp(argv[i], "-old_benders") == 0) { inst->oldBenders = atoi(argv[++i]); continue; } 	// old benders
		if (strcmp(argv[i], "-model") == 0) { inst->modelType = atoi(argv[++i]); continue; } 			// model type
		if (strcmp(argv[i], "-seed") == 0) { inst->randomSeed = abs(atoi(argv[++i])); continue; } 		// random seed
		if (strcmp(argv[i], "-threads") == 0) { inst->nThreads = atoi(argv[++i]); continue; } 		// n. threads
		if (strcmp(argv[i], "-memory") == 0) { inst->availableMemory = atoi(argv[++i]); continue; }	// available memory (in MB)
		if (strcmp(argv[i], "-node_file") == 0) { strcpy(inst->nodeFile, argv[++i]); continue; }		// cplex's node file
		if (strcmp(argv[i], "-max_nodes") == 0) { inst->maxNodes = atoi(argv[++i]); continue; } 		// max n. of nodes
		if (strcmp(argv[i], "-cutoff") == 0) { inst->cutOff = atof(argv[++i]); continue; }				// master cutoff
		if (strcmp(argv[i], "-int") == 0) { inst->integerCosts = 1; continue; } 						// inteher costs
		if (strcmp(argv[i], "-help") == 0) { help = 1; continue; } 									// help
		if (strcmp(argv[i], "--help") == 0) { help = 1; continue; } 									// help
		
		help = 1;
	}

	if (help || (VERBOSE >= 10))		// print current parameters
	{
		printf("\n\navailable parameters (vers. 04-mar-2017) --------------------------------------------------\n");
		printf("-file %s\n", inst->inputFile);
		printf("-time_limit %lf\n", inst->timeLimit);
		printf("-model_type %d\n", inst->modelType);
		printf("-old_benders %d\n", inst->oldBenders);
		printf("-seed %d\n", inst->randomSeed);
		printf("-threads %d\n", inst->nThreads);
		printf("-max_nodes %d\n", inst->maxNodes);
		printf("-memory %d\n", inst->availableMemory);
		printf("-int %d\n", inst->integerCosts);
		printf("-node_file %s\n", inst->nodeFile);
		printf("-cutoff %lf\n", inst->cutOff);
		printf("\nenter -help or --help for help\n");
		printf("----------------------------------------------------------------------------------------------\n\n");
	}

	if (help) exit(1);

}
#include "TSP.h"
#include "Main.h"

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

	if (VRPopt(&inst)) printError(" error within VRPopt()");		//se !=0 c'è un errore e comunico la cosa

	clock_t end = clock();	//Ending process

	elabTime(begin, end);	//Computing the time to execute the process		

	freeInstance(&inst);

	system("PAUSE");

	return 0;
}

void readInput(instance *inst) // simplified CVRP parser, not all SECTIONs detected  
{
	char *path = (char *)malloc(9);
	strcpy(path, "..\\Data\\");
	path = (char *)realloc(path, 25);
	strcat(path, inst->inputFile);
	FILE *fin = fopen(path, "r");
	if (fin == NULL) printError(" input file not found!");

	inst->nNodes = -1;
	strcpy(inst->edgeType, "NULL");

	char line[180];
	char *parName;
	char *token1;
	char *token2;
	int readingCoordinates = 0;

	while (fgets(line, sizeof(line), fin) != NULL)	//leggo una linea se c'è e la salvo in line
	{
		if (VERBOSE >= 2000) { printf("%s", line); fflush(NULL); }
		if (strlen(line) <= 1) continue; // skip empty lines

		parName = strtok(line, " :");

		if (VERBOSE >= 3000) { printf("parameter \"%s\" ", parName); fflush(NULL); }

		if (strncmp(parName, "NAME", 4) == 0)
		{
			token1 = strtok(NULL, ":\n");
			if (VERBOSE >= 1) printf("\nNAME :%s\n", token1);
			continue;
		}

		if (strncmp(parName, "COMMENT", 7) == 0)
		{
			token1 = strtok(NULL, ":\n");
			if (VERBOSE >= 1) printf("COMMENT :%s\n", token1);
			continue;
		}

		if (strncmp(parName, "TYPE", 4) == 0)
		{
			token1 = strtok(NULL, ": \n");
			if (strncmp(token1, "TSP", 3) != 0) printError(" ... format error:  only TYPE == TSP implemented so far!!!!!!");
			if (VERBOSE >= 1) printf("TYPE :%s\n", token1);
			continue;
		}

		if (strncmp(parName, "DIMENSION", 9) == 0)
		{
			token1 = strtok(NULL, ":\n");
			inst->nNodes = atoi(token1);
			if (VERBOSE >= 1) printf("DIMENSION : %s\n", token1);
			inst->coord = (point *)calloc(inst->nNodes, sizeof(point));
			continue;
		}

		if (strncmp(parName, "EDGE_WEIGHT_TYPE", 16) == 0)
		{
			token1 = strtok(NULL, ": \n");
			if (strncmp(token1, "EUC_2D", 6) !=0 && strncmp(token1, "ATT", 3) != 0) printError(" ... format error:  only EDGE_WEIGHT_TYPE == EUC_2D || ATT implemented so far!!!!!!");
			strcpy(inst->edgeType, token1);
			if (VERBOSE >= 1) printf("EDGE_WEIGHT_TYPE : %s\n", token1);
			continue;
		}

		if (strncmp(parName, "NODE_COORD_SECTION", 18) == 0)
		{
			if (inst->nNodes <= 0) printError(" ... DIMENSION section should appear before NODE_COORD_SECTION section");
			if (VERBOSE >= 3000) printf("NODE_COORD_SECTION\n");
			readingCoordinates = 1;
			continue;
		}

		if (strncmp(parName, "EOF", 3) == 0)
		{
			printCoord(inst);
			printf("EOF\n");
			break;
		}

		if (readingCoordinates == 1)
		{
			int i = atoi(parName) - 1;
			if (i < 0 || i >= inst->nNodes) printError(" ... unknown node in NODE_COORD_SECTION section");
			token1 = strtok(NULL, " ");
			token2 = strtok(NULL, "\n");
			inst->coord[i].x = atof(token1);
			inst->coord[i].y = atof(token2);
			continue;
		}

		printError(" ... wrong format for the current simplified parser!!!!!!!!!");

	}

	fclose(fin);

}

void parseCommandLine(int argc, char** argv, instance *inst)
{

	if (VERBOSE >= 100) printf(" running %s with %d parameters \n", argv[0], argc - 1);

	// default
	strcpy(inst->inputFile, "NULL");

	int help = 0; if (argc < 1) help = 1;
	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-file") == 0) { strcpy(inst->inputFile, argv[++i]); continue; } 			// input file
		if (strcmp(argv[i], "-time_limit") == 0) { inst->timeLimit = atof(argv[++i]); continue; }		// total time limit, atof converte stringa a float
		if (strcmp(argv[i], "-input") == 0) { strcpy(inst->inputFile, argv[++i]); continue; } 			// input file
		if (strcmp(argv[i], "-f") == 0) { strcpy(inst->inputFile, argv[++i]); continue; } 				// input file
		if (strcmp(argv[i], "-help") == 0) { help = 1; continue; } 									// help
		if (strcmp(argv[i], "--help") == 0) { help = 1; continue; } 									// help
		
		help = 1;
	}

	if (help || (VERBOSE >= 10))		// print current parameters
	{
		printf("\n\navailable parameters (vers. 04-mar-2017) --------------------------------------------------\n");
		printf("-file %s\n", inst->inputFile);
		printf("-time_limit %lf\n", inst->timeLimit);
		printf("\nenter -help or --help for help\n");
		printf("----------------------------------------------------------------------------------------------\n\n");
	}

	if (help) exit(1);

}
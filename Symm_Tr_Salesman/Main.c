#include "TSP.h"           

void print_error(const char *err);
void read_input(instance *inst);
void parse_command_line(int argc, char** argv, instance *inst);

void debug(const char *err) { printf("\nDEBUG: %s \n", err); fflush(NULL); }
void print_error(const char *err) { printf("\n\n ERROR: %s \n\n", err); fflush(NULL); exit(1); }

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


void free_instance(instance *inst)
{
	free(inst->xcoord);
	free(inst->ycoord);
	//free(inst->load_min);
	//free(inst->load_max);
}

int main(int argc, char **argv)
{
	if (argc < 2) { printf("Usage: %s -help for help\n", argv[0]); exit(1); }		//voglio almeno due parametri, se non li ricevo stampo il comando help ed esco dal programma
	if (VERBOSE >= 2) { for (int a = 0; a < argc; a++) printf("%s ", argv[a]); printf("\n"); }	//se i parametri sono sufficienti stampo quello che mi è stato passato

	clock_t begin = clock();	//salvo istante di inizio
							//sarebbe più interessante avere il tempo dedicato dalla cpu al processo
	instance inst;

	parse_command_line(argc, argv, &inst);	//inizializzo l'input  

											//printf(" file %s has %d non-empty lines\n", inst.input_file, number_of_nonempty_lines(inst.input_file)); exit(1);

	read_input(&inst);
//	if (VRPopt(&inst)) print_error(" error within VRPopt()");		//se !=0 c'è un errore e comunico la cosa
	clock_t end = clock();	//salvo istante di fine

//	double num_clock = CLOCKS_PER_SEC;

	if (VERBOSE >= 1)
	{
		printf("\nSOLVED IN %d CLOCK CYCLES\n", end-begin);
	}

	free_instance(&inst);
	system("PAUSE");
	return 0;
}

void read_input(instance *inst) // simplified CVRP parser, not all SECTIONs detected  
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

void parse_command_line(int argc, char** argv, instance *inst)
{

	if (VERBOSE >= 100) printf(" running %s with %d parameters \n", argv[0], argc - 1);

	// default   
	inst->model_type = 0;
	inst->old_benders = 0;
	strcpy(inst->input_file, "NULL");	//c fa così le assegnazioni alle stringhe
	inst->randomseed = 0;
	inst->num_threads = 0;
	inst->timelimit = CPX_INFBOUND;		//valore importato da vrp.h che lo ha importanto da cmplx.h
	inst->cutoff = CPX_INFBOUND;
	inst->integer_costs = 0;

	inst->available_memory = 12000;   			// available memory, in MB, for Cplex execution (e.g., 12000)
	inst->max_nodes = -1; 						// max n. of branching nodes in the final run (-1 unlimited)        

	int help = 0; if (argc < 1) help = 1;
	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-file") == 0) { strcpy(inst->input_file, argv[++i]); continue; } 			// input file
		if (strcmp(argv[i], "-time_limit") == 0) { inst->timelimit = atof(argv[++i]); continue; }		// total time limit, atof converte stringa a float
		/*if (strcmp(argv[i], "-input") == 0) { strcpy(inst->input_file, argv[++i]); continue; } 			// input file
		if (strcmp(argv[i], "-f") == 0) { strcpy(inst->input_file, argv[++i]); continue; } 				// input file		
		if (strcmp(argv[i], "-model_type") == 0) { inst->model_type = atoi(argv[++i]); continue; } 	// model type, atoi converte stringa a int
		if (strcmp(argv[i], "-old_benders") == 0) { inst->old_benders = atoi(argv[++i]); continue; } 	// old benders
		if (strcmp(argv[i], "-model") == 0) { inst->model_type = atoi(argv[++i]); continue; } 			// model type
		if (strcmp(argv[i], "-seed") == 0) { inst->randomseed = abs(atoi(argv[++i])); continue; } 		// random seed
		if (strcmp(argv[i], "-threads") == 0) { inst->num_threads = atoi(argv[++i]); continue; } 		// n. threads
		if (strcmp(argv[i], "-memory") == 0) { inst->available_memory = atoi(argv[++i]); continue; }	// available memory (in MB)
		if (strcmp(argv[i], "-node_file") == 0) { strcpy(inst->node_file, argv[++i]); continue; }		// cplex's node file
		if (strcmp(argv[i], "-max_nodes") == 0) { inst->max_nodes = atoi(argv[++i]); continue; } 		// max n. of nodes
		if (strcmp(argv[i], "-cutoff") == 0) { inst->cutoff = atof(argv[++i]); continue; }				// master cutoff
		if (strcmp(argv[i], "-int") == 0) { inst->integer_costs = 1; continue; } 						// inteher costs
		if (strcmp(argv[i], "-help") == 0) { help = 1; continue; } 									// help
		if (strcmp(argv[i], "--help") == 0) { help = 1; continue; } 									// help
		*/
		help = 1;
	}

	if (help || (VERBOSE >= 10))		// print current parameters
	{
		printf("\n\navailable parameters (vers. 04-mar-2017) --------------------------------------------------\n");
		printf("-file %s\n", inst->input_file);
		printf("-time_limit %lf\n", inst->timelimit);
		/*printf("-model_type %d\n", inst->model_type);
		printf("-old_benders %d\n", inst->old_benders);
		printf("-seed %d\n", inst->randomseed);
		printf("-threads %d\n", inst->num_threads);
		printf("-max_nodes %d\n", inst->max_nodes);
		printf("-memory %d\n", inst->available_memory);
		printf("-int %d\n", inst->integer_costs);
		printf("-node_file %s\n", inst->node_file);
		printf("-cutoff %lf\n", inst->cutoff);
		printf("\nenter -help or --help for help\n");
		printf("----------------------------------------------------------------------------------------------\n\n");*/
	}

	if (help) exit(1);

}
#ifndef MAIN_H_							

#define MAIN_H_

#include "TSP.h"

void emptyLines(instance inst);
int numberOfNonemptyLines(const char *file);
void elabTime(clock_t begin, clock_t end);
void readInput(instance *inst);
void parseCommandLine(int argc, char** argv, instance *inst);

void emptyLines(instance inst) {

	if (VERBOSE >= 100) {

		int numEmptyLines = numberOfNonemptyLines(inst.inputFile);

		if (numEmptyLines == 0) {

			printf("File %s is empty", inst.inputFile); exit(1);

		}

		printf("%d non-empty lines has been read", numEmptyLines);
	}
}

int numberOfNonemptyLines(const char *file)  // warning: the last line NOT counted if it is does not terminate with \n (as it happens with some editors) 
{
	FILE *fin = fopen(file, "r");
	if (fin == NULL) return 0;
	char line[123456];
	int count = 0;
	while (fgets(line, sizeof(line), fin) != NULL) { printf(" len %4d\n", (int)strlen(line)); if (strlen(line) > 1) count++; }
	fclose(fin);
	return count;
}

void elabTime(clock_t begin, clock_t end) {

	if (VERBOSE >= 1) { printf("\nCOMPLETED IN %.3f SECONDS\n", (double)(end - begin) / CLOCKS_PER_SEC); }

}

#endif   /* MAIN_H_ */ 

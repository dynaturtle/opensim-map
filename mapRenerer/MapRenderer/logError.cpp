#include "logError.h"

void logError(char *logInfo)
{
	FILE *file;
	file = fopen("log.txt", "a");
	if (file != NULL)
	{
		fprintf(file, logInfo);
		fprintf(file, "\n");
	}
	fclose(file);
}
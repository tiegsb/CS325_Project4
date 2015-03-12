#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <unistd.h>

struct city
{
	int id;
	int x;
	int y;
};

struct city *list;
int **adjMatrix;

void alarmHandler(int sig)
{
	printf ("Timer expired\n");

	//Print results

	free (adjMatrix);
	free (list);
	exit (0);
}

void getArgs (char *argv[], int argc, int *cityCount, int *timerMinutes, char *filename)
{
	FILE *fp;
	int size = 10;
	char line[50];
	char *token;
	int c;

	//Get flags
	 opterr = 0;

	while ((c = getopt (argc, argv, "t:o:")) != -1)
	{
		switch (c)
		{
			case 't':
				*timerMinutes = (int) strtol (optarg, NULL, 10);
				break;
			case 'o':
				strcpy (filename, optarg);
				break;
			case '?':
				if (optopt == 'o' || optopt == 't')
					fprintf (stderr, "Option -%c requires an argument\n", optopt);
				else if (isprint (optopt))
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				else
					fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
				exit (1);
			default:
				exit (1);
		}
	}

	if (optind >= argc)
	{
		fprintf (stderr, "usage: %s [input file] [-o output file] [-t timer minutes]\n", argv[0]);
		fprintf (stderr, "     -o: Set output filename. Unset defaults to stdout.\n");
		fprintf (stderr, "     -t: Set timer in minutes.\n");
		exit (1);
	}

	*cityCount = 0;

	if (allocateMemory (*cityCount, size) == -1)
	{
		fprintf (stderr, "Memory allocation failed\n");
		exit (1);
	}

	fp = fopen (argv[optind], "r");

	//Check to see if file opened successfully
	if (fp == NULL)
	{
		fprintf (stderr, "File failed to open\n");
		free (list);
		exit (1);
	}

	//Get data from file
	int d = 0;
	while (fgets (line, sizeof (line), fp) != NULL)
	{
		if (strcmp (line, "\n") == 0)
			break;

		(*cityCount)++;

		if (*cityCount == size)
		{
			size *= 2;

			if (allocateMemory (*cityCount, size) == -1)
			{
				fprintf (stderr, "Memory allocation failed\n");
				free (list);
				fclose (fp);
				exit (1);
			}			
		}

		token = strtok (line, " \n");
		list[*cityCount - 1].id = (int) strtol (token, NULL, 10);

		token = strtok (NULL, " \n");
		list[*cityCount - 1].x = (int) strtol (token, NULL, 10);

		token = strtok (NULL, " \n");
		list[*cityCount - 1].y = (int) strtol (token, NULL, 10);
	}
	fclose (fp);

	return;
}

int allocateMemory (int cityCount, int newSize)
{
	int i;
	struct city *newList;

	//Allocate memory
	newList = malloc (newSize * sizeof (struct city));

	if (newList == NULL)
		return -1;

	//Copy list info
	for (i = 0; i < cityCount; i++)
	{
		newList[i] = list[i];
	}

	free (list);

	list = newList;

	return 0;
}

void getAdjMatrix (int cityCount)
{
	int i, j;
	double dist;
	long int result;
	int xdiff;
	int ydiff;

	adjMatrix = malloc(cityCount * sizeof (int*));
	if (adjMatrix == NULL)
	{
		fprintf (stderr, "Memory allocation failed\n");
		free (list);
		exit (1);
	}

	for (i = 0; i < cityCount; i++)
	{
		adjMatrix[i] = malloc (cityCount * sizeof (int*));
		if (adjMatrix[i] == NULL)
		{
			fprintf (stderr, "Memory allocation failed%d\n",i);
			free (list);
			exit (1);
		}
	}

	//Build adjacency matrix
	for (i = 0; i < cityCount; i++)
	{
		for (j = 0; j < cityCount; j++)
		{
			xdiff = list[i].x - list[j].x;
			ydiff = list[i].y - list[j].y;
			result = (xdiff * xdiff) + (ydiff * ydiff);

			dist = sqrt ((double)result);

			if (dist - (int)dist >= 0.5)
				dist++;

			adjMatrix[i][j] = (int)dist;
		}
	}
}

int main (int argc, char *argv[])
{
	int cityCount;
	int i;
	int timerMinutes = 0;
	char filename[255];

	signal (SIGALRM, alarmHandler);

	getArgs (argv, argc, &cityCount, &timerMinutes, filename);

	//Starting timer if set
	alarm (timerMinutes * 60);

	getAdjMatrix (cityCount);






	free (adjMatrix);
	free (list);

	exit (0);
}

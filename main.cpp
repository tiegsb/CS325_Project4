#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <csignal>
#include <unistd.h>
#include <vector>
#include <queue>

struct city
{
	int id;
	int x;
	int y;
};

struct edge
{
	int u;
	int v;
	int dist;

	bool operator < (const edge &rhs) const
	{
	    return dist > rhs.dist;
	}
};

std::vector <struct city> list;
std::priority_queue <struct edge> edgeList;
//std::vector <std::vector <struct edge>> edgeList;

void alarmHandler(int sig)
{
	printf ("Timer expired\n");

	//Print results


	exit (0);
}

void getArgs (char *argv[], int argc, int *cityCount, int *timerMinutes, char *filename)
{
	FILE *fp;
	char line[50];
	char *token;
	int c;
	struct city temp;

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

	fp = fopen (argv[optind], "r");

	//Check to see if file opened successfully
	if (fp == NULL)
	{
		fprintf (stderr, "File failed to open\n");
		exit (1);
	}

	//Get data from file
	int d = 0;
	while (fgets (line, sizeof (line), fp) != NULL)
	{
		if (strcmp (line, "\n") == 0)
			break;

		(*cityCount)++;

		token = strtok (line, " \n");
		temp.id = (int) strtol (token, NULL, 10);

		token = strtok (NULL, " \n");
		temp.x = (int) strtol (token, NULL, 10);

		token = strtok (NULL, " \n");
		temp.y = (int) strtol (token, NULL, 10);

		list.push_back (temp);
	}

	fclose (fp);

	return;
}

void getAdjMatrix (int cityCount)
{
	double dist;
	long int result;
	int xdiff;
	int ydiff;
	struct edge temp;

	//Build priority queue
	for (int i = 0; i < cityCount; i++)
	{
		for (int j = i + 1; j < cityCount; j++)
		{
			xdiff = list[i].x - list[j].x;
			ydiff = list[i].y - list[j].y;
			result = (xdiff * xdiff) + (ydiff * ydiff);

			dist = sqrt ((double)result);

			if (dist - (int)dist >= 0.5)
				dist++;

			temp.u = list[i].id;
			temp.v = list[j].id;
			temp.dist = (int)dist;

			edgeList.push (temp);
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






	exit (0);
}

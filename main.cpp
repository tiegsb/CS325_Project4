#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
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

struct vertex
{
	int u;
	int v;
	int uDist;
	int vDist;
	bool uSet;
	bool vSet;

	vertex()
	{
		uSet = false;
		vSet = false;
	}
};

std::vector <struct city> list;
std::priority_queue <struct edge> edgeList;
std::vector <int> result;
char filename[255];

void getArgs (char *argv[], int argc, int *cityCount, int *timerMinutes)
{
	FILE *fp;
	char line[50];
	char *token;
	int c;
	struct city temp;

	//Get flags
	 opterr = 0;

	while ((c = getopt (argc, argv, "t:")) != -1)
	{
		switch (c)
		{
			case 't':
				*timerMinutes = (int) strtol (optarg, NULL, 10);
				break;
			case '?':
				if (optopt == 't')
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
		fprintf (stderr, "usage: %s [input file] [-t timer minutes]\n", argv[0]);
		fprintf (stderr, "     -t: Set timer in minutes.\n");
		exit (1);
	}

	*cityCount = 0;

	strcpy(filename, argv[optind]);
	fp = fopen (filename, "r");

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

void greedy(int cityCount)
{
	std::vector <struct vertex> greedyList(cityCount);
	int numberSet = 0;
	int start, prev, next, u, v, total;
	bool loop;

	while (edgeList.size() > 0)
	{
		u = edgeList.top().u;
		v = edgeList.top().v;

		if (!(greedyList[u].vSet || greedyList[v].vSet))
		{
			if (!(greedyList[u].uSet && greedyList[v].uSet))
			{
				numberSet++;
			
				if (!greedyList[u].uSet)
				{
					greedyList[u].uSet = true;
					greedyList[u].u = edgeList.top().v;				
					greedyList[u].uDist = edgeList.top().dist;
				}
				else
				{
					greedyList[u].vSet = true;
					greedyList[u].v = edgeList.top().v;
					greedyList[u].vDist = edgeList.top().dist;
				}

				if (!greedyList[v].uSet)
				{
					greedyList[v].uSet = true;
					greedyList[v].u = edgeList.top().u;
					greedyList[v].uDist = edgeList.top().dist;
				}
				else
				{
					greedyList[v].vSet = true;
					greedyList[v].v = edgeList.top().u;
					greedyList[v].vDist = edgeList.top().dist;
				}
			}
			else
			{
				//Last edge
				if (cityCount == numberSet + 1)
				{
					greedyList[u].vSet = true;
					greedyList[u].v = edgeList.top().v;
					greedyList[u].vDist = edgeList.top().dist;

					greedyList[v].vSet = true;
					greedyList[v].v = edgeList.top().u;
					greedyList[v].vDist = edgeList.top().dist;

					break;
				}
				//Check for loop
				else
				{
					start = u;
					prev = u;
					next = greedyList[u].u;
					loop = true;

					while (next != start && loop)
					{
						if (!(greedyList[next].uSet && greedyList[next].vSet))
						{
							if(next == v)
								break;

							loop = false;
						}
						else
						{
							if (prev == greedyList[next].u)
							{
								prev = next;
								next = greedyList[next].v;
							}
							else
							{
								prev = next;
								next = greedyList[next].u;
							}
						}
					}

					if(!loop)
					{
						numberSet++;

						greedyList[u].vSet = true;
						greedyList[u].v = edgeList.top().v;
						greedyList[u].vDist = edgeList.top().dist;

						greedyList[v].vSet = true;
						greedyList[v].v = edgeList.top().u;
						greedyList[v].vDist = edgeList.top().dist;
					}
				}
			}
		}
			
		edgeList.pop();
	}

	//Trace path
	total = 0;

	result.push_back(0);
	result.push_back(0);

	total = greedyList[0].vDist;
	prev = 0;
	next = greedyList[0].v;

	while (next != 0)
	{
		result.push_back(next);
		if (prev == greedyList[next].u)
		{
			prev = next;
			total += greedyList[next].vDist;
			next = greedyList[next].v;
		}
		else
		{
			prev = next;
			total += greedyList[next].uDist;
			next = greedyList[next].u;
		}
	}

	result[0] = total;
}

void outputResults()
{
	FILE *fp;

	strcat(filename, ".tour");

	fp = fopen (filename, "w");

	//Check to see if file opened successfully
	if (fp == NULL)
	{
		fprintf (stderr, "File failed to open\n");

		//Print results to stdout
		for (int i = 0; i < result.size(); i++)
		{
			printf ("%d\n", result[i]);
		}

		exit (1);
	}

	for (int i = 0; i < result.size(); i++)
	{
		fprintf (fp, "%d\n", result[i]);
	}

	fclose (fp);
}

void alarmHandler(int sig)
{
	printf ("Timer expired\n");

	outputResults();

	exit (0);
}

int main (int argc, char *argv[])
{
	int cityCount;
	int i;
	int timerMinutes = 0;

	signal (SIGALRM, alarmHandler);

	getArgs (argv, argc, &cityCount, &timerMinutes);

	//Starting timer if set
	alarm (timerMinutes * 60);

	getAdjMatrix (cityCount);

	greedy(cityCount);

	//Stop alarm
	alarm (0);

	outputResults();

	exit (0);
}

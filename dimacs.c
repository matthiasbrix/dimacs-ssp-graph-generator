#include <stdlib.h>
#include <stdio.h>
#include "sodium.h"

#define MIN_ARGS 5

void pp_matrix (int n, int **weight_matrix)
{
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			printf("%6d", weight_matrix[i][j]);
		}
		printf ("\n");
	}

	return;
}

int random_number(int min_num, int max_num)
{
	int result = 0, low_num = 0, hi_num = 0;

	if (min_num < max_num)
	{
		low_num = min_num;
		hi_num = max_num + 1; // include max_num in output
	} else {
		low_num = max_num + 1; // include max_num in output
		hi_num = min_num;
	}

	result = (randombytes_uniform (RAND_MAX) % (hi_num - low_num)) + low_num;
	return result;
}

int produce_spanning_tree (int n, int wmax, int wmin, int **weight_matrix, int *num_connections)
{
	int connected[n];

	for (int i = 0; i < n; i++) {
		connected[i] = -1;
		for (int j = 0; j < n; j++)
			weight_matrix[i][j] = -1;
	}

	for (int i = 0; i < n; i++) {
		int u = i;
		int v = randombytes_uniform (n);
		if (u == 0) {
			connected[u] = u;
			int weight = random_number(wmin, wmax);
			weight_matrix[u][v] = weight;
			weight_matrix[v][u] = weight;
			num_connections[u] += 1;
			num_connections[v] += 1;
			continue;
		}
		// check if u is not already connected
		while (connected[u] != -1) {
			u = randombytes_uniform (n);
		}
		// check if v is not u or v is connected, otherwise shuffle
		while (u == v || connected[v] == -1) {
			v = randombytes_uniform (n);
		}
		connected[u] = u;
		int weight = random_number(wmin, wmax);
		weight_matrix[u][v] = weight;
		weight_matrix[v][u] = weight;
		num_connections[u] += 1;
		num_connections[v] += 1;
	}
	return 0;
}

int add_edges_to_nodes (int n, int mleft, int wmin, int wmax, int **weight_matrix, int *num_connections)
{
	int num_edges_for_each_node = mleft/n;
	int rest_edge = mleft%n;
	int u, v, weight;

	for (int i = 0; i < n; i++) {
		u = i;
		if (num_connections[u] == (n-1)) {
			continue;
		}
		for (int j = 0; j < num_edges_for_each_node; j++) {
			v = randombytes_uniform (n);
			do {
				v = randombytes_uniform (n);
				/* printf ("%d %d\n", weight_matrix[u][v], v); */
				/* printf ("%d %d %d %d %d\n", u, v, (u == v), (weight_matrix[u][v] > -1), (num_connections[v] == (n-1))); */
			} while (u == v || (weight_matrix[u][v] > -1)); //|| weight_matrix[u][v] > -1 || num_connections[v] == (n-1));
			weight = random_number (wmin, wmax);
			weight_matrix[u][v] = weight;
			weight_matrix[v][u] = weight;
			num_connections[u] += 1;
			num_connections[v] += 1;
		}
	}
	pp_matrix (n, weight_matrix);
	printf ("test\n");
	/* Assigning some random nodes the left number of edges
	   connecting to some random nodes */
	for (int i = 0; i < rest_edge; i++) {
	/* 	int u = randombytes_uniform (n); */
	/* 	int v = randombytes_uniform (n); */
	/* 	while ((num_connections[u] == (n-1) || (num_connections[v] == (n-1))) && (u == v || weight_matrix[u][v] > -1)) { */
	/* 		printf ("tes %d %d\n", (num_connections[u] == (n-1)), (num_connections[v] == (n-1))); */
	/* 		u = randombytes_uniform (n); */
	/* 		v = randombytes_uniform (n); */
	/* 	} */
	/* 	int weight = random_number(wmin, wmax); */
	/* 	weight_matrix[u][v] = weight; */
	/* 	weight_matrix[v][u] = weight; */
	/* 	num_connections[u] += 1; */
	/* 	num_connections[v] += 1; */
	}
	return 0;
}

int main (int argc, char *argv[])
{
	if (argc != MIN_ARGS) {
		printf ("Obey the required number of arguments!!\n");
	}

	int n = atof(argv[1]);
	double density = atof(argv[2]);
	int wmin = atoi(argv[3]);
	int wmax = atoi(argv[4]);

	if ((n <= 0 || density <= 0.0 || wmin <= 0 || wmax <= 0) &&
		wmax >= wmin) {
		printf ("How to run: ./dimacs 100 0.1 100 1000\n");
	}

	int **weight_matrix = malloc (n * sizeof(int*));
	// checking how many connections of each node
	int *num_connections = calloc (n, sizeof(int));
	for (int i = 0; i < n; i++)
		weight_matrix[i] = malloc (n * sizeof(int));

	produce_spanning_tree (n, wmin, wmax, weight_matrix, num_connections);
	printf ("after spanning tree\n");
	pp_matrix (n, weight_matrix);
	// Directed graph!
	// D = m / ((n)(n-1))
	// m = (D*n*(n-1)) (undirected: / 2)
	int m = (density*n*(n-1));
	// after spanning tree is produced, we calc. remaining edges
	int mleft = m - (n-1);
	double avg_edges = (double)mleft/(double)n;

	printf ("her: %d %.2f %d %d %d %.2f\n", n, density, wmin, wmax, mleft, avg_edges);
	add_edges_to_nodes (n, mleft, wmin, wmax, weight_matrix, num_connections);
	printf ("After adding edges\n");
	pp_matrix (n, weight_matrix);

	return EXIT_SUCCESS;
}

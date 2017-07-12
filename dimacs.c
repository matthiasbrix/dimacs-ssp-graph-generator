
#include "dimacs.h"

#define MIN_ARGS 6
// #define DEBUG

void pp_matrix (int n, int **weight_matrix)
{
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			printf("%6d", weight_matrix[i][j]);
		}
		printf ("\n");
	}
	printf ("\n");
	return;
}

void write_to_file (const char *fname, int **weight_matrix, int n, int m)
{
	FILE *fp = fopen (fname, "a+");
	if (fp == NULL)
		exit (EXIT_FAILURE);

	char p[64];
	sprintf (p, "p sp %d %d", n, (m*2));
	fprintf (fp, "%s\n%s\n", p, "c");
	for (int i = 0; i < n; i++) {
		for (int j = (i+1); j < n; j++) {
			if (weight_matrix[i][j] != -1) {
				char a[64], b[64];
				sprintf (a, "a %d %d %d", i+1, j+1, weight_matrix[i][j]);
				sprintf (b, "a %d %d %d", j+1, i+1, weight_matrix[i][j]);
				fprintf (fp, "%s\n", a);
				fprintf (fp, "%s\n", b);
			}
		}
	}
}

// Generates number i such that min_num <= i <= max_num
// i.e. min and max are inclusive
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
	int connected[n], v, u;

	for (int i = 0; i < n; i++) {
		connected[i] = -1;
		for (int j = 0; j < n; j++)
			weight_matrix[i][j] = -1;
	}

	for (int i = 0; i < n; i++) {
		u = i;
		// if already connected to ST, chose next node
		if (connected[u] > -1)
			continue;

		v = randombytes_uniform (n);
		if (u == 0) {
			while (u == v) {
				v = randombytes_uniform (n);
			}
			int weight = random_number(wmin, wmax);
			weight_matrix[u][v] = weight;
			weight_matrix[v][u] = weight;
			num_connections[u] -= 1;
			num_connections[v] -= 1;
			connected[u] = u;
			connected[v] = v;
			continue;
		}
		v = randombytes_uniform (n);
		// check if v is not u or v is connected, otherwise shuffle
		while ((u == v) || (connected[v] == -1) || ((weight_matrix[u][v] > -1))) {
			v = randombytes_uniform (n);
		}
		connected[u] = u;
		int weight = random_number(wmin, wmax);
		weight_matrix[u][v] = weight;
		weight_matrix[v][u] = weight;
		num_connections[u] -= 1;
		num_connections[v] -= 1;
	}

	return 0;
}

int add_edges_to_nodes (int n, int mleft, int wmin, int wmax, int **weight_matrix, int *num_connections)
{
	int num_edges_for_each_node = mleft/n;
	int remaining_edges = mleft%n;
	int u, v, weight;

	for (int i = 0; i < n; i++) {
		if (mleft <= 0) {
			break;
		}
		u = i;
		for (int j = 0; j < num_edges_for_each_node; j++) {
			// If a row is full, e.g. af pst, all its edges is giving to the remaining pool
			if (!num_connections[u]) {
				remaining_edges += num_edges_for_each_node - j;
				break;
			}
			do {
				v = randombytes_uniform (n);
				// Check the node v still has enough entries left after connection, assuming v comes after u
				if (v > u && num_connections[v] <= num_edges_for_each_node) {
					continue;
				}
			} while ((u == v) || (!num_connections[v]) || (weight_matrix[u][v] > -1));
			weight = random_number (wmin, wmax);
			weight_matrix[u][v] = weight;
			weight_matrix[v][u] = weight;
			num_connections[u] -= 1;
			num_connections[v] -= 1;
			mleft -= 1;
		}
	}
	/* Assigning some random nodes the left number of edges
	   connecting to some random nodes */
	for (int i = 0; i < remaining_edges; i++) {
		int u = randombytes_uniform (n);
		int v = randombytes_uniform (n);
		while ((!num_connections[u] || !num_connections[v] || u == v || weight_matrix[u][v] > -1)) {
			u = randombytes_uniform (n);
			v = randombytes_uniform (n);
		}
		int weight = random_number(wmin, wmax);
		weight_matrix[u][v] = weight;
		weight_matrix[v][u] = weight;
		num_connections[u] -= 1;
		num_connections[v] -= 1;
	}
	return 0;
}

// https://openpatent.blogspot.dk/2013/04/xincrol-unique-and-random-number.html
// Use this approach: https://stackoverflow.com/questions/1608181/unique-random-numbers-in-an-integer-array-in-the-c-programming-language/1608585#1608585
// https://stackoverflow.com/questions/196017/unique-non-repeating-random-numbers-in-o1
int main (int argc, char *argv[])
{
	// TODO: Implement these
	/* int *harray[n]; */
	/* int counter = 0; */
	/* for (int j = 0; j < n; j++) { */
	/* 	if (weight_matrix[i][j] != -1 || i==j) // already connected */
	/* 		continue; */
	/* 	harray[counter] = malloc (sizeof(int)); */
	/* 	harray[counter] = &(weight_matrix[i][j]); */
	/* 	counter++; */
	/* } */
	/* int idx = randombytes_uniform(counter); */
	/* idx = *harray[idx]; */
	/* weight_matrix[i][idx] = weight; */
	/* weight_matrix[idx][i] = weight; */
	/* int tmp = harray[counter-1]; */
	/* harray[counter-1] = harray[idx]; */
	/* harray[idx] = tmp; */
	/* counter--; */

	if (argc != MIN_ARGS) {
		printf ("Obey the required number of arguments!!\n");
	}

	int n = atof(argv[1]);
	double density = atof(argv[2]);
	int wmin = atoi(argv[3]);
	int wmax = atoi(argv[4]);
	const char *fname = argv[5];

	if ((n <= 0 || density <= 0.0 || wmin <= 0 || wmax <= 0) &&
		wmax >= wmin) {
		printf ("How to run: ./dimacs 100 0.1 100 1000\n");
	}

	int **weight_matrix = malloc (n * sizeof(int*));
	// checking how many connections of each node
	int *num_connections = malloc (n * sizeof(int));
	for (int i = 0; i < n; i++) {
		weight_matrix[i] = malloc (n * sizeof(int));
		num_connections[i] = (n-1);
	}
	printf ("1\n");
	produce_spanning_tree (n, wmin, wmax, weight_matrix, num_connections);
	printf ("2\n");
	// D = m / ((n)(n-1))
	// m = (D*n*(n-1)) (undirected - m / 2)
	int m = (density*n*(n-1)) / 2;
	// after spanning tree is produced, we calc. remaining edges
	int mleft = m - (n-1);

	#ifdef DEBUG
	double avg_edges = (double)mleft/(double)n;
	printf ("after spanning tree\n");
	pp_matrix (n, weight_matrix);
	printf ("parameters: n=%d d=%.2f wmin=%d wmax=%d mleft=%d avg_edge_pr_node=%.2f\n", n, density, wmin, wmax, mleft, avg_edges);
	#endif

	printf ("3\n");
	add_edges_to_nodes (n, mleft, wmin, wmax, weight_matrix, num_connections);
	printf ("4\n");

	#ifdef DEBUG
	printf ("After adding edges\n");
	pp_matrix (n, weight_matrix);
	#endif

	write_to_file (fname, weight_matrix, n, m);

	return EXIT_SUCCESS;
}

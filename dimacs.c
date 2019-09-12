#include "dimacs.h"

/*
  Authors (and copyrigt):
  Matthias Brix
*/

void pp_matrix (int n, struct entry **matrix)
{
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			printf("%6d", matrix[i][j].weight);
		}
		printf ("\n");
	}
	printf ("\n");
	return;
}

void write_to_file (const char *fname, struct entry **weight_matrix, int n, int m)
{
	FILE *fp = fopen (fname, "a+");
	if (fp == NULL)
		exit (EXIT_FAILURE);

	char p[64];
	sprintf (p, "p sp %d %d", n, (m*2));
	fprintf (fp, "%s\n%s\n", p, "c");
	for (int i = 0; i < n; i++) {
		for (int j = (i+1); j < n; j++) {
			if (weight_matrix[i][j].weight != -1) {
				char a[64], b[64];
				sprintf (a, "a %d %d %d", i+1, j+1, weight_matrix[i][j].weight);
				sprintf (b, "a %d %d %d", j+1, i+1, weight_matrix[i][j].weight);
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

struct available_entries *find_available_entries (enum mode mode, int n, int u,
	struct entry **weight_matrix, int *connected,
	int *num_connections,int num_edges_for_each_node)
{
	struct available_entries *st = malloc (sizeof (struct available_entries));
	struct entry **harray = malloc (n * sizeof(struct entry*));
	st->counter = 0;

	// connect node u, that is not connected, to a node that is connected
	for (int v = 0; v < n; v++) {

		if (mode == pst) {
			if ((u != 0) && (weight_matrix[u][v].weight != -1 || connected[v] == -1 || u==v)) // already connected
				continue;
			else if (weight_matrix[u][v].weight != -1 || u == v)
				continue;
		} else if (mode == add_edges) {
			if (u==v || (!num_connections[v]) || weight_matrix[u][v].weight > -1)
				continue;
			// Check the node v still has enough entries left after connection, assuming v comes after u
			else if (v > u && num_connections[v] <= num_edges_for_each_node)
				continue;
		} else if (mode == rem_edges) {
			if (!num_connections[v] || u == v || weight_matrix[u][v].weight > -1)
				continue;
		}

		harray[st->counter] = malloc (sizeof(struct entry));
		harray[st->counter] = &weight_matrix[u][v];
		st->counter++;
	}

	st->harray = malloc (st->counter * sizeof(struct entry*));

	for (int i = 0; i < st->counter; i++) {
		memcpy (&st->harray[i], &harray[i], sizeof(struct entry*));
	}

	FREE (harray);

	return st;
}

int produce_spanning_tree (int n, int wmax, int wmin, int *num_connections, struct entry **weight_matrix)
{
	int *connected = malloc (n * sizeof (int));
	int v, u;

	for (int i = 0; i < n; i++) {
		connected[i] = -1;
		for (int j = 0; j < n; j++) {
			weight_matrix[i][j].weight = -1;
			weight_matrix[i][j].x_index = i;
			weight_matrix[i][j].y_index = j;
		}
	}

	for (int i = 0; i < n; i++) {
		u = i;
		// if already connected to ST, chose next node
		if (connected[u] > -1) {
			continue;
		}

		struct available_entries *st = find_available_entries (pst, n, u, weight_matrix, connected, NULL, -1);
		v = randombytes_uniform (st->counter);

		if (u == 0) {
			int weight = random_number(wmin, wmax);
			st->harray[v]->weight = weight;
			weight_matrix[st->harray[v]->y_index][st->harray[v]->x_index].weight = weight;
			num_connections[u] -= 1;
			num_connections[st->harray[v]->y_index] -= 1;
			connected[u] = u;
			connected[st->harray[v]->y_index] = v;
			FREE (st);
			continue;
		}
		int weight = random_number(wmin, wmax);
		st->harray[v]->weight = weight;
		weight_matrix[st->harray[v]->y_index][st->harray[v]->x_index].weight = weight;
		num_connections[u] -= 1;
		num_connections[st->harray[v]->y_index] -= 1;
		connected[u] = u;
		FREE (st);
	}

	return 0;
}

int add_edges_to_nodes (int n, int mleft, int wmin, int wmax,
						struct entry **weight_matrix, int *num_connections)
{
	int num_edges_for_each_node = mleft / n;
	int remaining_edges = mleft % n;
	int u, v, weight;

	for (int i = 0; i < n; i++) {
		if (mleft <= 0) {
			break;
		}
		u = i;

		struct available_entries *st =
			find_available_entries (add_edges, n, u,
									weight_matrix, NULL,
									num_connections,
									num_edges_for_each_node);

		for (int j = 0; j < num_edges_for_each_node && st->counter; j++) {
			// If a row is full, e.g. after pst, all its edges is giving to the remaining pool
			if (!num_connections[u]) {
				remaining_edges += num_edges_for_each_node - j;
				break;
			}
			v = randombytes_uniform (st->counter);
			weight = random_number (wmin, wmax);
			st->harray[v]->weight = weight;
			// u == st->harray[v]->x_index
			weight_matrix[st->harray[v]->y_index][st->harray[v]->x_index].weight = weight;
			num_connections[u] -= 1;
			num_connections[st->harray[v]->y_index] -= 1;
			// Erase element from harray to keep only available/free nodes
			struct entry *tmp = st->harray[st->counter-1];
			st->harray[st->counter-1] = st->harray[v];
			st->harray[v] = tmp;
			st->counter--;
			mleft -= 1;
		}
	}

	/* Assigning some random nodes the left number of edges
	   connecting to some random nodes */
	if (remaining_edges > 0) {
		int *available = malloc (n * sizeof(int));
		int counter = 0;
		for (int i = 0; i < n; i++) {
			// node has potential entries to connect with
			if (num_connections[i]) {
				available[counter] = i; // i == u
				counter += 1;
			}
		}
		for (int i = 0; i < remaining_edges; i++) {

			u = randombytes_uniform (counter);
			u = available[u];
			struct available_entries *st =
				find_available_entries (rem_edges, n, u, weight_matrix, NULL,
							num_connections, num_edges_for_each_node);
			v = randombytes_uniform (st->counter);
			weight = random_number (wmin, wmax);
			st->harray[v]->weight = weight;
			weight_matrix[st->harray[v]->y_index][u].weight = weight;
			num_connections[u] -= 1;
			num_connections[st->harray[v]->y_index] -= 1;
		}
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
	const char *fname = argv[5];
	struct entry **weight_matrix = malloc (n * sizeof (struct entry*));

	if ((n <= 0 || density <= 0.0 || wmin <= 0 || wmax <= 0) &&
		wmax >= wmin) {
		printf ("How to run: ./dimacs 100 0.1 100 1000\n");
	}

	// for checking how many connections of each node
	int *num_connections = malloc (n * sizeof(int));
	for (int i = 0; i < n; i++) {
		weight_matrix[i] = malloc (n * sizeof (struct entry));
		num_connections[i] = (n-1);
	}

	produce_spanning_tree (n, wmin, wmax, num_connections, weight_matrix);

	// D = m / ((n)(n-1))
	// m = (D*n*(n-1)) (undirected - m / 2)
	int m = (density*n*(n-1)) / 2;
	// after spanning tree is produced, we calc. remaining edges
	int mleft = m - (n-1);

	#ifdef DEBUG
	printf ("After spanning tree\n");
	pp_matrix (n, weight_matrix);
	printf ("parameters: n=%d d=%.2f wmin=%d wmax=%d mleft=%d avg_edge_pr_node=%.2f\n",
			n, density, wmin, wmax, mleft, ((double)mleft/(double)n));
	#endif

	add_edges_to_nodes (n, mleft, wmin, wmax, weight_matrix, num_connections);

	#ifdef DEBUG
	printf ("After adding edges\n");
	pp_matrix (n, weight_matrix);
	#endif

	write_to_file (fname, weight_matrix, n, m);

	return EXIT_SUCCESS;
}

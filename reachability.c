// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Code to calculate forward and backward reachability by Petter Holme 2020

#include "reachability.h"

GLOBALS g;
TIMESTEP *ts;
NODE *n;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// merging paths for all active nodes this time step

void merge_reachable (TIMESTEP *t) {
	unsigned int i, j, k, me, you, nr;

	for (i = 0; i < t->nactive; i++) {
		me = t->active[i];
		for (j = 0; j < g.nu; j++) n[me].rlast[j] = n[me].rnow[j]; // remembering the states before the merging
	}
	for (i = 0; i < t->nactive; i++) {
		me = t->active[i];
		for (j = 0; j < t->deg[i]; j++) {
			you = t->nb[i][j];
			for (k = 0; k < g.nu; k++) n[me].rnow[k] |= n[you].rlast[k]; // merging reachable nodes by bit-wise or
		}
		for (j = nr = 0; j < g.nu; j++) nr += g.ones[n[me].rlast[j]];
		n[me].reach += DIFF(n[me].tlast, t->time) * (double) nr; // weighing by the time since last update
		n[me].tlast = t->time; // remembering the time of this time step
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// main function handling i/o

int main (int argc, char *argv[]) {
	unsigned int i, j, nr;
	double d, *forward;

	if (argc != 2) {
		fprintf(stderr, "usage: reachability [fname]\n");
		exit(2);
	}

	// read network
	read_data(argv[1]);

	// creating look-up table for counting 1s in (16-bit) bit-fields
	for (i = 0; i < 0x10000; i++) g.ones[i] = (i & 1) + g.ones[i >> 1];

	// forward reachability
	for (i = 0; i < g.n; i++) {
		n[i].rnow[i >> 4] = 1 << (i & 0xf); // initialize the array of reachable nodes (from one node)
		n[i].tlast = g.dur;
	}
	for (i = g.nts - 1; i > 0; i--) merge_reachable(ts + i); // go through contacts backward in time
	for (i = 0; i < g.n; i++) {
		for (j = nr = 0; j < g.nu; j++) nr += g.ones[n[i].rnow[j]];
		n[i].reach += n[i].tlast * nr; // adding contributions between t = 0 and the first contact
	}

	forward = malloc(g.n * sizeof(double));

	// backward reachability
	for (i = 0; i < g.n; i++) {
		forward[i] = n[i].reach;
		for (j = 0; j < g.nu; j++) n[i].rnow[j] = n[i].rlast[j] = 0;
		n[i].rnow[i >> 4] = 1 << (i & 0xf); // initialize the array of reachable nodes (from one node)
		n[i].tlast = 0;
		n[i].reach = 0.0;
	}

	for (i = 0; i < g.nts; i++) merge_reachable(ts + i); // go through contacts forward in time
	for (i = 0; i < g.n; i++) {
		for (j = nr = 0; j < g.nu; j++) nr += g.ones[n[i].rnow[j]];
		n[i].reach += (g.dur - n[i].tlast) * nr; // adding contributions between t = 0 and the first contact
	}

	// printing
	d = (1.0 / g.dur) / g.n;
	for (i = 0; i < g.n; i++) printf("%u %g %g\n", i, forward[i] * d, n[i].reach * d);

	// free things
	for (i = 0; i < g.nts; i++) {
		for (j = 0; j < ts[i].nactive; j++) free(ts[i].nb[j]);
		free(ts[i].active);
		free(ts[i].deg);
		free(ts[i].nb);
	}
	for (i = 0; i < g.n; i++) {
		free(n[i].rnow);
		free(n[i].rlast);
	}
	free(ts); free(n); free(forward);
	 
	return 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

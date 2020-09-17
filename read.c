// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Code to calculate forward and backward reachability by Petter Holme 2020

#include "reachability.h"

extern GLOBALS g;
extern TIMESTEP *ts;
extern NODE *n;

unsigned int nactive, *active, *deg, *mark, time_last;
CONTACT *c;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void add_ts (unsigned int ilast, unsigned int inow) {
	unsigned int i, me, you, u, v;
	TIMESTEP *t;

	t = ts + g.nts;
	t->nactive = nactive;
	t->active = malloc(nactive * sizeof(unsigned int));
	t->deg = calloc(nactive, sizeof(unsigned int));
	t->nb = malloc(nactive * sizeof(unsigned int *));
	t->time = time_last;
			
	for (i = 0; i < nactive; i++) {
		t->active[i] = me = active[i];
		t->deg[i] = 0;
		t->nb[i] = malloc(deg[me] * sizeof(unsigned int));
	}

	for (i = ilast; i < inow; i++) {
		me = c[i].left;
		you = c[i].right;
		u = mark[me];
		v = mark[you];
		t->nb[u][t->deg[u]++] = you;
		t->nb[v][t->deg[v]++] = me;
	}

	for (i = 0; i < nactive; i++) {
		me = active[i];
		mark[active[i]] = NONE;
		deg[me] = 0;
	}

	g.nts++;
	nactive = 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// reading a contact list data file

void read_data (char *fname) {
	unsigned int i, me, you, i0 = 0, nc = 0, ncalloc = NC0;
	FILE *fp;

	fp = fopen(fname, "r");
	if (!fp) {
		fprintf(stderr, "cannot read %s\n", fname);
		exit(1);
	}

	c = malloc(ncalloc * sizeof(CONTACT));

	while (3 == fscanf(fp, "%u %u %u", &c[nc].left, &c[nc].right, &c[nc].time)) {
		nc++;
		if (ncalloc <= nc) {
			ncalloc *= 2;
			c = realloc(c, ncalloc * sizeof(CONTACT));
		}
	}

	fclose(fp);

	time_last = NONE;
	for (i = 0; i < nc; i++) {
		if (g.n < c[i].left) g.n = c[i].left;
		if (g.n < c[i].right) g.n = c[i].right;
		if (time_last != c[i].time) {
			g.nts++;
			time_last = c[i].time;
		}
	}
	g.dur = time_last;
	g.n++;
	g.nts++;
	g.nu = (g.n % 16) ? g.n / 16 + 1 : g.n / 16;

	n = calloc(g.n, sizeof(NODE));
	
	for (i = 0; i < g.n; i++) {
		n[i].rnow = calloc(g.nu, sizeof(uint32_t));
		n[i].rlast = calloc(g.nu, sizeof(uint32_t));
	}

	ts = calloc(g.nts, sizeof(TIMESTEP));

	active = malloc(g.n * sizeof(unsigned int));
	mark = malloc(g.n * sizeof(unsigned int));
	for (i = 0; i < g.n; i++) mark[i] = NONE;
	deg = calloc(g.n, sizeof(unsigned int));

	time_last = NONE;
	for (i = g.nts = nactive = 0; i < nc; i++) {
		if (time_last != c[i].time) {
			add_ts(i0, i);
			time_last = c[i].time;
			i0 = i;
 		}

		me = c[i].left;
		if (mark[me] == NONE) {
			active[nactive] = me;
			mark[me] = nactive++;
		}
		deg[me]++;

		you = c[i].right;
		if (mark[you] == NONE) {
			active[nactive] = you;
			mark[you] = nactive++;
		}
		deg[you]++;
	}
	add_ts(i0,nc);

	free(active); free(mark); free(deg); free(c);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


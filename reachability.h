// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Code to calculate forward and backward reachability by Petter Holme 2020

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

#define NC0 1000 // base guess of number of contacts
#define NONE UINT_MAX
#define DIFF(x,y) (((x) < (y)) ? ((y) - (x)) : ((x) - (y)))

// struct with various global variables
typedef struct GLOBALS {
	unsigned int n, nts, dur;
	unsigned int nu; // number of uint32 to represent the network
	unsigned short ones[0x10000]; // number of ones in the binary representation of the argument
} GLOBALS;

// struct encoding everything that happens during one time step
typedef struct TIMESTEP {
	unsigned int time;
	unsigned int nactive, *active;
	unsigned int *deg, **nb; // degrees & neighbors of active nodes during this timestep
} TIMESTEP;

typedef struct NODE { // node centric information
	uint16_t *rnow, *rlast;
	unsigned int tlast; // number of reachable nodes, number of reachable nodes at previous contact, time of last contact
	double reach; // reachabilities
} NODE;

typedef struct CONTACT { // only a helping struct for reading the data files
	unsigned int left, right, time;
} CONTACT;

extern void read_data (char *);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#ifndef _COMPILE_H_
#define _COMPILE_H_

#include "virtcpu.h"

#define MAX_LEN 200

/* EVANGELION 5.0: ASSEMBLY IS (NOT) KAWAII~~ */

typedef struct {
	char comd  [MAX_LEN];
	char arg[2][MAX_LEN];
} line;

typedef struct {
	char name[MAX_LEN];
	char valu[MAX_LEN];
} dfval;

typedef struct {
	line  *lns;
	dfval *lbs;
	int    nln;
	int    nlb;
} code;

int  coderead  (char *ifile, code *cd);
int  codecmpl  (char *ofile, code *cd, ECM *e); /* accepts code read by coderead() */

int  codeprint (char *ofile, code *cd);

void deleteCode(code *cd);

#endif

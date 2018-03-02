#ifndef _CPU_H_
#define _CPU_H_

#include "cpudefs.h"
#include "register.h"

/*
 * sregs[0] keeps the top of the stack
 * sregs[1] keeps the ponter to  stack start
 * sregs[2] keeps address calculated in [ ]
 * sregs[3] keeps execution position in program
 */

typedef struct {
	reg  *gregs;
	reg  *sregs;
	byte *mem;
	byte  flags;

	int   sNum;
	int   gNum;
	int   memsz;
} ECM;

int  getECfg (ECM *ecm, char *iflnm);
int  initECM (ECM *ecm, int  *gr,    int *sr,    int memsz);
int  loadExe (ECM *ecm, char *exename);
void printECM(ECM *ecm, int   width, int  start, int end);
int  memdump (ECM *ecm, char *oflnm, int start, int end);
int  run     (ECM *ecm);
void stopECM (ECM *ecm);
int  getASz  (cmd  c, int isscd);

#endif

#include <stdio.h>
#include <string.h>
#include "virtcpu.h"

int main(int argc, char **argv) {
	ECM   ecm;
	int   err      = 0, i;
	char  dcname[] = "ecm.cfg";
	char  dename[] = "a.out";
	char *confnm   = dcname;
	char *exefnm   = dename;

	for (i = 1; i < argc; ++i)
		if (!strcmp(argv[i], "-c"))
			confnm = argv[i++ + 1];
		else
			exefnm = argv[i];

	err |= getECfg(&ecm, confnm);

	if (!err && argc > 1) {
		err  = loadExe(&ecm, exefnm);
		err |= run    (&ecm);
		stopECM(&ecm);
	}

	if (err)
		fprintf(stderr, "Error %d occured!\n", err);

	return err;
}

#include <stdio.h>
#include <string.h>
#include "nyasm.h"

int main(int argc, char **argv) {
	ECM   e;
	int   err      = 0, i;
	char  dcname[] = "ecm.cfg";
	char  dename[] = "a.out";
	char *confnm   = dcname;
	char *exefnm   = dename;
	char *inpfnm;
	code  cd;

	if (argc > 1) {
		inpfnm = argv[1];

		for (i = 2; i < argc; ++i)
			if (!strcmp(argv[i], "-c"))
				confnm = argv[i++ + 1];
			else
				exefnm = argv[i];
	}

	err = coderead(inpfnm, &cd);

	if (!err) {
		err = getECfg   (&e,   confnm);
		codeprint (NULL, &cd);
		err = codecmpl  (exefnm,  &cd, &e);
		deleteCode(&cd);
		stopECM   (&e);
	}
	else
		printf("Fail! Exit code: %d\n", err);

	return err;
}

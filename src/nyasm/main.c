#include <stdio.h>
#include "nyasm.h"

int main(int argc, char **argv) {
	char  defname[] = "a.out";
	char *inm;
	char *onm;
	int   err = 0;
	ECM   e;
	code  cd;

	if (argc > 1) {
		inm = argv[1];
		onm = argc > 2 ? argv[2] : defname;
	}

	err = coderead(inm, &cd);

	if (!err) {
		err = getECfg   (&e,   "ecm.cfg");
		codeprint (NULL, &cd);
		err = codecmpl  (onm,  &cd, &e);
		deleteCode(&cd);
		stopECM   (&e);
	}
	else
		printf("Fail! Exit code: %d\n", err);

	return err;
}

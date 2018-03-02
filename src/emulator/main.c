#include <stdio.h>
#include "virtcpu.h"

int main(int argc, char **argv) {
	ECM ecm;
	int err = 0;

	err |= getECfg(&ecm, "ecm.cfg");

	if (!err && argc > 1) {
		err  = loadExe(&ecm, argv[1]);
		err |= run    (&ecm);
		stopECM(&ecm);
	}

	if (err)
		fprintf(stderr, "Error %d occured!\n", err);

	return err;
}

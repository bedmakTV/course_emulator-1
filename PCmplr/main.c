#include "Compiler.h"

int main(int argc, char **argv) {
    char *nameRFile, *nameWFile;
	int   err = 0;

	if (argc > 1) {
		nameRFile = argv[1];

		if (argc > 2)
			nameWFile = argv[2];
		else
			err = -3;
	}
	else
		err = -2;

    if (!err) {
		FILE *input,     *output;

		output = fopen(nameWFile, "wb");
		input  = fopen(nameRFile, "r");

		err = compileFile(output, input);

		if (input)
			fclose(input);

		if (output)
			fclose(output);
	}

	printf("Compilation %s (exit code %d).\n", err ? "Failed" : "Successed", err);

    return err;
}

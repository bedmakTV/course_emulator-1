#include <stdio.h>
#include <stdint.h>
#include "../src/lib/cpudefs.h"

#define SIZE 100

int main(void) {
	FILE     *dst = fopen("exe.out", "wb");

	uint16_t  out[] = {
		MOV  | FST_ISRG | SCD_ISVL, 
		1, 8,
		MOV  | FST_ISRG | SCD_ISRG, 
		0, 1,
		ADD  | FST_ISRG | SCD_ISVL, 
		0, 5,
		PUSH | FST_ISVL,
		0x68,
		HLT
	};

	fwrite(out, sizeof(out) / CMD_SIZE, CMD_SIZE, dst);

	fclose(dst);

	return 0;
}

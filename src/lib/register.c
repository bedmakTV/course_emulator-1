#include <stdlib.h>
#include <string.h>
#include "register.h"

void initReg(reg *r, size_t size) {
	r->size  = size;
	r->bytes = (byte *)malloc(size);
	memset(r->bytes, 0, size);
}

void freeReg(reg *r) {
	free(r->bytes);
}

int32_t getInt32(byte *b, int n) {
	int res;

	memcpy(&res, b, n);

	return res;
}

int32_t setInt32(byte *b, int n, int32_t number) {
	memcpy(b, &number, n);

	return number;
}

int32_t regToInt(reg *r) {
	return getInt32(r->bytes, r->size);
}

int32_t intToReg(reg *r, int32_t number) {
	return setInt32(r->bytes, r->size, number);
}

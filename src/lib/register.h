#ifndef _REGISTER_H_
#define _REGISTER_H_

#include <stddef.h>
#include <stdint.h>
#include "cpudefs.h"

typedef struct {
	byte   *bytes;
	size_t  size;
} reg;

void    initReg (reg  *r, size_t  size);
void    freeReg (reg  *r);

int32_t regToInt(reg  *r);
int32_t intToReg(reg  *r, int32_t n);

/* "n" below is how many bytes to copy */
int32_t getInt32(byte *b, int n);
int32_t setInt32(byte *b, int n, int32_t number);

#endif

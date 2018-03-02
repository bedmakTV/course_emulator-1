#ifndef COMPILER_H
#define COMPILER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../emulator/cpudefs.h"

#define TRY_SET(place, val) ((place)? (*(place) = (val)) : 0)
#define TRY_GET(place) ((place)? *(place) : 0)

#define STR_LEN 256
#define WORD_LEN 32

#define SRC 0
#define DST 1

#define DCM 10
#define HEX 16

#define GNUM 8
#define SNUM 4

char compileFile(FILE* output, FILE* input);

#endif

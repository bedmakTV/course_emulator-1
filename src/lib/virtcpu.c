#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include "virtcpu.h"

#define MIN(x, y) ((x) < (y) ? (x) : (y))

/*
 * Inits the Electronic Computing Machine
 * gr : gr[0] is the in bytes size of general registers
 *      gr[1] is the number of them
 *
 * sr : same.
 */
int initECM(ECM *ecm, int *gr, int *sr, int memsz) {
	int i;

	ecm->mem   = (byte *)malloc(memsz);
	ecm->memsz = memsz;

	ecm->gNum  = gr[1];
	ecm->sNum  = sr[1];

	ecm->gregs = (reg *)malloc(ecm->gNum * sizeof(reg));

	for (i = 0; i < ecm->gNum; ++i)
		initReg(ecm->gregs + i, gr[0]);

	ecm->sregs = (reg *)malloc(ecm->sNum * sizeof(reg));

	for (i = 0; i < ecm->sNum; ++i)
		initReg(ecm->sregs + i, sr[0]);

	ecm->flags = 0;

	return 0;
}

/*
 * Reads config file and inits the ECM
 * format is:
 * size of gregs, number of gregs, size of sregs, number of sregs, memsize
 */
int getECfg(ECM *ecm , char *iflnm) {
	FILE *ifile = fopen(iflnm, "r");
	int   err = 0;

	if (ifile) {
		int gr[2], sr[2];
		int memsz;

		fscanf (ifile, "%d%d%d%d%d", gr, gr + 1, sr, sr + 1, &memsz);

		initECM(ecm, gr, sr, memsz);
		fclose(ifile);
	}
	else
		err = -1;

	return err;
}

/*
 * Opens file and reads it into the memory,
 * also sets the stack top pointer to last_byte_of_program + 1
 */
int loadExe(ECM *ecm, char *exename) {
	FILE *exe = fopen(exename, "rb");
	int   err = 0, i;

	if (exe) {
		for (i = 0; fread(ecm->mem + i, 1, 1, exe) > 0 && i < ecm->memsz; ++i)
			;

		intToReg(ecm->sregs,     i);
		intToReg(ecm->sregs + 1, i);

		fclose(exe);
	}
	else
		err = -1;

	fprintf(stderr, "%s %d\n", exename, err);

	return err;
}

/*
 * Output format:
 * Flags
 * General registers
 * Special registers
 * Memory with WIDTH columns of bytes
 *
 * Anything else you see in output is printed from some other function
 */
void printECM(ECM *ecm, int width, int start, int end) {
	int i;

	for (i = 1; i <= 128; i *= 2)
		printf("%d", (ecm->flags & i) != 0);

	printf("\n");

	for (i = 0; i < ecm->gNum; ++i)
		printf("%8.4X ", regToInt(ecm->gregs + i));

	printf("\n");

	for (i = 0; i < ecm->sNum; ++i)
		printf("%8.8X ", regToInt(ecm->sregs + i));

	printf("\n ");

	for (i = 0; i < width; ++i)
		printf("%2d ", i);
	printf("\n ");

	for (i = 0; i < width; ++i)
		printf("-- ");
	printf("\n");

	while (start < ecm->memsz && start < end) {
		int ist = start == regToInt(ecm->sregs);
		int isp = start == regToInt(ecm->sregs + 3) || start - 1 == regToInt(ecm->sregs + 3);
		int itt = 0; /* isatty(1); */

		if (ist)
			printf("%s", itt ? " \e[1;32m" : ">");
		else if (isp)
			printf("%s", itt ? " \e[1;31m" : ")");
		else
			printf(" ");

		printf("%2.2X", ecm->mem[start++]);

		if ((ist || isp) && itt)
			printf("\e[0m");

		if (!(start % width))
			printf("\n");
	}

	printf("\n");
}

/*
 * This function dumps memory only. Made for compiling the source code.
 * Probably can be used for debug, but printECM() will be more useful.
 */
int memdump(ECM *ecm, char *oflnm, int start, int end) {
	FILE *of  = fopen(oflnm, "wb");
	int   err = 0;

	if (of) {
		fwrite(ecm->mem + start, 1, MIN(end, ecm->memsz) - start, of);
		fclose(of);
	}
	else
		err = -1;

	return err;
}

/*
 * If it's ICA, we don't need to read argument at all.
 * If it's value & is set to be dword, we must read 4 bytes.
 * Everything else can be represented with only 2 bytes, so we'd better do it.
 */
int getASz(cmd c, int isscd) {
	int asz = 2;
	int ivl = c & (FST_ISVL >> isscd);
	int rsz = c &  ASZ_ISDW;
	int ica = c & (FST_ISCA >> isscd);

	if (ivl && rsz)
		asz += 2;

	if (ica)
		asz = 0;

	//printf("asz = %d\n", asz);

	return asz;
}

/*
 * One of the most important functions
 */
int getArg(ECM *ecm, reg *ctr, int pos, cmd c, int isscd) {
	int ivl = c & (FST_ISVL >> isscd);
	int rsz = c &  ASZ_ISDW;
	int irg = c & (FST_ISRG >> isscd);
	int ica = c & (FST_ISCA >> isscd);
	int asz = 2;
	int arg;

	//if (!(irg || ica || !(ivl && rsz) || ivl))
		//asz += 2;

	rsz = 2 + 2 * (rsz != 0);

	asz = getASz(c, isscd);

	arg = getInt32(ecm->mem + pos, asz);

	//printf("asz : %d\n", asz);
	//printf("arg : %d\n", arg);
	//printf("%d%d%d\n", ivl != 0, irg != 0, ica != 0);
	//printf("\n");

	if (ivl) {
		ctr->size  = rsz;
		ctr->bytes = ecm->mem + pos;
	}
	else if (irg) {
		/* When registers are used, rsz & ASZ_ISDW flag are ignored */
		if (arg < ecm->gNum) {
			ctr->size  = ecm->gregs[arg].size;
			ctr->bytes = ecm->gregs[arg].bytes;
		}
		else if (arg < ecm->gNum + ecm->sNum) {
			ctr->size  = ecm->sregs[arg - ecm->gNum].size;
			ctr->bytes = ecm->sregs[arg - ecm->gNum].bytes;
		}
	}
	else if (ica) {
		//printf("ica : %d\n", ica);
		ctr->size  = rsz;
		ctr->bytes = ecm->mem + regToInt(ecm->sregs + 2);
	}
	else {
		ctr->size  = rsz;
		ctr->bytes = ecm->mem + arg;
	}

	return asz;
}

/*
 * Actually idk if this works, but it
 * should make 2 pseudo-registers the same size
 */
void resRegs(reg *r1, reg *r2) {
	if (r1->size == -1)
		r1->size = r2->size;
	else if (r2->size == -1)
		r2->size = r1->size;

	r1->size = MIN(r1->size, r2->size);
	r2->size = r1->size;
}

void add(reg *dst, int val) {
	intToReg(dst, regToInt(dst) + val);
}

void push(ECM *ecm, reg *tr1) {
	memcpy(ecm->mem  + regToInt(ecm->sregs), tr1->bytes, tr1->size);
	add   (ecm->sregs, tr1->size);
}

void pop(ECM *ecm, reg *tr1) {
	add     (ecm->sregs, -tr1->size);
	intToReg(
		tr1,
		getInt32(ecm->mem + regToInt(ecm->sregs), tr1->size)
	);
}

int run(ECM *ecm) {
	int err = 0;
	int as1 = 0;
	int as2 = 0;

	printECM(ecm, 10, 0, 200);

	while (!(ecm->flags & FLAG_HLT)) {
		reg *posrg = ecm->sregs + 3;                     /* just shorter name */
		int  pos   = regToInt(posrg);  /* legacy; shortens code but dangerous */
		cmd  bt    = getInt32(ecm->mem + pos, CMD_SIZE);
		reg  tr1, tr2;  /* the pointers inside will point to necessary memory */

		as1 = getArg(ecm, &tr1, pos + CMD_SIZE, bt, 0);
		ecm->flags |= FLAG_SFA;

		/* CMD_MASK is necessary because of flags */
		switch ((cmd)(bt & CMD_MASK)) {
			case NOP:
				/* Does nothing; this *probably* is useful; used in real asm */
				break;

			case HLT:
				ecm->flags |= FLAG_HLT;
				break;

			case MOV:
				ecm->flags |= FLAG_SSA;
				as2 = getArg(ecm, &tr2, pos + CMD_SIZE + as1, bt, 1);
				memcpy(tr1.bytes, tr2.bytes,  MIN(tr1.size, tr2.size));
				break;

			case PUSH:
				push(ecm, &tr1);
				break;

			case POP:
				pop(ecm, &tr1);
				break;

			case SUB:
			case ADD:
				ecm->flags |= FLAG_SSA;
				as2 = getArg(ecm, &tr2, pos + CMD_SIZE + as1, bt, 1);
				resRegs(&tr1, &tr2);
				add    (&tr1, regToInt(&tr2) * (1 - 2 * ((bt & CMD_MASK) == SUB)));
				break;

			case INC:
			case DEC:
				add   (&tr1, (1 - 2 * ((bt & CMD_MASK) == SUB)));
				break;

			case XOR:
			case AND:
			case OR:
				ecm->flags |= FLAG_SSA;
				as2 = getArg(ecm, &tr2, pos + CMD_SIZE + as1, bt, 1);
				resRegs(&tr1, &tr2);

				switch (bt & CMD_MASK) {
					case XOR:
						intToReg(&tr1, regToInt(&tr1) ^ regToInt(&tr2));
						break;

					case AND:
						intToReg(&tr1, regToInt(&tr1) & regToInt(&tr2));
						break;

					case OR:
						intToReg(&tr1, regToInt(&tr1) | regToInt(&tr2));
						break;

					default:
						fprintf(stderr, "Wtf?\n");
						break;
				}
				break;

			case JMP:
				/* Must NOT move after jumping */
				SET_FLAG(ecm->flags, FLAG_NSC);

				/*
				 * Might look unclear; here we put a new value to register
				 * responsible for executing position
				 */
				intToReg(posrg, regToInt(&tr1));
				break;

			case CMP:
				ecm->flags |= FLAG_SSA;
				as2 = getArg(ecm, &tr2, pos + CMD_SIZE + as1, bt, 1);
				SET_FLAG    (
					ecm->flags,
					FLAG_DIZ * !((regToInt(&tr1) - regToInt(&tr2)) != 0)
				);

				break;

			case JE:
			case JNE:
				if ( ((ecm->flags & FLAG_DIZ) && (bt & CMD_MASK) == JE) ||
					(!(ecm->flags & FLAG_DIZ) && (bt & CMD_MASK) == JNE)
				) {
					SET_FLAG(ecm->flags, FLAG_NSC);
					intToReg(posrg, regToInt(&tr1));
				}
				else
					SET_FLAG(ecm->flags, FLAG_SFA);

				break;

			case RET:
				//SET_FLAG(ecm->flags, FLAG_NSC);
				/*
				 * This workarund will make the position move cmd itself
				 * and then 4 bytes as we are going to saved position,
				 * which is CALL, and unless we move, we'll jump again
				 */
				as1 = 4;
				initReg (&tr1, sizeof(dword));
				pop     (ecm, posrg);
				break;

			case CALL:
				SET_FLAG(ecm->flags, FLAG_NSC);

				push    (ecm, posrg);
				intToReg(posrg, regToInt(&tr1));
				break;

			default:
				fprintf(stderr, "Impossibru command %x\n", bt);
				break;
		}

		if (!(ecm->flags & FLAG_NSC)) {
			add(posrg, CMD_SIZE);

			if (ecm->flags & FLAG_SFA)
				add(posrg, as1);

			if (ecm->flags & FLAG_SSA)
				add(posrg, as2);
		}
		//printf("%2d %d %d %d\n", pos, as1, as2, ecm->mem[pos]);

		printECM(ecm, 10, 0, 200);
		printf("\n");
		printf("\n");

		CLR_FLAG(ecm->flags, FLAG_SFA);
		CLR_FLAG(ecm->flags, FLAG_SSA);
		CLR_FLAG(ecm->flags, FLAG_NSC);
	}

	return err;
}

/* Correctly clears all memory allocated for ECM */
void stopECM(ECM *ecm) {
	int i;

	free(ecm->mem);

	for (i = 0; i < ecm->gNum; ++i)
		freeReg(ecm->gregs + i);
	free(ecm->gregs);

	for (i = 0; i < ecm->sNum; ++i)
		freeReg(ecm->sregs + i);
	free(ecm->sregs);
}

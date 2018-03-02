#ifndef _CPUDEFS_H_
#define _CPUDEFS_H_

#define byte     unsigned char
#define word     unsigned short
#define dword    unsigned int

/*
 * Eerything breaks if ((FST_ISRG >> 1) != SCD_ISRG) !!!
 * Same for FST_ISCA & SCD_ISCA
 */
#define FLAG_SFA 0x0001         /* Fisrts argument must be skipped            */
#define FLAG_SSA 0x0002         /* Second, too                                */
#define FLAG_NSC 0x0004         /* Do not skip command this time              */
#define FLAG_DIZ 0x0008         /* After CMP difference was zero              */
#define FLAG_HLT 0x0080         /* Emulator should be stopped                 */

#define CMD_SIZE 000002         /* Size of one command in bytes               */

#define FST_ISVL 0x8000
#define SCD_ISVL 0x4000
#define ASZ_ISDW 0x2000
#define FST_ISRG 0x1000
#define SCD_ISRG 0x0800
#define FST_ISCA 0x0400
#define SCD_ISCA 0x0200
#define CMD_MASK 0x01FF         /* Command must be `&` with this mask
                                 * or the above flags will spoil switch-case
                                 */

#define SET_FLAG(x, flag) ((x) |=  (flag))
#define CLR_FLAG(x, flag) ((x) &= ~(flag))

/*
 * Functions must be JMPs to the end of a func. (after RET!) so that
 * they'll be skipped;
 * Compiler must set flags like FST_ISVL with EVERY instruction,
 * (if neccesarry), for JMPs too.
 */

typedef enum {
	NOP = 0,                    /* Does nothing           */
	MOV,                        /* Copying                */
	PUSH,
	POP,
	ADD,
	SUB,
	DEC,                       /* Decrements given value  */
	INC,
	XOR,
	AND,
	OR,
	CMP,
	JMP,
	CALL,                      /* JMP with pushing adress of CALL to stack */
	RET,
	JE,
	JNE,
	HLT                        /* Stops emulation         */
} cmd;

#endif

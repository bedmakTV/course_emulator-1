#include "Compiler.h"

#define isStop(x) ((x) == ' ' || (x) == '\n' || (x) == ';' || (x) == '\0')

unsigned int getArg(unsigned short* mode, unsigned short* shortArg, unsigned int* intArg, const char* str, const char flag) {
    unsigned int res = 1;
    char tmp = 0;
    char ableHexFlag = 1;
    char dwordFlag = 0, ableDwordFlag = 1;
    char regFlag = 0, ableRegFlag = 1;
    char OBraketFlag = 0, ableOBraketFlag = 1;
    char CBraketFlag = 0, ableCBraketFlag = 0;
    unsigned int i;
    char base = DCM;

    TRY_SET(mode, 0);
    TRY_SET(shortArg, 0);
    TRY_SET(intArg, 0);

    for(i = 0; str[i] == ' ' || str[i] == '\n'; i++);

    for(; !isStop(str[i]) && res; i++)
        if ( !CBraketFlag ) {
            if ( str[i] == 'd' ) {
                if ( !strncmp(str + i, "dword ", strlen("dword ")) && ableDwordFlag && flag == SRC) {
                    dwordFlag = 1;
                    tmp = 2;

                    for(i += strlen("dword "); str[i] == ' ' || str[i] == '\n'; i++);

                    i--;
                } else if ( base == HEX ) {
                    ableRegFlag = 0;

                    TRY_SET(shortArg, TRY_GET(shortArg) * base + (str[i] - 'a') + 10);
                    TRY_SET(intArg, TRY_GET(intArg) * base + (str[i] - 'a') + 10);
                } else
                    res = 0;
            } else if ( str[i] == '[' ) {
                if ( ableOBraketFlag ) {
                    ableOBraketFlag = 0;
                    ableCBraketFlag = 1;
                    OBraketFlag = 1;
                } else
                    res = 0;
            } else if ( str[i] == ']' ) {
                if ( ableCBraketFlag ) {
                    ableCBraketFlag = 0;
                    CBraketFlag = 1;
                } else
                    res = 0;
            } else if ( str[i] == 'x' ) {
                if ( tmp && ableHexFlag ) {
                    ableHexFlag = 0;
                    base = HEX;
                } else
                    res = 0;
            } else if ( str[i] == 'G' ) {
                if ( ableRegFlag ) {
                    ableRegFlag = 0;
                    ableHexFlag = 0;
                    regFlag = 1;
                } else
                    res = 0;
            } else if ( str[i] == 'S' ) {
                if ( ableRegFlag ) {
                    ableRegFlag = 0;
                    ableHexFlag = 0;
                    regFlag = 2;
                } else
                    res = 0;
            } else if ( str[i] >= 'a' && str[i] <= 'f' ) {
                if ( base == HEX ) {
                    ableRegFlag = 0;

                    TRY_SET(shortArg, TRY_GET(shortArg) * base + (str[i] - 'a') + 10);
                    TRY_SET(intArg, TRY_GET(intArg) * base + (str[i] - 'a') + 10);
                } else
                    res = 0;
            } else if ( str[i] >= '0' && str[i] <= '9' ) {
                ableOBraketFlag = 0;
                ableRegFlag = 0;

                TRY_SET(shortArg, TRY_GET(shortArg) * base + (str[i] - '0'));
                TRY_SET(intArg, TRY_GET(intArg) * base + (str[i] - '0'));
            } else
                res = 0;

            ableOBraketFlag = (tmp == 2)? 1 : 0;
            ableDwordFlag = 0;
            tmp = (str[i] == '0')? 1 : 0;
        } else
            res = 0;

    if ( res && !(ableCBraketFlag && OBraketFlag) ) {
        if ( res && dwordFlag ) {
            if ( flag == SRC )
                TRY_SET(mode, TRY_GET(mode) | SRC_ISDW);
            else
                res = 0;
        }

        if ( res && regFlag == 1 ) {
            if ( shortArg && (*shortArg) < GNUM || intArg && (*intArg) < GNUM ) {
                if ( flag == DST )
                    TRY_SET(mode, TRY_GET(mode) | DST_ISRG);
                else if ( flag == SRC )
                    TRY_SET(mode, TRY_GET(mode) | SRC_ISRG);
                else
                    res = 0;
            } else
                res = 0;
        }

        if ( res && regFlag == 2 ) {
            if ( shortArg && (*shortArg) < SNUM || intArg && (*intArg) < SNUM ) {
                if ( flag == DST ) {
                    TRY_SET(mode, TRY_GET(mode) | DST_ISRG);
                    TRY_SET(shortArg, TRY_GET(shortArg) + GNUM);
                    TRY_SET(intArg, TRY_GET(intArg) + GNUM);
                } else if ( flag == SRC ) {
                    TRY_SET(mode, TRY_GET(mode) | SRC_ISRG);
                    TRY_SET(shortArg, TRY_GET(shortArg) + GNUM);
                    TRY_SET(intArg, TRY_GET(intArg) + GNUM);
                } else
                    res = 0;
            } else
                res = 0;
        }

        if ( res && base == DCM ) {
            if ( !regFlag && flag == SRC )
                TRY_SET(mode, TRY_GET(mode) | SRC_ISVL);
            else if ( !regFlag )
                res = 0;
        }

        if ( res && CBraketFlag ) {
            if ( flag == SRC && (base == HEX || regFlag) )
                TRY_SET(mode, TRY_GET(mode) | SRC_ISCA);
            else if ( flag == DST && base == HEX )
                TRY_SET(mode, TRY_GET(mode) | DST_ISCA);
            else
                res = 0;
        }
    } else
        res = 0;

    if ( res )
        res = i;

    return res;
}

unsigned int getCommand(unsigned short* comm, unsigned char* argNum, const char* str) {
    unsigned int res = 0;

    TRY_SET(comm, 0);
    TRY_SET(argNum, 0);

    for(res = 0; str[res] == ' ' || str[res] == '\n'; res++);

    if ( !strncmp(str + res, "NOP", strlen("NOP")) && isStop(str[res + strlen("NOP")]) ) {
        TRY_SET(comm, TRY_GET(comm) | NOP);
        res += strlen("NOP") + 1;
        TRY_SET(argNum, 0);
    } else if ( !strncmp(str + res, "RES", strlen("RES")) && isStop(str[res + strlen("RET")]) ) {
        TRY_SET(comm, TRY_GET(comm) | RET);
        res += strlen("RET") + 1;
        TRY_SET(argNum, 0);
    } else if ( !strncmp(str + res, "HLT", strlen("HLT")) && isStop(str[res + strlen("HLT")]) ) {
        TRY_SET(comm, TRY_GET(comm) | HLT);
        res += strlen("HLT") + 1;
        TRY_SET(argNum, 0);
    } else if ( !strncmp(str + res, "PUSH ", strlen("PUSH ")) ) {
        TRY_SET(comm, TRY_GET(comm) | PUSH);
        res += strlen("PUSH ");
        TRY_SET(argNum, 1);
    } else if ( !strncmp(str + res, "POP ", strlen("POP ")) ) {
        TRY_SET(comm, TRY_GET(comm) | POP);
        res += strlen("POP ");
        TRY_SET(argNum, 1);
    } else if ( !strncmp(str + res, "JMP ", strlen("JMP ")) ) {
        TRY_SET(comm, TRY_GET(comm) | JMP);
        res += strlen("JMP ");
        TRY_SET(argNum, 1);
    } else if ( !strncmp(str + res, "JE ", strlen("JE ")) ) {
        TRY_SET(comm, TRY_GET(comm) | JE);
        res += strlen("JE ");
        TRY_SET(argNum, 1);
    } else if ( !strncmp(str + res, "JNE ", strlen("JNE ")) ) {
        TRY_SET(comm, TRY_GET(comm) | JNE);
        res += strlen("JNE ");
        TRY_SET(argNum, 1);
    } else if ( !strncmp(str + res, "CALL ", strlen("CALL ")) ) {
        TRY_SET(comm, TRY_GET(comm) | CALL);
        res += strlen("CALL ");
        TRY_SET(argNum, 1);
    } else if ( !strncmp(str + res, "INC ", strlen("INC ")) ) {
        TRY_SET(comm, TRY_GET(comm) | INC);
        res += strlen("INC ");
        TRY_SET(argNum, 1);
    } else if ( !strncmp(str + res, "DEC ", strlen("DEC ")) ) {
        TRY_SET(comm, TRY_GET(comm) | DEC);
        res += strlen("DEC ");
        TRY_SET(argNum, 1);
    } else if ( !strncmp(str + res, "ADD ", strlen("ADD ")) ) {
        TRY_SET(comm, TRY_GET(comm) | ADD);
        res += strlen("ADD ");
        TRY_SET(argNum, 2);
    } else if ( !strncmp(str + res, "SUB ", strlen("SUB ")) ) {
        TRY_SET(comm, TRY_GET(comm) | SUB);
        res += strlen("SUB ");
        TRY_SET(argNum, 2);
    } else if ( !strncmp(str + res, "OR ", strlen("OR ")) ) {
        TRY_SET(comm, TRY_GET(comm) | OR);
        res += strlen("OR ");
        TRY_SET(argNum, 2);
    } else if ( !strncmp(str + res, "XOR ", strlen("XOR ")) ) {
        TRY_SET(comm, TRY_GET(comm) | XOR);
        res += strlen("XOR ");
        TRY_SET(argNum, 2);
    } else if ( !strncmp(str + res, "AND ", strlen("AND ")) ) {
        TRY_SET(comm, TRY_GET(comm) | AND);
        res += strlen("AND ");
        TRY_SET(argNum, 2);
    } else if ( !strncmp(str + res, "MOV ", strlen("MOV ")) ) {
        TRY_SET(comm, TRY_GET(comm) | MOV);
        res += strlen("MOV ");
        TRY_SET(argNum, 2);
    } else if ( !strncmp(str + res, "CMP ", strlen("CMP ")) ) {
        TRY_SET(comm, TRY_GET(comm) | CMP);
        res += strlen("CMP ");
        TRY_SET(argNum, 2);
    }

    return res;
}

unsigned char makeCommand(void *space, const char* str) {
    unsigned char res, argNum;
    unsigned int len = getCommand((unsigned short*)space, &argNum, str);

    if ( len ) {
        str += len;

        if ( argNum == 1 ) {
            unsigned short mode, arg;
            unsigned int darg;

            if ( getArg(&mode, &arg, &darg, str, SRC) ) {
                ((unsigned short*)space)[0] |= mode;

                if ( mode & SRC_ISDW ) {
                    memcpy(space + sizeof(short), &darg, sizeof(int));
                    res = sizeof(short) + sizeof(int);
                } else {
                    memcpy(space + sizeof(short), &arg, sizeof(short));
                    res = 2 * sizeof(short);
                }
            } else
                res = 0;
        } else if ( argNum == 2 ) {
            unsigned short mode;

            len = getArg(&mode, (unsigned short*)(space + sizeof(short)), NULL, str, DST);

            if ( len ) {
                unsigned short arg;
                unsigned int darg;

                str += len;
                ((unsigned short*)space)[0] |= mode;

                if ( getArg(&mode, &arg, &darg, str, SRC) ) {
                    ((unsigned short*)space)[0] |= mode;

                    if ( mode & SRC_ISDW ) {
                        memcpy((unsigned int*)(space + 2 * sizeof(short)), &darg, sizeof(int));
                        res = 2 * sizeof(short) + sizeof(int);
                    } else {
                        memcpy((unsigned short*)(space + 2 * sizeof(short)), &arg, sizeof(short));
                        res = 3 * sizeof(short);
                    }
                } else
                    res = 0;
            } else
                res = 0;
        } else
            res = sizeof(short);
    } else
        res = 0;

    return res;
}

char compileFile(FILE* output, FILE* input) {
    unsigned short err = 0;
    unsigned char len;

    if ( output && input ) {
    unsigned int i = 0;
        char space[2 * sizeof(short) + sizeof(int)];
        char str[STR_LEN];

        fseek(output, 0, SEEK_SET);

        for(; !err && fgets(str, STR_LEN, input) ;) {
            len = makeCommand(space, str);

            if ( len ) {
                fseek(output, i, SEEK_SET);
                fwrite(space, 1, len, output);
                i += len;
            } else
                err = -1;
        }

        fflush(output);
    } else
        err = -1;

    return err;
}

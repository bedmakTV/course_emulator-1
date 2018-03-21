#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "nyasm.h"

#define errprint(args...) /* (stderr, args) */

#define FNC_PREFIX "fnc_"
#define RET_PREFIX "ret_"

const char *comarr[] = {
	"nop",
	"mov",
	"push",
	"pop",
	"add",
	"sub",
	"dec",
	"inc",
	"xor",
	"and",
	"or",
	"cmp",
	"jmp",
	"call",
	"ret",
	"je",
	"jne",
	"hlt"
};

void clearColon(char *str) {
	while (*(str++) && *str != ':')
		;

	*str = '\0';
}

char *strtolow(char *str) {
	char *s = str;

	while (*s) {
		*s = tolower(*s);
		s++;
	}

	return str;
}

int addLine(code *cd, char *ln) {
	char *dp;
	line *cl;
	int   i;

	cd->lns = (line *)realloc(cd->lns, sizeof(line) * ++(cd->nln));

	cl = cd->lns + cd->nln - 1;

	if (dp = strstr(ln, "dword"))
		while (*dp && isalpha(*dp))
			*(dp++) = ' ';

	for (i = 0; i < MAX_LEN && ln[i]; ++i)
		if (ln[i] == ',')
			ln[i] = ' ';

	cl->arg[0][0] = '\0';
	cl->arg[1][0] = '\0';

	sscanf(ln, "%s%s%s", cl->comd, cl->arg[0], cl->arg[1]);

	if (dp)
		sprintf(cl->comd + strlen(cl->comd), " dword");

	strtolow(cl->comd);
}

char *addLabel(code *cd, char *ln) {
	dfval *l;
	int    n = 0;

	cd->lbs = (dfval *)realloc(cd->lbs, sizeof(dfval) * ++(cd->nlb));

	l = cd->lbs + cd->nlb - 1;

	sscanf(ln, "%s%n", l->name, &n);

	clearColon(l->name);
	sprintf(l->valu, "%d", cd->nln);

	return ln + n;
}

char *getNE(char *str) {
	while (*str && (*str == ' ' || *str == '\t' || *str == '\n' || *str == '\r'))
		str++;

	return str;
}

int coderead(char *ifile, code *cd) {
	FILE *fin = fopen(ifile, "r");
	char  rln   [MAX_LEN];
	char  t_str [MAX_LEN];
	char  f_name[MAX_LEN];            /* stores function name in which we are */
	char *t;
	int   err = 0;

	if (fin) {
		cd->nln = 0;
		cd->nlb = 0;

		cd->lns = (line  *)malloc(sizeof(line));
		cd->lbs = (dfval *)malloc(sizeof(dfval));

		while (fgets(rln, MAX_LEN - 1, fin)) {
			char *ln = getNE(rln);
			char *hf = strstr(ln, "function") || strstr(ln, "FUNCTION");
			char *hr = strstr(ln, "RET")      || strstr(ln, "ret");
			char *hc = strstr(ln, "CALL")     || strstr(ln, "call");

			/* In order to not mix up with labels */
			if (hf)
				clearColon(ln);

			/* there might be several labels on line, & code after them */
			while (strchr(ln, ':'))
				ln = getNE(addLabel(cd, ln));

			/* Now, even if we had labels, there all lefter than 'ln' */
			if (hf) {
				static int sfln = strlen("function ");

				sprintf   (t_str, "JMP " RET_PREFIX "%s", ln + sfln);
				clearColon(t_str);
				addLine   (cd, t_str);

				sprintf   (t_str, "%s%s", FNC_PREFIX, ln + sfln);
				addLabel  (cd, t_str);

				sprintf   (f_name, "%s", ln + sfln);
			}
			else if (hr) {
				addLine (cd, "RET");

				if (strlen(f_name)) {
					sprintf (t_str, RET_PREFIX "%s", f_name);
					addLabel(cd, t_str);
				}
			}
			else if (hc)  {
				if (!strstr(ln, FNC_PREFIX)) {
					static int clln = strlen("CALL ");

					sprintf(t_str, "CALL " FNC_PREFIX "%s", ln + clln);

					addLine(cd, t_str);
				}
				else
					addLine(cd, ln);
			}
			else if (t = strchr(ln, '[')) {
				char  t2[MAX_LEN];
				char  t3[MAX_LEN];
				char  tx[2];
				char *te = strchr(t, ']');

				tx[0] = '\0';
				t2[0] = '\0';
				t3[0] = '\0';

				*te = '\0';
				sscanf (t + 1, "%s%s%s", t2, tx, t3);

				sprintf(t_str, "MOV S2 %s", t2);
				addLine(cd, t_str);

				if (tx[0]) {
					//fprintf(stderr, "%d %c\n", tx[0], tx[0]);
					sprintf(t_str, "%s S2 %s", (tx[0] == '+') ? "ADD" : "SUB", t3);
					addLine(cd, t_str);
				}

				sprintf(t2, "%s", "ICA");
				memcpy (t, t2, strlen("ICA"));

				t += 3;

				while (*t && *t != ']')
					*(t++) = ' ';

				*t = ' ';

				sprintf(t_str, "%s", ln);
				addLine(cd, t_str);
			}
			else if (strlen(ln) > 2)
				addLine(cd, ln);
		}

		fclose(fin);
	}
	else
		err = -1;

	return err;
}

int codeprint(char *ofile, code *cd) {
	FILE *ofl = ofile ? fopen(ofile, "w") : stdout;
	int  err  = 0;
	int  i;

	for (i = 0; i < cd->nln; ++i) {
		int k;
		int lbl = 0;

		for (k = 0; k < cd->nlb; ++k) {
			int pos = atoi(cd->lbs[k].valu);

			if (pos == i) {
				fprintf(ofl, "%s:\n", cd->lbs[k].name);

				if (lbl)
					fprintf(ofl, "\n");

				lbl = 1;
			}
		}

		fprintf(
			ofl,
			"\t %-17s %-5s %-5s\n",
			cd->lns[i].comd,
			cd->lns[i].arg[0],
			cd->lns[i].arg[1]
		);
	}

	return err;
}

void deleteCode(code *cd) {
	free(cd->lns);
	free(cd->lbs);
}

cmd getCmdByName(char *comd) {
	int   i   =  0;
	cmd   res = -1;
	char *t;

	if (t = strstr(comd, "dword"))
		*(t - 1) = '\0';

	for (i = 0; i <= HLT; ++i)
		if (!strcmp(comd, comarr[i])) {
			res = i;
			break;
		}

	if (t)
		*(t - 1) = 'd';

	return res | (ASZ_ISDW * (t != NULL));
}

char *getDefbyName(dfval *df, int nodf, char *name) {
	int   i;
	char *res;

	for (i = 0; i < nodf; ++i)
		if (!strcmp(name, df[i].name)) {
			res = df[i].valu;

			break;
		}

	return res;
}

int parseLine(ECM *e, line *l, line **jmps, int *szbln) {
	int   lsz  = 2, i;
	word  comd;
	dword arg[2];
	int   asz[2];

	asz[0] = 0;
	asz[1] = 0;

	comd = getCmdByName(l->comd);

	for (i = 0; i < 2; ++i) {
		if (strlen(l->arg[i]) > 0) {
			if (l->arg[i][0] == 'G') {                    /* General register */
				comd  |= (FST_ISRG >> i);
				arg[i] = atoi(l->arg[i] + 1);
			}
			else if (l->arg[i][0] == 'S') {               /* Special register */
				comd  |= (FST_ISRG >> i);
				arg[i] = atoi(l->arg[i] + 1) + e->gNum;
			}
			else if (!strcmp(l->arg[i], "ICA")) {
				comd  |= (FST_ISCA >> i);
				arg[i] = 0;
			}
			else {
				int noj = 0;

				switch (comd & CMD_MASK) {
					case JMP:
					case JE:
					case JNE:
					case CALL:
						comd |= ASZ_ISDW;

						while (jmps[noj])
							noj++;

						jmps[noj++] = l;
						jmps[noj]   = NULL;
						arg [i]     = 0;
						break;

					default:
						arg[i] = atoi(l->arg[i]);
						break;
				}

				comd  |= (FST_ISVL >> i);
			}

			asz[i] = getASz(comd, i);
			lsz   += asz[i];
		}
	}

	setInt32(e->mem + *szbln, CMD_SIZE, comd);

	setInt32(e->mem + *szbln + CMD_SIZE,  asz[0], arg[0]);
	setInt32(e->mem + *szbln + CMD_SIZE + asz[0], asz[1], arg[1]);

	return lsz + *szbln;
}

int codecmpl(char *ofile, code *cd, ECM *e) {
	FILE *of  = fopen(ofile, "w");
	int   err = 0;

	if (of) {
		int   *szByLn = (int   *)malloc(sizeof(int)    * (cd->nln + 1));
		line **jmps   = (line **)malloc(sizeof(line *) * (cd->nln + 1));
		int    i;

		szByLn[0] = 0;
		jmps  [0] = NULL;

		for (i = 0; i < cd->nln; ++i)
			szByLn[i + 1] = parseLine(e, cd->lns + i, jmps, szByLn + i);

		errprint("compiling %d\n", __LINE__);

		for (i = 0; jmps[i]; ++i) {
			setInt32 (
				e->mem + szByLn[jmps[i] - cd->lns] + CMD_SIZE,
				4,
				szByLn[atoi(getDefbyName(cd->lbs, cd->nlb, jmps[i]->arg[0]))]
			);

			errprint("JUMPS\n%s %d %d\n",
				jmps[i]->arg[0],
				szByLn[jmps[i] - cd->lns] + CMD_SIZE,
				getInt32(e->mem + szByLn[jmps[i] - cd->lns] + CMD_SIZE, 4)
			);
		}

		printECM(e, 15, 0, 100);
		memdump (e, ofile, 0, szByLn[cd->nln]);

		free  (szByLn);
		free  (jmps);
		fclose(of);
	}
	else
		err = -2;

	return err;
}

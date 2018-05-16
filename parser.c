#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>

regex_t regex_line;
regex_t regex_numfuses;
FILE *jedfile;
int reti;
char *line;
size_t len;
ssize_t read;
regmatch_t match[2];

void str_trim(char *input)
{
	while (*input++) {
		if (*input == 0x20 || *input == '*') {
			memmove(input, input+1, strlen(input+1)+1);
			input--;
		}
	}
}

int main(int argc, char *argv)
{
	printf("Hello\n");
	reti = regcomp(&regex_numfuses, "^QF.*\\*\n$", 0);
	if (reti) {
		fprintf(stderr, "could not compile regex\n");
		exit(1);
	}
	reti = regcomp(&regex_line, "^L[0-9]{1,4}\\ (0|1|\\ )+\\*\n$", REG_EXTENDED);
	if (reti) {
		fprintf(stderr, "could not compile regex\n");
		exit(1);
	}


	FILE *jedfile = fopen("test.jed", "r");
	if (!jedfile) {
		return 1;
	}
	char *endptr;
	while ((read = getline(&line, &len, jedfile)) != -1) {
		fprintf(stdout, "%s\n", line);
		reti = regexec(&regex_numfuses, line, 1, match, 0);
		if (!reti) {
			// get number of fuses
			int nfuses = strtol(line+2, &endptr, 10);
			printf("Number of fuses: %d\n", nfuses);
			continue;
		}
		reti = regexec(&regex_line, line, 1, match, 0);
		if (!reti) {
			// get fuse number
			int fuse = strtol(line+1, &endptr, 10);
			printf("fuse = %d\n", fuse);
			str_trim(endptr);
			printf("bitstream: %s\n", endptr);
			continue;
		}

	}

	fclose(jedfile);
	return 0;
}

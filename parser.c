#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>

regex_t regex;
regex_t rex_fusenum;
FILE *jedfile;
int reti;
char *line;
size_t len;
ssize_t read;
regmatch_t match[2];

void str_trim(char *input)
{
	while (*input++) {
		if (*input == 0x20) {
			memmove(input, input+1, strlen(input+1)+1);
			input--;
		}
	}
}

int main(void)
{
	reti = regcomp(&regex, "^L[0-9]+\\(0|1|\\ )+(\\*)$", REG_EXTENDED);
	if (reti) {
		fprintf(stderr, "could not compile regex\n");
		exit(1);
	}
	FILE *jedfile = fopen("test.jed", "r");
	if (!jedfile) {
		return 1;
	}
	while ((read = getline(&line, &len, jedfile)) != -1) {
		fprintf(stdout, "%s\n", line);
		reti = regexec(&regex, line, 2, match, 0);
		if (!reti) {
			// get fuse number
			char *endptr;
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

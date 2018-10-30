#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
	SIMPLE, COMPLEX, REGISTERED
} GAL_MODE;

typedef struct _input_node {
	bool *pin_ref;
	struct _input_node *next;
} input_node;

typedef struct _OLMC {
	int input_row_first;
	int input_row_last;
	bool *pout_ref;
} OLMC;

OLMC *olmc_init(int input_row_first, int input_row_last);

typedef struct _GAL {
	GAL_MODE mode;
	int term_width;
	int input_fuse_low;
	int input_fuse_high;
	int PTD_low;
	int num_rows;
	bool *PTD;
	bool *inputs;
	bool *outputs;
	int *input_pins;
	int *output_pins;
	int npins;
	int npouts;
	int nOLMCs;
	OLMC **olmc;
	input_node **input_list;
	void (*initfunc)(void);
	void (*deinitfunc)(void);
} GAL;

GAL gal;

bool product(int fuse)
{
	int row = fuse - gal.input_fuse_low;
	if (gal.PTD[gal.PTD_low + row]) return false;

	bool res =  true;

	input_node *input = gal.input_list[row];
	if (!input) return false;
	do {
		if (!*(input->pin_ref)) res = false;
	} while(input = input->next);
	return res;
}

OLMC *olmc_init(int input_row_first, int input_row_last)
{
	OLMC *olmc = calloc(1, sizeof(OLMC));
	olmc->input_row_first = input_row_first;
	olmc->input_row_last = input_row_last;
	return olmc;
}

bool olmc_calc_or(int idx)
{
	// TODO: check if first row goes into OR (depending on config)

	// get indices of first fuse for each row
	min_row = gal.olmc[idx].input_row_first;
	max_row = gal.olmc[idx].input_row_last;

	// or is true if one input is true
	for (int first_fuse = min_row; first_fuse < max_row;
	     first_fuse += term_width) {
		if (product(first_fuse)) return true;
	}
}

void append_input(input_node **input_list, bool *pinref)
{
	input_node **next = input_list;
	while(*next) {
		next = &((*next)->next);
	}
	*next = calloc(1, sizeof(input_node));
	(*next)->pin_ref = pinref;
}

void free_input_list(input_node *node)
{
	if (!node) return;
	input_node **p = &(node->next);
	input_node *tmp;
	while (*p) {
		tmp = (*p)->next;
		free(*p);
		p = &tmp;
	}
}


void gal16v8_init(void)
{
	gal.term_width = 32;
	gal.input_fuse_low = 0;
	gal.input_fuse_high = 2047;
	gal.PTD_low = 2128;
	gal.num_rows = (gal.input_fuse_high - gal.input_fuse_low + 1) / gal.term_width;
	gal.PTD = calloc(gal.num_rows, sizeof(bool));
	if (!gal.PTD) exit(1);
	gal.input_list = calloc(gal.num_rows, sizeof(input_node *));
	if (!gal.input_list) exit(1);
	gal.mode = SIMPLE;
	gal.nOLMCs = 8;
	gal.olmc = calloc(gal.nOLMCs, sizeof(OLMC));
	gal.olmc[0] = olmc_init(0, 224);
	gal.olmc[0] = olmc_init(256, 480);
	gal.olmc[0] = olmc_init(512, 736);
	gal.olmc[0] = olmc_init(768, 992);
	gal.olmc[0] = olmc_init(1024, 1248);
	gal.olmc[0] = olmc_init(1280, 1504);
	gal.olmc[0] = olmc_init(1536, 1760);
	gal.olmc[0] = olmc_init(1792, 2016);
}

void gal_deinit(void)
{
	for (int i = 0; i < gal.nOLMCs; i++) {
		free(gal.olmc[i]);
	}
	free(gal.olmc);
	free_input_list(gal.input_list[0]);
	free(gal.input_list);
	free(gal.PTD);
}

int main(int argc, char *argv)
{
	gal.initfunc = &gal16v8_init;
	gal.deinitfunc = &gal_deinit;
	gal.initfunc();
	printf("Number of fabric rows: %d\n", gal.num_rows);

	printf("%s\n", product(0) ? "true" : "false");
	gal.deinitfunc();
	return 0;
}

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>
#include "markov.h"
#include "debug.h"

#define MAX_LINE 1023

typedef float (*f_func)(char *state, char *next_state, int state_len);
typedef float (*g_func)(char *state, char *next_state, int state_len);

void shift(char *state, int c, char *next_state, int state_len){
	memmove(next_state, state + 1, state_len - 1);
	next_state[state_len - 1] = c;
}

int train_on_file(char *path, int state_len)
{
	int i = 0, c = 0, r = 0;
	FILE *f = NULL;
	char *state = NULL, *next_state = NULL, *tmp = NULL;

	f = fopen(path, "r");
	check(f, "Could not open file: %s", path);

	state = malloc(state_len + 1);
	check_mem(state);
	next_state = malloc(state_len + 1);
	check_mem(next_state);
	for(i = 0 ; i < state_len ; i++){
		state[i] = next_state[i] = ' ';	
	}
	state[i] = next_state[i] = '\0';

	while((c = fgetc(f)) != EOF){
		if(iscntrl(c)) c = ' ';

		shift(state, c, next_state, state_len);
	    r = MC_add_trans(state, next_state);	
		check(r, "Failed to add transition %s -> %s", state, next_state);

		tmp = state;
		state = next_state;
		next_state = tmp;
	}
	MC_calc_p();

	free(state);
	free(next_state);
	fclose(f);

	return 1;

error:

	if(f) fclose(f);
	if(state) free(state);
	if(next_state) free(next_state);

	return 0;
}

int print_state_p(char *line, int state_len){
	int i = 0;
	char c = 0;
	char *state = NULL, *next_state = NULL, *tmp = NULL;
	MC_Transition *t = NULL;

	state = calloc(state_len + 1, sizeof(char));
	check_mem(state);
	next_state = calloc(state_len + 1, sizeof(char));
	check_mem(next_state);

	for(i = 0 ; i < state_len ; i++){
		state[i] = next_state[i] = ' ';	
	}

	for(i = 0 ; line[i] ; i++){
		c = line[i];

		if(iscntrl(c)) c = ' ';

		shift(state, c, next_state, state_len);	
		t = MC_lookup(state, next_state, 0);

		if (NULL == t){
			printf("P('%s', '%s') = %f\n", state, next_state, 0.0f);	
		} else {
			printf("P('%s', '%s') = %f\n", state, next_state, t->p);
		}

		tmp = state;
		state = next_state;
		next_state = tmp;
	}

	free(state);
	free(next_state);

	return 1;

error:

	if(state) free(state);
	if(next_state) free(next_state);

	return 0;
}

float miss_p_f(char *state, char *next_state, int state_len) 
{
	MC_Transition **col = NULL;
	MC_Transition *t = NULL;
	int i = 0;
	float z = 1.0f, p_miss = 0.0f;

	t = MC_lookup(state, next_state, 0);
	if(t){
		col = MC_lookup_column(state);
		for(i = 0 ; i < TRANS_STATE_TAB_W ; i++){
			for(t = col[i] ; t ; t = t->next){
				if(0 == strncmp(t->state, state, state_len) && 0 != strncmp(t->next_state, next_state, state_len)){
					p_miss += t->p;
				}
			}			
		}
		return p_miss;
	} else {
		return z;
	}
}

float miss_p_g(char *state, char *next_state, int state_len)
{
	return 1;
}

float miss_p_metric(char *line, int state_len, f_func ff, g_func gf)
{
	float x = 0.0f, y = 0.0f;
	int i = 0;
	char c = 0;
	char *state = NULL, *next_state = NULL, *tmp = NULL;

	state = calloc(state_len + 1, sizeof(char));
	check_mem(state);
	next_state = calloc(state_len + 1, sizeof(char));
	check_mem(next_state);

	for(i = 0 ; i < state_len ; i++){
		state[i] = next_state[i] = ' ';
	}

	for(i = 0 ; line[i] ; i++){
		c = line[i];		

		if(iscntrl(c)) c = ' ';

		shift(state, c, next_state, state_len);

		y = y + ff(state, next_state, state_len);
		x = x + gf(state, next_state, state_len);

		tmp = state;
		state = next_state;
		next_state = tmp;
	}

	if(0.0f == x) return 0.0f;

	return y/x;
	
error:

	if(state) free(state);
	if(next_state) free(next_state);

	return 0.0f;
}

int main(int argc, char *argv[])
{
	int r = 0;
	char line[MAX_LINE+1];
	int w = 3;

	r = MC_init(w);
	assert(r && "MC_init failed");

	r = train_on_file("corpus/hg1.txt", w);
	assert(r && "Train on hg file failed");

	r = train_on_file("corpus/dict.txt", w);
	assert(r && "Train on dict file failed");

	r = train_on_file("corpus/huckle.txt", w);
	assert(r && "Train on dict file failed");

	r = train_on_file("corpus/tom.txt", w);
	assert(r && "Train on dict file failed");

	r = train_on_file("corpus/alice.txt", w);
	assert(r && "Train on dict file failed");

	while((fgets(line, MAX_LINE, stdin)) != NULL){
		printf("Miss probability metric: %f\n", miss_p_metric(line, w, miss_p_f, miss_p_g));
	}

	MC_destroy();

	return 0;
}

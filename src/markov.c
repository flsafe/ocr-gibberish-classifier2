#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "debug.h"
#include "markov.h"

enum
{
	TRANS_STATE_TAB_W = 2047,         /* The width of the transition state hash table. */
	TRANS_STATE_TAB_H = 2047,         /* The hieght of the transition state hash table. */
	STATE_TAB       = 2047            /* Size of the state hash table */
};

MC_Transition ***trans_state_tab;      /* Transition state hash table, used to define a transition hash table. */

MC_State **state_tab;                 /* State table used to count the number of times a state has appeared. */

int STATE_LEN;

/* Initialize the hash tables */
int MC_init(int state_len)
{
	int i = 0;

	check(state_len >= 1, "state");

	STATE_LEN = state_len;
	
	trans_state_tab = calloc(TRANS_STATE_TAB_W, sizeof(MC_Transition*));
	check_mem(trans_state_tab);
	for(i = 0 ; i < TRANS_STATE_TAB_W ; i++){
		trans_state_tab[i] = calloc(TRANS_STATE_TAB_H, sizeof(MC_Transition*));
		check_mem(trans_state_tab[i]);
	}

	state_tab = calloc(STATE_TAB, sizeof(MC_State*));
	check_mem(state_tab);

	return 1;

error:

	for(i = 0 ; i < TRANS_STATE_TAB_W ; i++){
		if(trans_state_tab[i]) free(trans_state_tab[i]);
	}
	if(trans_state_tab) free(trans_state_tab);

	if(state_tab) free(state_tab);

	return 0;
}

void MC_destroy()
{
	MC_Transition *trans = NULL, *n = NULL;
	int c = 0, r = 0;

	for(c = 0 ; c < TRANS_STATE_TAB_W ; c++){
		for(r = 0 ; r < TRANS_STATE_TAB_H ; r++){
			trans = trans_state_tab[c][r];
			while(trans){
				n = trans->next;
				if(trans->state) free(trans->state);
				if(trans->next_state) free(trans->next_state);
				free(trans);
				trans = n;
			}
			trans_state_tab[c][r] = NULL;
		}
		if(trans_state_tab[c]) free(trans_state_tab[c]);
	}

	if(trans_state_tab) free(trans_state_tab);
	if(state_tab) free(state_tab);
}

unsigned int hash(char *s, int mod)
{
	unsigned int h = 0;
	int mult = 32, i = 0;

	for(i = 0 ; i < STATE_LEN ; i++){
		h = mult * h + s[i];
	}

	return h % mod; 
}

int count_state(char *state)
{
	unsigned int h = 0;
	MC_State *st = NULL;


	h = hash(state, STATE_TAB);
	for(st = state_tab[h] ; st ; st = st->next){
		if(0 == strncmp(state, st->state, STATE_LEN)){
			st->count++;	
			return 1;
		}
	}

	st = calloc(1, sizeof(MC_State));
	check_mem(st);

	st->state = strndup(state, STATE_LEN);
	check_mem(st->state);
	st->next = state_tab[h];
	state_tab[h] = st;

	return 1;
error:

	if(st->state) free(st->state);
	if(st) free(st);

	return 0;
}

int MC_add_trans(char *state, char *next_state)
{
	unsigned int h = 0, r = 0;
	MC_Transition *t = NULL;

	t = MC_lookup(state, next_state, 1);
	check(NULL != t, "Failed to lookup transition for %s -> %s", state, next_state);
	t->count++;

	r = count_state(state);
	check(r, "Failed to count state: %s", state);

	r = count_state(next_state);
	check(r, "Failed to count next_state: %s", next_state);

	return 1;

error:

	return 0;
}

int MC_get_count(char *state)
{
	return 0;
}

MC_Transition *MC_lookup(char *state, char *next_state, int create)
{
	MC_Transition *trans = NULL;
	unsigned int c,r;

	c = hash(state, TRANS_STATE_TAB_W);
	r = hash(next_state, TRANS_STATE_TAB_H);

	for(trans = trans_state_tab[c][r] ; trans ; trans = trans->next){
		if(0 == strncmp(state, trans->state, STATE_LEN) && 0 == strncmp(next_state, trans->next_state, STATE_LEN)){
			return trans;
		}
	}

	if(create){
		trans = calloc(1, sizeof(MC_Transition));
		check_mem(trans);

		trans->state = strndup(state, STATE_LEN);
		check_mem(trans->state);

		trans->next_state = strndup(next_state, STATE_LEN);
		check_mem(trans->next_state);

		trans->next = trans_state_tab[c][r];

		trans->count = 1;

		trans_state_tab[c][r] = trans;
	}
	
	return trans;

error:

	if(trans) free(trans);
	if(trans->state) free(trans->state);
	if(trans->next_state) free(trans->next_state);

	return NULL;
}

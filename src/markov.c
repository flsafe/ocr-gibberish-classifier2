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

MC_Transition **trans_state_tab;      /* Transition state hash table, used to define a transition hash table. */

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

error:

	for(i = 0 ; i < TRANS_STATE_TAB_W ; i++){
		if(trans_state_tab[i]) free(trans_state_tab[i]);
	}
	if(trans_state_tab) free(trans_state_tab);

	if(state_tab) free(state_tab);

	return 0;
}

int MC_add_trans(char *state, char *next_state)
{
	return 0;
}

int MC_get_count(char *state)
{
	return 0;
}

MC_Transition *MC_lookup(char *state, char *next_state)
{
	return NULL;
}

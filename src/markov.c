#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "debug.h"
#include "markov.h"

enum
{
	INIT_STATE_TAB  = 2047;         /* Length of the state hash table. */
	TRANS_STATE_TAB = 2047;         /* The length of the transition state hash table. */
	STATE_TAB       = 2047;
};

MC_Transition *init_state_tab;      /* Initial state hash table, used to define part of a transition hash table. */
MC_Transition *trans_state_tab;     /* Transition state hash table, used to define part of a transition hash table. */

MC_State *state_tab;                /* A state hash table. */

int STATE_LEN;

/* Initialize the hash tables */
int MC_init(int state_len)
{
	int i = 0;

	check(state_len >= 1, "state");

	STATE_LEN = state_len;
	
	init_state_tab = calloc(INIT_STATE_TAB, sizeof(MC_Transition*));
	check_mem(init_state_tab);
	for(i = 0 ; i < INIT_STATE_TAB ; i++){
		trans_state_tab = calloc(TRANS_STATE_TAB, sizeof(MC_Transition*));
		check_mem(trans_state_tab);
		init_state_tab[i] = trans_state_tab;
	}

	state_tab = calloc(STATE_TAB, sizeof(MC_State*));
	check_mem(state_tab);

error:

	for(i = 0 ; i < INIT_STATE_TAB){
		if(init_state_tab[i]) free(init_state_tab[i]);
	}
	if(init_state_tab) free(init_state_tab);

	if(state_tab) free(state_tab);

	return 0;
}

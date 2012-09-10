#ifndef _MARKOV_H
#define _MARKOV_H

typedef struct MC_Tran	

struct MC_Transition
{
	int count;           /* The number of times the initial state transitions to the transition state. */ 
	float p;             /* Probablility of this transition. */

	char *state;         /* The initial state. */
	char *next_state;    /* The transition state. */

	MC_Transition *next; /* A list of transitions. */
};

struct MC_State
{
	int count;              /* The number of times this state has appeared. */

	char *state;            /* The char str associated with this state. */

	MC_State *next;         /* A list of states */
};

int MC_init(int state_len); /* Initialize the data structures before using the following functions. */

int MC_add_trans(char *state, char *next_state);        /* Add transition from state to next_state. */

int MC_get_count(char *state);                          /* Get the count for this state */

MC_Transition *MC_lookup(char *state, char *next_state) /* Retrieve the transition between state and next_state. */ 

#endif

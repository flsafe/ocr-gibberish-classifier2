#include "minunit.h"
#include <markov.c>

char *test_init()
{
	mu_assert(MC_init(3), "Failed to init data structures");
	return NULL;
}

char *test_create()
{
	MC_Transition *t = NULL;

	t = MC_lookup("abc", "xyz", 1);

	mu_assert(t, "MC_lookup with create failed");
	mu_assert(0 == t->count, "Initial transition count should be 0");

	return NULL;	
}

char *all()
{
	mu_suite_start();

	mu_run_test(test_init);
	mu_run_test(test_create);

	return NULL;
}

RUN_TESTS(all)

#include "minunit.h"
#include <markov.c>

char *test_init()
{
	mu_assert(MC_init(3), "Failed to init data structures");

	MC_destroy();

	return NULL;
}

char *test_destroy()
{
	MC_Transition *t = NULL;

	MC_init(3);
	MC_lookup("abc", "xyz", 1);
	MC_destroy();

	MC_init(3);
	t = MC_lookup("abc", "xyz", 0);
	mu_assert(NULL == t, "Failed to destroy datastructures");

	MC_destroy();

	return NULL;
}

char *test_create()
{
	MC_Transition *t = NULL;

	MC_init(3);

	t = MC_lookup("abc", "xyz", 1);

	mu_assert(t, "MC_lookup with create failed");
	mu_assert(0 == t->count, "Initial transition count should be 0");

	MC_destroy();

	return NULL;	
}

char *test_lookup()
{
	MC_Transition *t = NULL;

	MC_init(3);

	MC_lookup("123", "456", 1);
	t = MC_lookup("123", "456", 0);

	mu_assert(t, "MC_lookup failed to return a previously inserted value");
	mu_assert(0 == t->count, "Transition count should be 0");

	MC_destroy();

	return NULL;
}

char *test_add_trans()
{
	MC_Transition *t = NULL;

	MC_init(3);
	
	MC_add_trans("123", "456");
	MC_add_trans("123", "456");
	MC_add_trans("123", "456");

	t = MC_lookup("123", "456", 0);
	mu_assert(t, "Failed to look up transition 123 -> 456");
	mu_assert(3 == t->count, "The transiton 123 -> 456 should have a count of 3");

	MC_destroy();

	return NULL;
}

char *test_get_count()
{
	MC_init(3);

	MC_add_trans("123", "456");
	MC_add_trans("123", "abc");
	MC_add_trans("xyz", "abc");

	mu_assert(2 == MC_get_count("123"), "Count for 123 should be 2");
	mu_assert(1 == MC_get_count("456"), "Count for 456 should be 1");
	mu_assert(2 == MC_get_count("abc"), "Count for abc should be 2");
	mu_assert(1 == MC_get_count("xyz"), "Count for xyz should be 1");

	MC_destroy();

	return NULL;
}

char *test_trans_probability()
{
	MC_Transition *t = NULL;

	MC_init(3);

	MC_add_trans("abc", "123");
	MC_add_trans("xyz", "123");
	MC_add_trans("abc", "hig");
	MC_add_trans("abc", "123");

	MC_calc_p();

	t = MC_lookup("abc", "123", 0);
	mu_assert(2.0f/3.0f == t->p, "Probablity of transition abc->123 should be 2/3");

	t = MC_lookup("abc", "hig", 0);
	mu_assert(1.0f/3.0f == t->p, "Probablity of transition abc->hig should be 1/3");

	MC_destroy();

	return NULL;
}

char *all()
{
	mu_suite_start();

	mu_run_test(test_init);
	mu_run_test(test_destroy);
	mu_run_test(test_create);
	mu_run_test(test_lookup);
	mu_run_test(test_add_trans);
	mu_run_test(test_get_count);
	mu_run_test(test_trans_probability);

	return NULL;
}

RUN_TESTS(all)

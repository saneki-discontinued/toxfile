#include <stdlib.h>
#include "check.h"

int run_test_suite(Suite *s)
{
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);

	int num_failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return num_failed;
}

int main()
{
	int num_failed = 0;

#define RUN(tsuite) (num_failed += run_test_suite((tsuite)))
	RUN(toxfile_path_suite());
#undef RUN

	return (num_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

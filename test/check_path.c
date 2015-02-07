#include <check.h>
#include <linux/limits.h>
#include "../src/path.h"

START_TEST (test_joinpath)
{
	// Simple hello/world test
	char path1[64];
	int result = joinpath(path1, "hello", "world", sizeof(path1));
	ck_assert_int_eq(result, 0);
	ck_assert_str_eq(path1, "hello/world");

	memset(path1, 0, sizeof(path1));

	// Simple maxlen test
	result = joinpath(path1, "this is", "a test", 12);
	ck_assert_int_eq(result, 0);
	ck_assert_str_eq(path1, "this is/a t");

	memset(path1, 0, sizeof(path1));

	// Potential overflow test
	char buf1a[PATH_MAX], buf1b[PATH_MAX];
	memset(buf1a, 'A', sizeof(buf1a));
	memset(buf1b, 'B', sizeof(buf1b));
	// Should result in 63 A's, finished by \0
	result = joinpath(path1, buf1a, buf1b, sizeof(path1));
	ck_assert_int_eq(result, 0);
	for(int i = 0; i < sizeof(path1) - 1; i++)
	{
		ck_assert_int_eq(path1[i], 'A');
	}

	memset(path1, 0, sizeof(path1));

	// NULL part1 test
	result = joinpath(path1, NULL, "world", sizeof(path1));
	ck_assert_int_eq(result, 0);
	ck_assert_str_eq(path1, "world");

	memset(path1, 0, sizeof(path1));

	// NULL part2 test
	result = joinpath(path1, "hello", NULL, sizeof(path1));
	ck_assert_int_eq(result, 0);
	ck_assert_str_eq(path1, "hello");

	memset(path1, 0, sizeof(path1));

	// NULL parts test
	result = joinpath(path1, NULL, NULL, sizeof(path1));
	ck_assert_int_eq(result, -1);

	// NULL dest test
	result = joinpath(NULL, "hello", "world", 16);
	ck_assert_int_eq(result, -1);
}
END_TEST

Suite * toxfile_path_suite()
{
	Suite *s = suite_create("path");
	TCase *tc_core = tcase_create("core");
	tcase_add_test(tc_core, test_joinpath);
	suite_add_tcase(s, tc_core);
	return s;
}

#include <check.h>
#include <stdio.h>
#include "../src/hex.h"

START_TEST (test_to_hex)
{
	// Simple test
	char *bin1 = "\x12\x34\x56\x78\xab\x90\xcd\xef";
	char res1[(strlen(bin1)*2)+1];
	memset(res1, 0, sizeof(res1));
	to_hex(res1, bin1, strlen(bin1), 0);
	ck_assert_str_eq(res1, "12345678ab90cdef");

	// Simple uppercase test
	to_hex(res1, bin1, strlen(bin1), HEX_UPPERCASE);
	ck_assert_str_eq(res1, "12345678AB90CDEF");
}
END_TEST

START_TEST (test_from_hex)
{
	// Simple test
	char *hex1 = "12345678ab09cdef";
	char res1[(strlen(hex1)/2)+1];
	memset(res1, 0, sizeof(res1));
	int success = from_hex(res1, hex1, strlen(hex1), 0);
	ck_assert_int_eq(success, 0);
	ck_assert_str_eq(res1, "\x12\x34\x56\x78\xab\x09\xcd\xef");

	// Odd hex length test
	memset(res1, 0, sizeof(res1));
	char *hex2 = "e4c3d";
	success = from_hex(res1, hex2, 0, 0);
	ck_assert_int_eq(success, 0);
	ck_assert_str_eq(res1, "\x0e\x4c\x3d");
}
END_TEST

Suite * toxfile_hex_suite()
{
	Suite *s = suite_create("hex");
	TCase *tc_core = tcase_create("core");
	tcase_add_test(tc_core, test_to_hex);
	tcase_add_test(tc_core, test_from_hex);
	suite_add_tcase(s, tc_core);
	return s;
}

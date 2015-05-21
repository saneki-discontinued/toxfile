/*
 * This file is part of toxfile.
 *
 * toxfile is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * toxfile is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with toxfile. If not, see <http://www.gnu.org/licenses/>.
 *
 */

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
#undef RUN

	return (num_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>

#include "deps/tinyunit/src/include/tinyunit.h"

#include "../src/tinyarg.c"


int tests_run = 0;
int suites_run = 0;

const char* test_tinyargs_pass()
{
	const char* pass[] = {
		"",
		"-a",
		"--abc",
		"--def",
		"val1",
		"-ag",
		"val2"
	};
	const int pass_size = 7;

	bool arg1;
	char arg2[1024];
	char arg3[1024];

	struct tiny_args_t* args = NULL;

	tiny_args_add_bool(&args, 'a', "abc", &arg1, "Usage 1");
	tu_assert_ptr_ne(args, NULL);
	tu_assert_int_eq(args->short_opt, 'a');
	tu_assert_str_eq(args->long_opt, "abc");

	tiny_args_add_str(&args, 'd', "def", arg2, 1024, "Usage 2");
	tu_assert_ptr_ne(args->next_arg, NULL);
	tu_assert_int_eq(args->next_arg->short_opt, 'd');
	tu_assert_str_eq(args->next_arg->long_opt, "def");

	tiny_args_add_str(&args, 'g', "ghi", arg3, 1024, "Usage 3");
	tu_assert_ptr_ne(args->next_arg->next_arg, NULL);
	tu_assert_int_eq(args->next_arg->next_arg->short_opt, 'g');
	tu_assert_str_eq(args->next_arg->next_arg->long_opt, "ghi");

	tu_assert_int_eq(tiny_args_parse(args, pass_size, pass), true);
	tu_assert_int_eq(arg1, true);
	tu_assert_str_eq(arg2, "val1");
	tu_assert_str_eq(arg3, "val2");

	tiny_args_usage("tinyargs", args);

	tiny_args_destroy(args);

	return MU_SUCCESS;
}

const char* test_tinyargs_undefined_arg_short_opt()
{
	const char* fail[] = {
		"",
		"-az"
	};
	const int fail_size = 2;

	bool arg1;

	struct tiny_args_t* args = NULL;

	tiny_args_add_bool(&args, 'a', "abc", &arg1, NULL);
	tu_assert_ptr_ne(args, NULL);
	tu_assert_int_eq(args->short_opt, 'a');
	tu_assert_str_eq(args->long_opt, "abc");

	tu_assert_int_ne(tiny_args_parse(args, fail_size, fail), true);
	tu_assert_int_eq(arg1, true);

	tiny_args_destroy(args);

	return MU_SUCCESS;
}

const char* test_tinyargs_undefined_arg_long_opt()
{
	const char* fail[] = {
		"",
		"--zzz"
	};
	const int fail_size = 2;

	bool arg1;

	struct tiny_args_t* args = NULL;

	tiny_args_add_bool(&args, 'a', "abc", &arg1, NULL);
	tu_assert_ptr_ne(args, NULL);
	tu_assert_int_eq(args->short_opt, 'a');
	tu_assert_str_eq(args->long_opt, "abc");

	tu_assert_int_ne(tiny_args_parse(args, fail_size, fail), true);
	tu_assert_int_eq(arg1, false);

	tiny_args_destroy(args);

	return MU_SUCCESS;
}

const char* test_tinyargs_fail_double_dash()
{
	const char* fail[] = {
		"",
		"--"
	};
	const int fail_size = 2;

	struct tiny_args_t* args = NULL;
	tu_assert_int_ne(tiny_args_parse(args, fail_size, fail), true);

	tiny_args_destroy(args);

	return MU_SUCCESS;
}

const char* test_tinyargs_fail_empty_arg()
{
	const char* fail[] = {
		"",
		""
	};
	const int fail_size = 2;

	struct tiny_args_t* args = NULL;
	tu_assert_int_ne(tiny_args_parse(args, fail_size, fail), true);

	tiny_args_destroy(args);

	return MU_SUCCESS;
}

const char* test_tinyargs_fail_single_char_str_param_not_at_end()
{
	const char* fail[] = {
		"",
		"-da",
		"val1"
	};
	const int fail_size = 3;

	struct tiny_args_t* args = NULL;
	tu_assert_int_ne(tiny_args_parse(args, fail_size, fail), true);

	tiny_args_destroy(args);

	return MU_SUCCESS;
}

const char* test_tinyargs_int()
{
	const char* pass[] = {
		"",
		"-d",
		"1234",
		"--ddd",
		"9999"
	};
	const int pass_size = 5;

	int arg1 = 0;
	int arg2 = 0;

	struct tiny_args_t* args = NULL;
	tiny_args_add_int(&args, 'd', "abc", &arg1, NULL);
	tiny_args_add_int(&args, 'a', "ddd", &arg2, NULL);
	tu_assert_int_eq(tiny_args_parse(args, pass_size, pass), true);
	tu_assert_int_eq(arg1, 1234);
	tu_assert_int_eq(arg2, 9999);

	tiny_args_destroy(args);

	return MU_SUCCESS;
}

const char* test_tinyargs_short_opt_too_long()
{
	const char* pass[] = {
		"",
		"-d",
		"1234",
	};
	const int pass_size = 3;

	char arg1[3];
    int arg1_len = 3;

	struct tiny_args_t* args = NULL;
	tiny_args_add_str(&args, 'd', "abc", arg1, arg1_len, NULL);
	tu_assert_int_eq(tiny_args_parse(args, pass_size, pass), false);
}

const char* test_tinyargs_long_opt_too_long()
{
	const char* pass[] = {
		"",
		"--abcd",
		"1234",
	};
	const int pass_size = 3;

	char arg1[3];
    int arg1_len = 3;

	struct tiny_args_t* args = NULL;
	tiny_args_add_str(&args, 'd', "abcd", arg1, arg1_len, NULL);
	tu_assert_int_eq(tiny_args_parse(args, pass_size, pass), false);
}

const char* testsuite_tinyargs()
{
	tu_run_test(test_tinyargs_pass);
	tu_run_test(test_tinyargs_undefined_arg_short_opt);
	tu_run_test(test_tinyargs_undefined_arg_long_opt);
	tu_run_test(test_tinyargs_fail_double_dash);
	tu_run_test(test_tinyargs_fail_empty_arg);
	tu_run_test(test_tinyargs_fail_single_char_str_param_not_at_end);
	tu_run_test(test_tinyargs_int);
	tu_run_test(test_tinyargs_short_opt_too_long);
	tu_run_test(test_tinyargs_long_opt_too_long);

	return MU_SUCCESS;
}

const char* all_tests()
{
	tu_run_suite(testsuite_tinyargs);

	return MU_SUCCESS;
}

int main()
{
	const char* result = all_tests();

	fprintf(stderr, "-----------------------------------\n");
	if(result != MU_SUCCESS)
	{
		fprintf(stderr, "Fail: %s\n", result);
	}
	else
	{
		fprintf(stderr, "All tests passed!\n");
	}
	fprintf(stderr, "Suites run:  %d\n", suites_run);
	fprintf(stderr, "Tests run:   %d\n", tests_run);

	if(result == MU_SUCCESS)
	{
		return MU_SUCCESS;
	}
	else
	{
		return MU_FAIL;
	}
}

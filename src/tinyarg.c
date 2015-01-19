#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>

#include "tinyarg.h"

struct tiny_args_t
{
	char short_opt;
	char* long_opt;
	bool* flag;
	char* str;
	int*  val;
	size_t str_len;
	char* desc;
	struct tiny_args_t* next_arg;
};

int _process_long_form(struct tiny_args_t* args, int argc, const char* argv[], int idx)
{
	const char* flag = (argv[idx]) + 2;

	while(args != NULL)
	{
		if(strcmp(args->long_opt, flag) == 0)
		{
			break;
		}
		args = args->next_arg;
	}

	if(args != NULL && strcmp(args->long_opt, flag) == 0)
	{
		if(args->flag != NULL)
		{
			(*args->flag) = true;
			return idx;
		}
		else if((args->str != NULL || args->val != NULL) && (idx + 1) < argc)
		{
			char valStr[10];
			char* str_ptr = args->str;
			int len = args->str_len;
			if(args->val != NULL)
      {
				str_ptr = valStr;
        len = 10;
      }
			int cpy_len = MIN(strlen(argv[idx+1]), len);
			memcpy(str_ptr, argv[idx+1], cpy_len);
			str_ptr[cpy_len] = '\0';


			if(args->val != NULL)
      {
				*(args->val) = atoi(valStr);
			}
			return (idx + 1);
		}
		else
		{
			fprintf(stderr, "Missing argument to '%s'\n", flag);
			return -1;
		}
	}
	else
	{
		fprintf(stderr, "Unrecognised option '--%s'\n", flag);
		return -1;
	}
}

int _process_short_form(struct tiny_args_t* args, int argc, const char* argv[], int idx)
{
	const char* flag = (argv[idx]) + 1;

	while(*flag != '\0')
	{
		struct tiny_args_t* arg = args;
		while(arg != NULL)
		{
			if(*flag == arg->short_opt)
			{
				break;
			}
			arg = arg->next_arg;
		}

		if(arg == NULL)
		{
			fprintf(stderr, "Unrecognised option '-%c'", *flag);
			return -1;
		}
		else if(arg->flag != NULL)
		{
			(*arg->flag) = true;
		}
		else if((arg->str != NULL || arg->val != NULL) && (idx + 1) < argc)
		{
			if(*(flag + 1) != '\0')
			{
				fprintf(stderr, "Missing argument to '-%c'\n", *flag);
				return -1;
			}

			char valStr[10];
			char* str_ptr = arg->str;
			int len = arg->str_len;
			if(arg->val != NULL)
      {
				str_ptr = valStr;
        len = 10;
      }
			int cpy_len = MIN(strlen(argv[idx+1]), len);
			memcpy(str_ptr, argv[idx+1], cpy_len);
			str_ptr[cpy_len] = '\0';

			if(arg->val != NULL)
      {
				*(arg->val) = atoi(valStr);
			}
			return (idx + 1);
		}
		else
		{
			fprintf(stderr, "Unrecognised option '-%c'", *flag);
			return -1;
		}

		++flag;
	}

	return idx;
}

bool tiny_args_parse(struct tiny_args_t* args, int argc, const char* argv[])
{
	if(args == NULL)
	{
		return false;
	}

	for(int i = 1; i < argc; ++i)
	{
		// Validate. An argument should be of the form:
		//	-abc OR --abc.
		//	
		//	If the argument is preceeded with a single '-' character
		//	then each character is treated as a separate flag.
		if(strlen(argv[i]) < 2)
		{
			fprintf(stderr, "Malformed argument\n");
			return false;
		}

		if(strlen(argv[i]) == 2 && (argv[i][0] != '-' || argv[i][1] == '-'))
		{
			fprintf(stderr, "Malformed argument\n");
			return false;
		}

		if(argv[i][0] == '-' && argv[i][1] == '-')
		{
			//Long form
			i = _process_long_form(args, argc, argv, i);
			if(i < 0)
			{
				return false;
			}
		}
		else
		{
			i = _process_short_form(args, argc, argv, i);
			if(i < 0)
			{
				return false;
			}
		}
	}

	return true;
}

struct tiny_args_t* _create_new_arg(struct tiny_args_t** args)
{
	struct tiny_args_t* arg;
	if(*args == NULL)
	{
		(*args) = malloc(sizeof(struct tiny_args_t));
		arg = (*args);
	}
	else
	{
		arg = (*args);
		while(arg->next_arg != NULL)
		{
			arg = arg->next_arg;
		}
		arg->next_arg = malloc(sizeof(struct tiny_args_t));
		arg = arg->next_arg;
	}

	return arg;
}

void tiny_args_add_bool(struct tiny_args_t** args,
												char short_opt,
												const char* long_opt,
												bool* flag,
												const char* desc)
{
	struct tiny_args_t* arg = _create_new_arg(args);

	//Initialise bool flag to be false;
	*flag = false;

	arg->short_opt = short_opt;
	arg->long_opt = strdup(long_opt);
	arg->flag = flag;
	arg->str = NULL;
	if(desc != NULL)
	{
		arg->desc = strdup(desc);
	}
	else
	{
		arg->desc = NULL;
	}
	arg->next_arg = NULL;
}

void tiny_args_add_int(struct tiny_args_t** args,
                       char short_opt,
                       const char* long_opt,
                       int* val,
                       const char* desc)
{
	struct tiny_args_t* arg = _create_new_arg(args);

	//Initialise bool flag to be zero
	*val = 0;

	arg->short_opt = short_opt;
	arg->long_opt = strdup(long_opt);
	arg->flag = NULL;
	arg->val = val;
	arg->str = NULL;
	if(desc != NULL)
	{
		arg->desc = strdup(desc);
	}
	else
	{
		arg->desc = NULL;
	}
	arg->next_arg = NULL;
}

void tiny_args_add_str(struct tiny_args_t** args,
											 char short_opt,
											 const char* long_opt,
											 char* str,
											 size_t str_len,
											 const char* desc)
{
	struct tiny_args_t* arg = _create_new_arg(args);

	//Initialise chararray to be empty
	*str = '\0';

	arg->short_opt = short_opt;
	arg->long_opt = strdup(long_opt);
	arg->flag = NULL;
	arg->str = str;
	arg->str_len = str_len;
	if(desc != NULL)
	{
		arg->desc = strdup(desc);
	}
	else
	{
		arg->desc = NULL;
	}
	arg->next_arg = NULL;
}

void tiny_args_usage(const char* process_name, struct tiny_args_t* args)
{
	fprintf(stderr, "\nUsage: %s [OPTION]\n", process_name);
	while(args != NULL)
	{
		fprintf(stderr, "-%c, --%s %s\n", args->short_opt, args->long_opt, args->str != NULL ? "<value>" : "");
		if(args->desc != NULL)
		{
			fprintf(stderr, "    %s\n", args->desc);
		}
		args = args->next_arg;
	}
}

void tiny_args_destroy(struct tiny_args_t* args)
{
	while(args != NULL)
	{
		struct tiny_args_t* tmp = args;
		free(tmp->long_opt);
		free(tmp->desc);
		args = tmp->next_arg;
		free(tmp);
	}
}


#ifndef __TINYARG_H__
#define __TINYARG_H__

#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct  tiny_args_t;

bool tiny_args_parse(struct tiny_args_t* args, int argc, const char* argv[]);

void tiny_args_add_bool(struct tiny_args_t** args,
                        char short_opt,
                        const char* long_opt,
                        bool* flag,
                        const char* desc);

void tiny_args_add_str(struct tiny_args_t** args,
                       char short_opt,
                       const char* long_opt,
                       char* str,
                       size_t str_len,
                       const char* desc);

void tiny_args_add_int(struct tiny_args_t** args,
                       char short_opt,
                       const char* long_opt,
                       int* val,
                       const char* desc);

void tiny_args_usage(const char* process_name, struct tiny_args_t* args);

void tiny_args_destroy(struct tiny_args_t* args);

#ifdef __cplusplus
}
#endif

#endif /* __TINYARG_H__ */

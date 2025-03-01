#ifndef S21_GREP
#define S21_GREP

#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int e, i, v, c, l, n, h, s, f, o;
} flag;

void parser(flag* flg, int opt, char** pattern);
void output(flag* flg, int argc, char* argv[], regex_t* re);
void no_flags(flag* flg, FILE* file, int count_files, char* argv[],
              regex_t* re);
void output_last_enter(char* lineptr, int count_printed_line);
void func_flg_c(flag* flg, FILE* file, int count_files, char* argv[],
                regex_t* re);
void func_flg_l(flag* flg, FILE* file, char* argv[], regex_t* re);
void func_flg_v(flag* flg, FILE* file, int count_files, char* argv[],
                regex_t* re);
void func_flg_o(flag* flg, FILE* file, int count_files, char* argv[],
                regex_t* re);
void add_pattern(char** pattern, const char* str);
void add_file_pattern(char** pattern, const char* str);

#endif
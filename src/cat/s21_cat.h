#ifndef S21_CAT
#define S21_CAT

#include <stdio.h>

typedef struct {
  int b, e, n, s, t, v;
} flag;

void parser(flag* flg, int opt);
void output(FILE* file, int* enter_count, int* line_count, flag* flg,
            char* prev);
void func_cat(flag* flg, int argc, char* argv[]);
void flag_v(int c);

#endif
#include "s21_cat.h"

#include <getopt.h>

int main(int argc, char* argv[]) {
  char short_opt[] = "beEnstTv";
  int opt;
  flag flg = {0};
  struct option long_opt[] = {{"number-nonblank", no_argument, NULL, 'b'},
                              {"number", no_argument, NULL, 'n'},
                              {"squeeze-blank", no_argument, NULL, 's'},
                              {0, 0, 0, 0}};
  while ((opt = (getopt_long(argc, argv, short_opt, long_opt, 0))) != -1) {
    parser(&flg, opt);
  }
  func_cat(&flg, argc, argv);

  return 0;
}

void parser(flag* flg, int opt) {
  switch (opt) {
    case 'n':
      flg->n = 1;
      break;
    case 'b':
      flg->b = 1;
      break;
    case 'E':
      flg->e = 1;
      break;
    case 'e':
      flg->e = 1;
      flg->v = 1;
      break;
    case 'T':
      flg->t = 1;
      break;
    case 'v':
      flg->v = 1;
      break;
    case 't':
      flg->t = 1;
      flg->v = 1;
      break;
    case 's':
      flg->s = 1;
      break;
  }
}

void func_cat(flag* flg, int argc, char* argv[]) {
  int line_count = 1;
  int enter_count = 1;
  char prev = '\n';
  for (; optind < argc; optind++) {
    FILE* file = fopen(argv[optind], "r");
    if (file != NULL) {
      output(file, &enter_count, &line_count, flg, &prev);
      fclose(file);
    } else {
      fprintf(stderr, "No such file or directory\n");
    }
  }
}

void output(FILE* file, int* enter_count, int* line_count, flag* flg,
            char* prev) {
  int symb;
  while ((symb = fgetc(file)) != EOF) {
    if (symb == '\n') {
      (*enter_count)++;
    } else {
      (*enter_count) = 0;
    }
    if (flg->s == 1 && *enter_count > 2) {
      continue;
    }
    if (flg->n == 1 && flg->b != 1 && *prev == '\n') {
      fprintf(stdout, "%6d\t", (*line_count)++);
    }
    if (flg->b == 1 && *prev == '\n' && symb != '\n') {
      fprintf(stdout, "%6d\t", (*line_count)++);
    }
    if (flg->e == 1 && symb == '\n') {
      fprintf(stdout, "$");
    }
    if (flg->t == 1 && symb == '\t') {
      fprintf(stdout, "^I");
    } else if (flg->v == 1) {
      flag_v(symb);
    } else {
      putchar(symb);
    }
    *prev = symb;
  }
}

void flag_v(int c) {
  if (c == 9 || c == 10) {
    printf("%c", c);
  } else if (c >= 32 && c < 127) {
    printf("%c", c);
  } else if (c == 127) {  // это знак delete
    printf("^?");
  } else if (c >= 128 + 32) {  //>160
    printf("M-");
    (c < 250) ? printf("%c", c - 128) : printf("^?");
  } else {
    (c > 32) ? printf("M-^%c", c - 64) : printf("^%c", c + 64);
  }
}
#define _GNU_SOURCE
#define opts "e:ivclnhsf:o"
#include "s21_grep.h"

int main(int argc, char* argv[]) {
  int error = 0;
  if (argc < 3) {
    fprintf(stderr, "Использование: grep <pattern> <file>\n");
    error = 1;
  }
  if (!error) {
    int opt;
    flag flg = {0};
    char* pattern = NULL;
    while ((opt = (getopt(argc, argv, opts))) != -1) {
      parser(&flg, opt, &pattern);
    }
    if (pattern == NULL) {
      add_pattern(&pattern, argv[optind++]);
    }
    regex_t re;
    flg.i ? regcomp(&re, pattern, REG_ICASE | REG_EXTENDED)
          : regcomp(&re, pattern, REG_EXTENDED);
    output(&flg, argc, argv, &re);
    regfree(&re);
    free(pattern);
  }
  return 0;
}

void parser(flag* flg, int opt, char** pattern) {
  switch (opt) {
    case 'e':
      flg->e = 1;
      add_pattern(pattern, optarg);
      break;
    case 'f':
      flg->f = 1;
      add_file_pattern(pattern, optarg);
      break;
    case 'i':
      flg->i |= REG_ICASE;
      break;
    case 's':
      flg->s = 1;
      break;
    case 'v':
      flg->v = 1;
      break;
    case 'n':
      flg->n = 1;
      break;
    case 'h':
      flg->h = 1;
      break;
    case 'o':
      flg->o = 1;
      break;
    case 'l':
      flg->l = 1;
      break;
    case 'c':
      flg->c = 1;
      break;
  }
}

void add_pattern(char** pattern, const char* str) {
  if (*pattern == NULL) {
    *pattern = calloc(1, (strlen(str) + 1));
    strcat(*pattern, str);
  } else {
    if (strlen(*pattern) > 0) {
      *pattern = realloc(*pattern,
                         (sizeof(char) * (strlen(*pattern) + strlen(str) + 3)));
      strcat(*pattern, "|");
      strcat(*pattern, str);
    }
  }
}
void add_file_pattern(char** pattern, const char* str) {
  char* line = NULL;
  size_t len = 0;
  FILE* file = fopen(str, "r");
  if (file != NULL) {
    while (getline(&line, &len, file) != -1) {
      size_t newline_pos = strcspn(line, "\n");
      line[newline_pos] = '\0';
      add_pattern(pattern, line);
    }
    fclose(file);
  } else {
    fprintf(stderr, "No such file or directory\n");
  }
  free(line);
}

void output(flag* flg, int argc, char* argv[], regex_t* re) {
  int count_files = 0;
  if ((argc - optind) > 1) {
    count_files = 1;
  }
  for (; optind < argc; optind++) {
    FILE* file = fopen(argv[optind], "r");
    if (file != NULL) {
      if (flg->l)
        func_flg_l(flg, file, argv, re);
      else if (flg->v && !flg->c && !flg->o)
        func_flg_v(flg, file, count_files, argv, re);
      else if (flg->o && !flg->v && !flg->c)
        func_flg_o(flg, file, count_files, argv, re);
      else {
        if (flg->c) func_flg_c(flg, file, count_files, argv, re);
        if ((!flg->s || !flg->c || flg->i || flg->h || flg->n) &&
            (!flg->o && !flg->v))
          no_flags(flg, file, count_files, argv, re);
      }
      fclose(file);
    } else if (!flg->s) {
      fprintf(stderr, "No such file or directory\n");
    }
  }
}

void no_flags(flag* flg, FILE* file, int count_files, char* argv[],
              regex_t* re) {
  char* lineptr = NULL;
  size_t len = 0;
  int count_printed_line = 0;
  int number_line = 0;
  while ((getline(&lineptr, &len, file)) != -1) {
    number_line++;
    if (!regexec(re, lineptr, 0, 0, 0)) {
      count_printed_line++;
      if (!count_files || flg->h) {
        flg->n ? printf("%d:%s", number_line, lineptr) : printf("%s", lineptr);
      } else {
        flg->n ? printf("%s:%d:%s", argv[optind], number_line, lineptr)
               : printf("%s:%s", argv[optind], lineptr);
      }
      output_last_enter(lineptr, count_printed_line);
    }
  }
  free(lineptr);
}

void output_last_enter(char* lineptr, int count_printed_line) {
  char last_char_str = 'l';
  char* new_line = lineptr;
  while (*new_line != '\0') {
    last_char_str = *new_line;
    new_line++;
  }
  if (last_char_str != '\n' && count_printed_line > 0) {
    printf("\n");
  }
}

void func_flg_c(flag* flg, FILE* file, int count_files, char* argv[],
                regex_t* re) {
  char* lineptr_c = NULL;
  size_t len_c = 0;
  int count_finds_c = 0;
  while ((getline(&lineptr_c, &len_c, file)) != -1) {
    size_t newline_pos = strcspn(lineptr_c, "\n");
    lineptr_c[newline_pos] = '\0';
    if (flg->v ? regexec(re, lineptr_c, 0, 0, 0)
               : regexec(re, lineptr_c, 0, 0, 0) == 0) {
      count_finds_c++;
    }
  }
  if (!count_files || flg->h) {
    printf("%d\n", count_finds_c);
  } else {
    printf("%s:%d\n", argv[optind], count_finds_c);
  }
  free(lineptr_c);
}

void func_flg_l(flag* flg, FILE* file, char* argv[], regex_t* re) {
  char* lineptr_l = NULL;
  size_t len_l = 0;
  int count_files_l = 0;
  while ((getline(&lineptr_l, &len_l, file)) != -1) {
    if (flg->v ? regexec(re, lineptr_l, 0, 0, 0) == REG_NOMATCH
               : regexec(re, lineptr_l, 0, 0, 0) == 0) {
      count_files_l++;
    }
  }
  if (count_files_l) {
    printf("%s\n", argv[optind]);
  }
  free(lineptr_l);
}

void func_flg_v(flag* flg, FILE* file, int count_files, char* argv[],
                regex_t* re) {
  char* lineptr_v = NULL;
  size_t len_v = 0;
  int count_printed_line_v = 0;
  int number_line = 0;
  while ((getline(&lineptr_v, &len_v, file)) != -1) {
    number_line++;
    count_printed_line_v++;
    if (regexec(re, lineptr_v, 0, 0, 0) == REG_NOMATCH) {
      if (!count_files || flg->h) {
        if (regexec(re, lineptr_v, 0, 0, 0)) {
          flg->n ? printf("%d:%s", number_line, lineptr_v)
                 : printf("%s", lineptr_v);
        }
      } else {
        flg->n ? printf("%s:%d:%s", argv[optind], number_line, lineptr_v)
               : printf("%s:%s", argv[optind], lineptr_v);
      }
      output_last_enter(lineptr_v, count_printed_line_v);
    }
  }
  free(lineptr_v);
}

void func_flg_o(flag* flg, FILE* file, int count_files, char* argv[],
                regex_t* re) {
  char* lineptr_o = NULL;
  size_t len_o = 0;
  int number_line = 0;
  while ((getline(&lineptr_o, &len_o, file)) != -1) {
    number_line++;
    regmatch_t pmatch[1];
    regoff_t len;
    char* line = lineptr_o;
    while (!regexec(re, line, 1, pmatch, 0)) {
      len = pmatch[0].rm_eo - pmatch[0].rm_so;
      if (!count_files || flg->h) {
        flg->n
            ? printf("%d:%.*s\n", number_line, (int)len, line + pmatch[0].rm_so)
            : printf("%.*s\n", (int)len, line + pmatch[0].rm_so);
      } else {
        flg->n ? printf("%s:%d:%.*s\n", argv[optind], number_line, (int)len,
                        line + pmatch[0].rm_so)
               : printf("%s:%.*s\n", argv[optind], (int)len,
                        line + pmatch[0].rm_so);
      }
      line += pmatch[0].rm_eo;
    }
  }
  free(lineptr_o);
}

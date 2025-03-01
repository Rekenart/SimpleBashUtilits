CC=gcc
CFLAGS=-Wall -Werror -Wextra -std=c11

all: s21_grep

s21_grep: s21_grep.o
	$(CC) $(FLAGS) -o s21_grep s21_grep.o

style-check:
	cp ../../materials/linters/.clang-format .clang-format
	clang-format -n s21_grep.c s21_grep.h

style-fix:
	cp ../../materials/linters/.clang-format .clang-format
	clang-format -i s21_grep.c s21_grep.h

valgrind: 
	bash test_leaks.sh

cppcheck: 
	cppcheck --suppres=missingIncludeSystem --suppress=style s21_grep.c

test:
	bash tests.sh

clean:
	rm -rf *.o s21_grep .clang-format *.log

rebuild: clean all
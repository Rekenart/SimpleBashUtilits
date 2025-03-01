CC=gcc
CFLAGS=-Wall -Werror -Wextra -std=c11

all: s21_cat

s21_cat: s21_cat.o
	$(CC) $(FLAGS) s21_cat.o -o s21_cat

style-check:
	cp ../../materials/linters/.clang-format .clang-format
	clang-format -n s21_cat.c s21_cat.h

style-fix:
	cp ../../materials/linters/.clang-format .clang-format
	clang-format -i s21_cat.c s21_cat.h

valgrind: 
	bash test_leaks.sh

cppcheck: 
	cppcheck --suppres=missingIncludeSystem --suppress=style s21_cat.c

test:
	bash tests.sh

clean:
	rm -rf *.o  s21_cat .clang-format *.log

rebuild: clean all
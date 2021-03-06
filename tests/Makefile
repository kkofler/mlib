#NOTE: GMP is only used for the tests
CC=gcc -std=c99
CFLAGS=-Wall -W -pedantic -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes -Wswitch-default -Wswitch-enum -Wcast-align -Wpointer-arith -Wbad-function-cast -Wstrict-overflow=5 -Wstrict-prototypes -Winline -Wundef -Wnested-externs -Wcast-qual -Wshadow -Wunreachable-code -Wlogical-op -Wstrict-aliasing=2 -Wredundant-decls -Wold-style-definition -Wno-unused-function -DSTART_COVERAGE="" -DEND_COVERAGE=""
CPPFLAGS=-I..
RM=rm -f
VALGRIND=
EXE=./a.exe

all:
	@echo "Nothing to be done."

check: core mutex array list i-list string buffer shared tuple snapshot algo dict rbtree variant

valgrind:
	make check VALGRIND=valgrind

core:
	$(CC) $(CFLAGS) $(CPPFLAGS) -g $(COVERAGE_CFLAGS) test-mcore.c -lgmp -o $(EXE) && $(VALGRIND) $(EXE)

mutex:
	$(CC) $(CFLAGS) $(CPPFLAGS) -g $(COVERAGE_CFLAGS) test-mmutex.c -lpthread -o $(EXE) && $(VALGRIND) $(EXE)

array:
	$(CC) $(CFLAGS) $(CPPFLAGS) -g $(COVERAGE_CFLAGS) test-marray.c -lgmp -o $(EXE) && $(VALGRIND) $(EXE)

list:
	$(CC) $(CFLAGS) $(CPPFLAGS) -g $(COVERAGE_CFLAGS) test-mlist.c -lgmp -o $(EXE) && $(VALGRIND) $(EXE)

i-list:
	$(CC) $(CFLAGS) $(CPPFLAGS) -g $(COVERAGE_CFLAGS) test-milist.c -lgmp -o $(EXE) && $(VALGRIND) $(EXE)

string:
	$(CC) $(CFLAGS) $(CPPFLAGS) -g $(COVERAGE_CFLAGS) test-mstring.c -lgmp -o $(EXE) && $(VALGRIND) $(EXE)

buffer:
	$(CC) $(CFLAGS) $(CPPFLAGS) -g $(COVERAGE_CFLAGS) test-mbuffer.c -lpthread -o $(EXE) && $(VALGRIND) $(EXE)

shared:
	$(CC) $(CFLAGS) $(CPPFLAGS) -g $(COVERAGE_CFLAGS) test-mshared.c -lpthread -o $(EXE) && $(VALGRIND) $(EXE)

algo:
	$(CC) $(CFLAGS) $(CPPFLAGS) -g $(COVERAGE_CFLAGS) test-malgo.c -lgmp -o $(EXE) && $(VALGRIND) $(EXE)

tuple:
	$(CC) $(CFLAGS) $(CPPFLAGS) -g $(COVERAGE_CFLAGS) test-mtuple.c -lgmp -o $(EXE) && $(VALGRIND) $(EXE)

snapshot:
	$(CC) $(CFLAGS) $(CPPFLAGS) -g $(COVERAGE_CFLAGS) test-msnapshot.c -lgmp -o $(EXE) && $(VALGRIND) $(EXE)

dict:
	$(CC) $(CFLAGS) $(CPPFLAGS) -g $(COVERAGE_CFLAGS) test-mdict.c -lgmp -o $(EXE) && $(VALGRIND) $(EXE)

rbtree:
	$(CC) $(CFLAGS) $(CPPFLAGS) -g $(COVERAGE_CFLAGS) test-mrbtree.c -lgmp -o $(EXE) && $(VALGRIND) $(EXE)

bitset:
	$(CC) $(CFLAGS) $(CPPFLAGS) -g $(COVERAGE_CFLAGS) test-mbitset.c -lgmp -o $(EXE) && $(VALGRIND) $(EXE)

variant:
	$(CC) $(CFLAGS) $(CPPFLAGS) -g $(COVERAGE_CFLAGS) test-mvariant.c -lgmp -o $(EXE) && $(VALGRIND) $(EXE)

gene-list:
	$(CC) $(CFLAGS) $(CPPFLAGS) -O3 -DNDEBUG -S tgen-mlist.c && less tgen-mlist.s

gene-array:
	$(CC) $(CFLAGS) $(CPPFLAGS) -O3 -DNDEBUG -S tgen-marray.c && less tgen-marray.s

gene-shared:
	$(CC) $(CFLAGS) $(CPPFLAGS) -O3 -DNDEBUG -S tgen-shared.c && less tgen-shared.s

clean:
	$(RM) $(EXE) *.s *~ *.o ./a.dat
	$(RM) -rf coverage *.gcda *.gcno *.gcov all.info test.tar coverage-dir

coverage-dir:
	mkdir -p coverage-dir

coverage-dir/%.c: %.c coverage-dir
	awk '/START_COVERAGE/ { printit=1} (printit == 0) { print }' $< > coverage-dir/first.c
	awk '  printit { print } /END_COVERAGE/ { printit=1}' $< > coverage-dir/end.c
	$(CC) -E -std=c99 -I.. -DNDEBUG $< |grep -v '^#' |awk ' /END_COVERAGE/ { printit=0} printit { print } /START_COVERAGE/ { printit=1 }' > coverage-dir/middle.c
	cat coverage-dir/first.c coverage-dir/middle.c coverage-dir/end.c > coverage-dir/$<

coverage: coverage-dir
	for i in test-m*.c ; do echo $$i ; make coverage-dir/$$i && $(CC) -ftest-coverage -fprofile-arcs -g -I.. coverage-dir/$$i -o $(EXE) -lgmp -pthread && $(EXE) ; done
	for i in test-m*.c ; do gcov coverage-dir/$$i ; done

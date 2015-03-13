
CC=gcc
CFLAGS=-c -Wall -I./
SOURCES=$(wildcard src/*.c)
OBJECTS=$(patsubst src/%.c,build/%.o,$(SOURCES))
HEADERS=$(wildcard src/*.h)
LIB=jinks
HEADERS+=$(LIB).h
TESTFLAGS=$(CFLAGS) -g -DJX_TESTING
TEST_OBJS=$(patsubst src/%.c,build/%_test.o,$(SOURCES))
TEST_OBJS+=build/unit_test_defs.o
RELFLAGS=$(CFLAGS) -O3 -DNDEBUG

.PHONY=clean test debug lib

test : $(TEST_OBJS) build/$(LIB)_test
	clear  #this is a cheat to start the testing with a clean screen
	-valgrind --leak-check=full --show-leak-kinds=all build/$(LIB)_test

debug: $(TEST_OBJS) build$(LIB)_test
	gdb -tui build/$(LIB)_test

lib: build/$(LIB).a
	cp build/$(LIB).a ./$(LIB).a

build/$(LIB).a : $(OBJECTS)
	if [ ! -d ./build ]; then mkdir ./build; fi
	ar -r build/$(LIB).a $(OBJECTS)

build/%.o : src/%.c $(HEADERS)
	if [ ! -d ./build ]; then mkdir ./build; fi
	$(CC) $(RELFLAGS) -o $@ $<

build/unit_test_defs.o : $(SOURCES)
	if [ ! -d ./build ]; then mkdir ./build; fi
	sed -n 's/^jx_test \(.*\)().*$$/\1/p' $(SOURCES) | sort | uniq > unit_tests.tmp
	echo '#include "jx_private.h"' > build/unit_test_defs.c
	sed -n 's/.*/jx_test &();/p' unit_tests.tmp >> build/unit_test_defs.c
	echo "" >> build/unit_test_defs.c
	echo "const struct unit_test all_tests[] = {" >> build/unit_test_defs.c
	sed -n 's/.*/  \{ \"&\", & \},/p' unit_tests.tmp >> build/unit_test_defs.c
	echo "};" >> build/unit_test_defs.c
	echo "" >> build/unit_test_defs.c
	echo "const int num_of_tests = sizeof(all_tests) / sizeof(struct unit_test);" >> build/unit_test_defs.c
	rm unit_tests.tmp
	$(CC) $(TESTFLAGS) -I./src -o build/unit_test_defs.o build/unit_test_defs.c

clean: 
	if [ -d ./build ]; then rm -r ./build; fi
	if [ -e ./$(LIB).a ]; then rm ./$(LIB).a; fi

build/%_test.o : src/%.c $(HEADERS)
	if [ ! -d ./build ]; then mkdir ./build; fi
	$(CC) $(TESTFLAGS) -o $@ $<

build/$(LIB)_test : $(TEST_OBJS)
	if [ ! -d ./build ]; then mkdir ./build; fi
	gcc -o build/$(LIB)_test $(TEST_OBJS)



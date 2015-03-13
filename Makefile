
LIB=jinks
LIBFILE=build/lib$(LIB).a
DEBUGLIB=$(LIB)-debug
DEBUGLIBFILE=build/lib$(DEBUGLIB).a

TESTHARNESS=build/$(LIB)_test

CC=gcc
CPP=g++
CPPFLAGS=-c -Wall -I./ -g
CFLAGS=-c -Wall -I./
DEBUGFLAGS=$(CFLAGS) -g
RELFLAGS=$(CFLAGS) -O3 -DNDEBUG

SOURCES=$(wildcard src/*.c)
TESTS=$(wildcard tests/*.cc)
TESTOBJS=$(patsubst tests/%.cc,build/%_test.o,$(TESTS))
OBJECTS=$(patsubst src/%.c,build/%.o,$(SOURCES))
DEBUGOBJS=$(patsubst src/%.c,build/%_debug.o,$(SOURCES))
HEADERS=$(wildcard src/*.h)

HEADERS+=$(LIB).h

.PHONY=clean test test-mem debug lib lib-debug

all : lib lib-debug

test : $(TESTHARNESS)
	clear
	$(TESTHARNESS)

test-mem : $(TESTHARNESS)
	clear  #this is a cheat to start the testing with a clean screen
	-valgrind --leak-check=full --show-leak-kinds=all $(TESTHARNESS)

debug : $(TESTHARNESS)
	gdb -tui $(TESTHARNESS)

lib : $(LIBFILE)
	cp $(LIBFILE) ./

lib-debug: $(DEBUGLIBFILE)
	cp $(DEBUGLIBFILE) ./

$(LIBFILE) : $(OBJECTS)
	ar -r $(LIBFILE) $(OBJECTS)

$(DEBUGLIBFILE) : $(DEBUGOBJS)
	ar -r $(DEBUGLIBFILE) $(DEBUGOBJS)

$(TESTHARNESS) : $(DEBUGLIBFILE) $(TESTOBJS)
	$(CPP) -o $@ $(TESTOBJS) -Lbuild -l$(DEBUGLIB) -lgtest -lgtest_main -lpthread

build/%.o : src/%.c $(HEADERS)
	@mkdir -p $(@D)
	$(CC) $(RELFLAGS) -o $@ $<

build/%_debug.o : src/%.c $(HEADERS)
	@mkdir -p $(@D)
	$(CC) $(DEBUGFLAGS) -o $@ $<

build/%_test.o : tests/%.cc $(HEADERS)
	@mkdir -p $(@D)
	$(CPP) $(CPPFLAGS) -o $@ $<

clean: 
	-rm -r ./build
	-rm *.a





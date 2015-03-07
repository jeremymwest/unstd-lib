
CC=gcc
CFLAGS=-c -Wall
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
LIB=jinks
TESTFLAGS=$(CFLAGS) -g -DJX_TESTING
TEST_OBJS=$(SOURCES:.c=_test.o)

.PHONY=clean test debug

test : $(TEST_OBJS) $(LIB)_test
	-valgrind --leak-check=full --show-leak-kinds=all ./$(LIB)_test

debug: $(TEST_OBJS) $(LIB)_test
	gdb -tui ./$(LIB)_test

$(LIB).a : $(OBJECTS)
	ar -r $(LIB).a $(OBJECTS)

%.o : %.c %.h $(LIB).h
	$(CC) $(CFLAGS) -o $@ $<

list_of_tests.h : $(SOURCES)
	sed -n 's/^jx_test \(.*\)().*$$/\1/p' $(SOURCES) | sort | uniq > unit_tests.tmp
	sed -n 's/.*/jx_test &();/p' unit_tests.tmp > list_of_tests.h
	echo "" >> list_of_tests.h
	echo "static const struct unit_test all_tests[] = {" >> list_of_tests.h
	sed -n 's/.*/  \{ \"&\", & \},/p' unit_tests.tmp >> list_of_tests.h
	echo "};" >> list_of_tests.h
	echo "" >> list_of_tests.h
	rm unit_tests.tmp

clean: 
	-rm *.o *.a list_of_tests.h *_test*

%_test.o : %.c %.h $(LIB).h list_of_tests.h
	$(CC) $(TESTFLAGS) -o $@ $<

$(LIB)_test : $(TEST_OBJS)
	gcc -o $(LIB)_test $(TEST_OBJS)




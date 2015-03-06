
CC=gcc
CFLAGS=-c -Wall
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
LIB=jinks
TESTFLAGS=$(CFLAGS) -DJX_TESTING
TEST_OBJS=$(SOURCES:.c=_test.o)

.PHONY=clean test

test : $(TEST_OBJS) $(LIB)_test
	-./$(LIB)_test

$(LIB).a : $(OBJECTS)
	ar -r $(LIB).a $(OBJECTS)

%.o : %.c %.h $(LIB).h
	$(CC) $(CFLAGS) -o $@ $<

test_list.in : $(SOURCES)
	sed -n 's/^jx_test \(.*\)().*$$/\1/p' $(SOURCES) | sort | uniq > unit_tests.tmp
	sed -n 's/.*/jx_test &();/p' unit_tests.tmp > test_list.in
	echo "" >> test_list.in
	echo "static const struct unit_test all_tests[] = {" >> test_list.in
	sed -n 's/.*/  \{ \"&\", & \},/p' unit_tests.tmp >> test_list.in
	echo "};" >> test_list.in
	echo "" >> test_list.in
	rm unit_tests.tmp

clean: 
	-rm *.o *.a *.in *_test*

%_test.o : %.c %.h $(LIB).h test_list.in
	$(CC) $(TESTFLAGS) -o $@ $<

$(LIB)_test : $(TEST_OBJS)
	gcc -o $(LIB)_test $(TEST_OBJS)





CC=gcc
CFLAGS=-c -Wall
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
LIB=jinks
TESTFLAGS=$(CFLAGS) -DJX_TESTING
TEST_OBJS=$(SOURCES:.c=_test.o)

.PHONY=clean test

$(LIB).a : $(OBJECTS)
	ar -r $(LIB).a $(OBJECTS)

%.o : %.c %.h $(LIB).h
	$(CC) $(CFLAGS) -o $@ $<

test_list.in : $(SOURCES)
	grep 'bool jx_test_.*()' *.c | sed 's/^.*bool \(jx_test_.*\)().*$$/bool \1()\;/' > test_list.in
	echo "" >> test_list.in
	echo "static const struct unit_test all_tests[] = {" >> test_list.in
	grep 'bool jx_test_.*()' *.c | sed 's/^.*bool \(jx_test_.*\)().*$$/  \{ \"\1\", \1 \},/' >> test_list.in
	echo "};" >> test_list.in
	echo "" >> test_list.in

clean: 
	-rm *.o *.a *.in *_test*

%_test.o : %.c %.h $(LIB).h test_list.in
	$(CC) $(TESTFLAGS) -o $@ $<

$(LIB)_test : $(TEST_OBJS)
	gcc -o $(LIB)_test $(TEST_OBJS)

test : $(TEST_OBJS) $(LIB)_test
	./$(LIB)_test



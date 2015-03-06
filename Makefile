
CC=gcc
CFLAGS=-c -Wall
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
LIB=jinks

.PHONY=clean

$(LIB).a : $(OBJECTS)
	ar -r $(LIB).a $(OBJECTS)

%.o : %.c %.h $(LIB).h
	$(CC) $(CFLAGS) -o $@ $<

clean: 
	-rm *.o *.a

CFLAGS = -Wall
PPOS_OBJECTS = ppos_core.o queue.o
TEST_OBJETCS = $(PPOS_OBJECTS) test.o

all: test

debug: CFLAGS += -DDEBUG -g 
debug: test

test: $(TEST_OBJETCS)
	gcc $(TEST_OBJETCS) -o test $(CFLAGS)


test.o: test.c
	gcc -c test.c -o test.o $(CFLAGS)

ppos_core.o: ppos_core.c ppos_data.h ppos.h
	gcc -c ppos_core.c $(CFLAGS)

queue.o: queue.c queue.h
	gcc -c queue.c -o queue.o $(CFLAGS)

clean: 
		rm -f *.o test

purge:  clean
		rm -f test
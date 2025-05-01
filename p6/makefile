CFLAGS = -Wall
PPOS_OBJECTS = ppos_core.o queue.o
TEST1_OBJECTS = $(PPOS_OBJECTS) test1.o
TEST2_OBJECTS = $(PPOS_OBJECTS) test2.o

all: test1 test2

debug: CFLAGS += -DDEBUG -g 
debug: test1 test2

test1: $(TEST1_OBJECTS)
	gcc $(TEST1_OBJECTS) -o test1 $(CFLAGS)

test2: $(TEST2_OBJECTS)
	gcc $(TEST2_OBJECTS) -o test2 $(CFLAGS)

test1.o: test1.c
	gcc -c test1.c -o test1.o $(CFLAGS)

test2.o: test2.c
	gcc -c test2.c -o test2.o $(CFLAGS)

ppos_core.o: ppos_core.c ppos_data.h ppos.h
	gcc -c ppos_core.c $(CFLAGS)

queue.o: queue.c queue.h
	gcc -c queue.c -o queue.o $(CFLAGS)

clean: 
		rm -f *.o test1 test2

purge:  clean
		rm -f test1 test2
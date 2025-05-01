CFLAGS = -Wall
PPOS_OBJECTS = ppos_core.o queue.o
TEST_OBJECTS = $(PPOS_OBJECTS) test.o
STRESS_TEST_OBJECTS = $(PPOS_OBJECTS) stress_test.o

all: test stress_test

debug: CFLAGS += -DDEBUG -g 
debug: test stress_test

test: $(TEST_OBJECTS)
	gcc $(TEST_OBJECTS) -o test $(CFLAGS)

stress_test: $(STRESS_TEST_OBJECTS)
	gcc $(STRESS_TEST_OBJECTS) -o stress_test $(CFLAGS)

test.o: test.c
	gcc -c test.c -o test.o $(CFLAGS)

stress_test.o: stress_test.c
	gcc -c stress_test.c -o stress_test.o $(CFLAGS)

ppos_core.o: ppos_core.c ppos_data.h ppos.h
	gcc -c ppos_core.c $(CFLAGS)

queue.o: queue.c queue.h
	gcc -c queue.c -o queue.o $(CFLAGS)

clean: 
		rm -f *.o test stress_test

purge:  clean
		rm -f test stress_test
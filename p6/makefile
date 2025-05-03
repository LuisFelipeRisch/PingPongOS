CFLAGS = -Wall
PPOS_OBJECTS = ppos_core.o queue.o
PPOS_OBJECTS_MAIN = $(PPOS_OBJECTS) main.o
PPOS_OBJECTS_MAIN2 = $(PPOS_OBJECTS) main2.o

all: ppos ppos2

debug: CFLAGS += -DDEBUG -g 
debug: ppos ppos2

ppos: $(PPOS_OBJECTS_MAIN)
	gcc $(PPOS_OBJECTS_MAIN) -o ppos $(CFLAGS)

ppos2: $(PPOS_OBJECTS_MAIN2)
	gcc $(PPOS_OBJECTS_MAIN2) -o ppos2 $(CFLAGS)

main.o: main.c
	gcc -c main.c -o main.o $(CFLAGS)

main2.o: main2.c
	gcc -c main2.c -o main2.o $(CFLAGS)

ppos_core.o: ppos_core.c ppos_data.h ppos.h
	gcc -c ppos_core.c $(CFLAGS)

queue.o: queue.c queue.h
	gcc -c queue.c -o queue.o $(CFLAGS)

clean: 
	rm -f *.o ppos ppos2

purge: clean
	rm -f ppos ppos2

CFLAGS = -Wall
PPOS_OBJECTS = ppos_core.o queue.o
MAIN_OBJECTS = $(PPOS_OBJECTS) main.o

all: ppos

debug: CFLAGS += -DDEBUG -g
debug: ppos

ppos: $(MAIN_OBJECTS)
	gcc $(MAIN_OBJECTS) -o ppos $(CFLAGS)

main.o: main.c
	gcc -c main.c -o main.o $(CFLAGS)

ppos_core.o: ppos_core.c ppos_data.h ppos.h
	gcc -c ppos_core.c -o ppos_core.o $(CFLAGS)

queue.o: queue.c queue.h
	gcc -c queue.c -o queue.o $(CFLAGS)

clean:
	rm -f *.o

purge: clean
	rm -f ppos

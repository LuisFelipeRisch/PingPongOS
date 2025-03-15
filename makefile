CC = gcc
CFLAGS = -Wall -Wextra -std=c99

SRC = main.c src/queue/queue.c
OBJ = $(SRC:.c=.o)

TARGET = queue_program

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean

FILENAME = myShell

CC = gcc
CFLAGS = -g

default: all

all: $(FILENAME)

${FILENAME}: $(FILENAME).c
	$(CC) $(CFLAGS) -o $(FILENAME) $(FILENAME).c

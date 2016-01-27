
CC = gcc
CFLAGS = -g -ggdb -Wall

all: aptback

aptback: main.o log_parse.o debug.o
	$(CC) $(CFLAGS) -o main.exe main.o log_parse.o debug.o
	
main.o: main.c 
	$(CC) $(CFLAGS) -c main.c
	
log_parse.o: log_parse.c
	$(CC) $(CFLAGS) -c log_parse.c
	
debug.o: debug.c
	$(CC) $(CFLAGS) -c debug.c

	
clean: 
	rm main.exe main.o log_parse.o debug.o
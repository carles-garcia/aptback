
CC = gcc
CFLAGS = -g -ggdb -Wall

all: aptback

aptback: main.o log_parse.o debug.o selection.o mem.o
	$(CC) $(CFLAGS) -o main.exe main.o log_parse.o debug.o selection.o mem.o
main.o: main.c 
	$(CC) $(CFLAGS) -c main.c
	
log_parse.o: log_parse.c
	$(CC) $(CFLAGS) -c log_parse.c
	
debug.o: debug.c
	$(CC) $(CFLAGS) -c debug.c
	
selection.o: selection.c
	$(CC) $(CFLAGS) -c selection.c

mem.o: mem.c
	$(CC) $(CFLAGS) -c mem.c
	
clean: 
	rm main.exe main.o log_parse.o debug.o selection.o mem.o
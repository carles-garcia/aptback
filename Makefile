
CC = gcc
CFLAGS = -g -ggdb -Wall -std=gnu99

all: aptback

aptback: main.o log_parse.o debug.o selection.o darray.o
	$(CC) $(CFLAGS) -o main.exe main.o log_parse.o debug.o selection.o darray.o
main.o: main.c 
	$(CC) $(CFLAGS) -c main.c
	
log_parse.o: log_parse.c
	$(CC) $(CFLAGS) -c log_parse.c
	
debug.o: debug.c
	$(CC) $(CFLAGS) -c debug.c
	
selection.o: selection.c
	$(CC) $(CFLAGS) -c selection.c
	
darray.o: darray.c
	$(CC) $(CFLAGS) -c darray.c
	
clean: 
	rm main.exe main.o log_parse.o debug.o selection.o darray.o
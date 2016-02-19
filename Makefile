CC = gcc
CFLAGS = -g -ggdb -Wall -std=gnu99
CFLAGS = -g -O2 -std=gnu99
INSTALL_PATH = /usr/local

all: aptback

aptback: main.o log_parse.o selection.o darray.o print_search.o argp_aux.o
	$(CC) $(CFLAGS) -o aptback main.o log_parse.o selection.o darray.o print_search.o argp_aux.o
	
main.o: main.c 
	$(CC) $(CFLAGS) -c main.c
	
log_parse.o: log_parse.c
	$(CC) $(CFLAGS) -c log_parse.c
	
selection.o: selection.c
	$(CC) $(CFLAGS) -c selection.c
	
darray.o: darray.c
	$(CC) $(CFLAGS) -c darray.c
	
print_search.o: print_search.c
	$(CC) $(CFLAGS) -c print_search.c
	
argp_aux.o: argp_aux.c
	$(CC) $(CFLAGS) -c argp_aux.c
	
clean: 
	rm aptback main.o log_parse.o selection.o darray.o print_search.o argp_aux.o
	
install: aptback
	cp -v aptback $(INSTALL_PATH)/bin
	
uninstall:
	rm $(INSTALL_PATH)/bin/aptback
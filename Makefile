CC = gcc
CFLAGS = -g -ggdb -Wall  -pedantic -std=gnu11
INSTALL_PATH = /usr/local
EXE_NAME = aptback

all: aptback

aptback: main.o log_parse.o selection.o darray.o print_search.o argp_aux.o
	$(CC) $(CFLAGS) -o $(EXE_NAME) main.o log_parse.o selection.o darray.o print_search.o argp_aux.o
	
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
	rm $(EXE_NAME) main.o log_parse.o selection.o darray.o print_search.o argp_aux.o
	
install: aptback
	cp -v $(EXE_NAME) $(INSTALL_PATH)/bin
	
uninstall:
	rm $(INSTALL_PATH)/bin/$(EXE_NAME)

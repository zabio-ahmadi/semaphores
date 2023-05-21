#updated
CC=gcc
CFLAGS= -Wall -Wextra -std=c11 -g -O3 -fsanitize=leak -fsanitize=address  -fsanitize=address
LDFLAGS= -lm -lpthread
all: executable 

executable:arcade.o 
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) 


arcade.o: arcade.c
	$(CC) $(CFLAGS) -c $^ -o $@ $(LDFLAGS)

run:
	./executable
clean:
	rm -rf *.o executable
	$(MAKE) cls

rebuild: 
	$(MAKE) clean 
	$(MAKE) all
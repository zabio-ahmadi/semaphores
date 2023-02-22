CC=gcc
CFLAGS= -Wall -Wextra -std=c11 -g -O3 -fsanitize=leak -fsanitize=address  -fsanitize=address
LDFLAGS= -lm -lpthread
VPATH=semaphore/
all: executable 

executable: semaphore.o main.o 
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) 


semaphore: semaphore.c semaphore.h 
	$(CC) $(CFLAGS) -c $^ -o $@
main.o: main.c
	$(CC) $(CFLAGS) -c $^ -o $@

run:
	./executable
clean:
	rm -rf *.o executable
	$(MAKE) cls

rebuild: 
	$(MAKE) clean 
	$(MAKE) all
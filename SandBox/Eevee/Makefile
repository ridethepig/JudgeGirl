CC=gcc
CFLAGS= -Wall -g

.PHONY: all
all: caretaker

caretaker: caretaker.o syscall_listener_x64.o common.o
	$(CC) $(CFLAGS) caretaker.o syscall_listener_x64.o common.o -o caretaker

caretaker.o: caretaker.c exit_code.h syscall_listener.h common.h
	$(CC) $(CFLAGS) -c caretaker.c

syscall_listener_x64.o: syscall_listener_x64.c exit_code.h syscall_listener.h common.h
	$(CC) $(CFLAGS) -c syscall_listener_x64.c

common.o: common.c common.h
	$(CC) $(CFLAGS) -c common.c

.PHONY: clean
clean:
	rm *.o caretaker

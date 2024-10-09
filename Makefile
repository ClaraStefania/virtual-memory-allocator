CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
SRC = sfl.c doubly_list.c memory_allocator.c
OBJ = $(SRC:.c=.o)

build: $(OBJ)
	$(CC) $(CFLAGS) -o sfl $(OBJ)

run_sfl: build
	./sfl

clean:
	rm -f sfl $(OBJ)

pack:
	zip -FSr Virtual_Memory_Allocator.zip README.md Makefile *.c *.h
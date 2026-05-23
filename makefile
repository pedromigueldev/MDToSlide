CC = gcc 										
DEBUG_FLAGS     = -g3 \
                  -Wall -Wextra -Wconversion -Wsign-conversion \
                  -fsanitize=address \
                  -fsanitize=undefined \
                  -fsanitize=leak\
                  -std=c11
                  
CFLAGS = $(DEBUG_FLAGS)

.PHONY: all clean								

all: MDToSite

MDToSite: main.c
	rm -f MDToSite && $(CC) $(CFLAGS) main.c -o MDToSite
clean:											
	rm -f MDToSite

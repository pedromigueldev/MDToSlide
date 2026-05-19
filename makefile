CC = gcc 										
CFLAGS = -Wall -Wextra -pedantic -g
.PHONY: all clean								
all: MDToSite
MDToSite: main.c 						
	$(CC) $(CFLAGS) main.c -o MDToSite
clean:											
	rm -f MDToSite

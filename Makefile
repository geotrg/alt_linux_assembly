CC = gcc
CFLAGS_DUMB = -O0 -Wall -Wextra
CFLAGS_FAST = -O2 -Wall -Wextra

all: rsa_dumb rsa_optimized rsa_compare

rsa_dumb: rsa_dumb.c
	$(CC) $(CFLAGS_DUMB) -o $@ $<

rsa_optimized: rsa_optimized.c
	$(CC) $(CFLAGS_FAST) -o $@ $<

rsa_compare: rsa_compare.c
	$(CC) $(CFLAGS_FAST) -o $@ $<

clean:
	rm -f rsa_dumb rsa_optimized rsa_compare

.PHONY: all clean

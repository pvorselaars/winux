SRC = $(wildcard *.c)
BIN = $(SRC:.c=.exe)

CC = cl
LD = link
CFLAGS = /nologo
LDFLAGS = /nologo

all: $(BIN)

debug: CFLAGS += /Z7
debug: all

%.exe: %.obj
	$(LD) $(LDFLAGS) $<

%.obj: %.c
	$(CC) $(CFLAGS) $<

clean:
	rm -f *.exe *.pdb

.PHONY: clean

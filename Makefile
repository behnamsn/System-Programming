SOURCES = $(wildcard *.c)
EXECS = $(SOURCES:%.c=%)
CFLAGS = -pthread

all: $(EXECS)

clean:
	rm -f $(EXECS)

.PHONY: all clean

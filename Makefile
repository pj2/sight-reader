CC=gcc
PROJECT=sight-reader
LFLAGS=-L/usr/lib64 -lm -lgtk-x11-2.0 -lglib-2.0
CFLAGS=-Wall -g -I/usr/include/gtk-2.0 `pkg-config --cflags gtk+-2.0 \ pkg-config --libs gtk+-2.0`
OBJS=$(patsubst %.c,%.o,$(wildcard *.c))

all: $(OBJS)
	$(CC) -o $(PROJECT) $^ $(LFLAGS) $(CFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o
	rm -f $(PROJECT)
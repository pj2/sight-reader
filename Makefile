SRC_F=src/
INC_F=include/
OBJ_F=obj/

CC=gcc
PROJECT=sight-reader
LFLAGS=-L/usr/lib64 -lm -lgtk-x11-2.0 -lglib-2.0
CFLAGS=-Wall -g -I$(INC_F) -I/usr/include/gtk-2.0 `pkg-config --cflags gtk+-2.0 \ pkg-config --libs gtk+-2.0`
OBJS=$(patsubst $(SRC_F)%.c,$(OBJ_F)%.o,$(wildcard $(SRC_F)*.c))

all: $(OBJS)
	$(CC) -o $(PROJECT) $^ $(LFLAGS) $(CFLAGS)

$(OBJ_F)%.o: $(SRC_F)%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ_F)*.o
	rm -f $(PROJECT)
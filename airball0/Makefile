# Makefile

TARGET = airball 
OBJECTS = main.o anims.o animspr.o font.o frame.o game.o hero.o menu.o psd.o render.o room.o sfx.o

CFLAGS = -O3 -Wall -I/usr/include/SDL -L/usr/lib -L. -s -DNDEBUG 
LIBS = -lSDL -s libymlib.a

CC = gcc
LINKER = g++

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LINKER) $(CFLAGS) -o $@ $^ $(LIBS) 

.cc.o:
	$(CC) $(CFLAGS) -o $< 

clean:
	rm $(OBJECTS)


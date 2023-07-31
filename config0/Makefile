# Mon Makefile
#PATH = /usr/include/SDL
TARGET = airball_cfg 
OBJECTS = main.o font.o 

CFLAGS = -O3 -Wall -I/usr/include/SDL -L/usr/lib -fno-strict-aliasing -s 
LIBS = -lSDL
CC = gcc

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) 

clean:
	rm $(TARGET) $(OBJECTS)


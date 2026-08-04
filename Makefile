CC = gcc
CFLAGS = -O2 -Iinclude
LDFLAGS = -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl

SRC = $(wildcard src/*.c)
BUILD_DIR = build
TARGET = $(BUILD_DIR)/main

all:
	xxd -i src/shaders/default.frag > src/shaders/default_frag.h
	xxd -i src/shaders/default.vert > src/shaders/default_vert.h
	
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS) 

	./$(TARGET)
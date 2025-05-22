CC        := gcc
CFLAGS    := -Wall -std=c99

LIBS_LINUX := -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
LIBS_MAC   := -lraylib -framework OpenGL -framework Cocoa \
               -framework IOKit -framework CoreAudio \
               -framework CoreVideo

SRC       := main.c
TARGET    := main

UNAME_S   := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
  LIBS := $(LIBS_LINUX)
endif
ifeq ($(UNAME_S),Darwin)
  LIBS := $(LIBS_MAC)
endif

.PHONY: all clean run

all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LIBS)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)

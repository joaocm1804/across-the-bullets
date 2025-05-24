CC        := gcc
CFLAGS    := -Wall -std=c99

SRC       := main.c
TARGET    := main

UNAME_S   := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
  PREFIX_RAYLIB := $(shell brew --prefix raylib)
  CFLAGS        += -I$(PREFIX_RAYLIB)/include
  LDFLAGS       := -L$(PREFIX_RAYLIB)/lib
  LIBS          := -lraylib \
                   -framework OpenGL \
                   -framework Cocoa \
                   -framework IOKit \
                   -framework CoreAudio \
                   -framework CoreVideo
else

  LIBS := -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
endif

.PHONY: all clean run

all:
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(SRC) $(LIBS)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)
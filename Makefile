include ./src/include/Makefile.inc

TARGET = ss

SOURCES := $(wildcard ./src/*.c)
SOURCES_UTILS := $(wildcard ./src/utils/*.c)

OBJECTS := $(SOURCES:.c=.o)
OBJECTS_UTILS := $(SOURCES_UTILS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS) $(OBJECTS_UTILS)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf $(OBJECTS) $(TARGET)

.PHONY: all clean

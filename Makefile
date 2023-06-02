include ./src/include/Makefile.inc

TARGET = ss
SOURCES := $(wildcard ./src/*.c)
OBJECTS := $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf $(OBJECTS) $(TARGET)

.PHONY: all clean

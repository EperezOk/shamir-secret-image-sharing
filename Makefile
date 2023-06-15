include ./src/include/Makefile.inc

# Reference: https://stackoverflow.com/questions/52034997/how-to-make-makefile-recompile-when-a-header-file-is-changed and ChatGPT

SOURCES := $(wildcard $(SRCDIR)/*.c) $(wildcard $(SRCDIR)/utils/*.c)
OBJECTS := $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))
DEPENDS := $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.d, $(SOURCES))

TESTS := $(wildcard $(TESTDIR)/*.c)
TEST_OBJECTS := $(filter-out $(OBJDIR)/main.o, $(OBJECTS))
TESTBINS := $(patsubst $(TESTDIR)/%.c, $(TESTBINDIR)/%, $(TESTS))

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

-include $(DEPENDS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c Makefile | $(OBJDIR)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(TESTBINDIR)/%: $(TESTDIR)/%.c | $(TESTBINDIR)
	$(CC) $(CFLAGS) $< $(TEST_OBJECTS) -o $@ $(TESTLIBS)

$(OBJDIR):
	mkdir -p $(dir $(OBJECTS))

$(TESTBINDIR):
	mkdir -p $@

test: $(TESTBINS)
	@for test in $(TESTBINS); do \
		$$test --verbose; \
	done

clean:
	$(RM) $(TARGET) $(OBJECTS) $(DEPENDS) $(TESTBINS)
	$(RM) -r $(OBJDIR)

.PHONY: all clean 

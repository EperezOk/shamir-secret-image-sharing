TARGET := ss
SRCDIR := src
OBJDIR := object
TESTDIR := $(SRCDIR)/tests
TESTBINDIR := $(OBJDIR)/tests

CFLAGS := -std=c11 -pedantic -pedantic-errors -Wall -fsanitize=address -Wextra -Werror -Wno-unused-parameter -Wno-implicit-fallthrough -D_POSIX_C_SOURCE=200112L

LIBS := -lm
TESTLIBS := $(LIBS) -lcriterion

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
LIBS = -lm -lmpfr -lgmp
TARGET = poly_comparison
TEST_TARGET = test_runner
SRCDIR = src
UTILSDIR = $(SRCDIR)/utils
TESTSDIR = $(SRCDIR)/tests

MAIN_SOURCES = main.c \
               $(SRCDIR)/naive.c \
               $(SRCDIR)/naive_mpfr.c \
               $(SRCDIR)/karatsuba.c \
               $(SRCDIR)/tom.c \
               $(SRCDIR)/tom4.c \
               $(SRCDIR)/cutoffs.c \
               $(UTILSDIR)/utils.c \
               $(UTILSDIR)/timer.c \
               $(UTILSDIR)/timer_log.c \
               $(UTILSDIR)/arena_allocator.c \
               $(TESTSDIR)/test_framework.c \
               $(TESTSDIR)/test_naive.c \
               $(TESTSDIR)/test_karatsuba.c \
               $(TESTSDIR)/test_toom_cook.c \
               $(TESTSDIR)/test_tom4.c

TEST_SOURCES = test_main.c \
               $(SRCDIR)/naive.c \
               $(SRCDIR)/karatsuba.c \
               $(SRCDIR)/tom.c \
               $(SRCDIR)/tom4.c \
               $(SRCDIR)/cutoffs.c \
               $(UTILSDIR)/utils.c \
               $(UTILSDIR)/arena_allocator.c \
               $(TESTSDIR)/test_framework.c \
               $(TESTSDIR)/test_naive.c \
               $(TESTSDIR)/test_karatsuba.c \
               $(TESTSDIR)/test_toom_cook.c \
               $(TESTSDIR)/test_tom4.c

MAIN_OBJECTS = $(MAIN_SOURCES:.c=.o)
TEST_OBJECTS = $(TEST_SOURCES:.c=.o)

all: $(TARGET) $(TEST_TARGET)

$(TARGET): $(MAIN_OBJECTS)
	$(CC) $(MAIN_OBJECTS) -o $(TARGET) $(LIBS)

$(TEST_TARGET): $(TEST_OBJECTS)
	$(CC) $(TEST_OBJECTS) -o $(TEST_TARGET) -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -f $(MAIN_OBJECTS) $(TEST_OBJECTS) $(TARGET) $(TEST_TARGET)

.PHONY: all test clean
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
CPPFLAGS = -I$(SRCDIR) -I$(UTILSDIR) -I$(TESTSDIR)
LIBS = -lm -lmpfr -lgmp

TARGET = poly_comparison
TEST_TARGET = test_runner

SRCDIR = src
UTILSDIR = $(SRCDIR)/utils
TESTSDIR = $(SRCDIR)/tests

# Sources shared by both the main program and the test runner
COMMON_SOURCES = \
	$(SRCDIR)/naive.c \
	$(SRCDIR)/karatsuba.c \
	$(SRCDIR)/tom.c \
	$(SRCDIR)/tom4.c \
	$(SRCDIR)/cutoffs.c \
	$(SRCDIR)/naive_mpfr.c \
	$(SRCDIR)/mpfr_poly.c \
	$(UTILSDIR)/utils.c \
	$(UTILSDIR)/arena_allocator.c

MAIN_SOURCES = \
	main.c \
	$(COMMON_SOURCES) \
	$(UTILSDIR)/timer.c \
	$(UTILSDIR)/timer_log.c \
    src/tests/test_framework.c \
    src/tests/test_naive.c \
    src/tests/test_karatsuba.c \
    src/tests/test_toom_cook.c \
    src/tests/test_tom4.c \
    src/tests/test_naive_mpfr.c


TEST_SOURCES = \
	test_main.c \
	$(COMMON_SOURCES) \
	$(TESTSDIR)/test_framework.c \
	$(TESTSDIR)/test_naive.c \
	$(TESTSDIR)/test_naive_mpfr.c \
	$(TESTSDIR)/test_karatsuba.c \
	$(TESTSDIR)/test_toom_cook.c \
	$(TESTSDIR)/test_tom4.c

MAIN_OBJECTS = $(MAIN_SOURCES:.c=.o)
TEST_OBJECTS = $(TEST_SOURCES:.c=.o)

all: $(TARGET) $(TEST_TARGET)

$(TARGET): $(MAIN_OBJECTS)
	$(CC) $(MAIN_OBJECTS) -o $(TARGET) $(LIBS)

$(TEST_TARGET): $(TEST_OBJECTS)
	$(CC) $(TEST_OBJECTS) -o $(TEST_TARGET) $(LIBS)

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -f $(MAIN_OBJECTS) $(TEST_OBJECTS) $(TARGET) $(TEST_TARGET)

.PHONY: all test clean

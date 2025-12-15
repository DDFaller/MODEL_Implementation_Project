CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
LIBS = -lm -lmpfr -lgmp
TARGET = poly_comparison
SRCDIR = src
UTILSDIR = $(SRCDIR)/utils

SOURCES = main.c \
          $(SRCDIR)/naive.c \
          $(SRCDIR)/naive_mpfr.c \
          $(SRCDIR)/karatsuba.c \
          $(SRCDIR)/tom.c \
          $(UTILSDIR)/utils.c \
          $(UTILSDIR)/timer.c \
          $(UTILSDIR)/timer_log.c

OBJECTS = $(SOURCES:.c=.o)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: clean
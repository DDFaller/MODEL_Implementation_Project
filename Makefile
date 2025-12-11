CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
TARGET = poly_comparison
SRCDIR = src
UTILSDIR = $(SRCDIR)/utils

SOURCES = main.c \
          $(SRCDIR)/naive.c \
          $(SRCDIR)/karatsuba.c \
          $(UTILSDIR)/utils.c \
          $(UTILSDIR)/timer.c \
          $(UTILSDIR)/timer_log.c

OBJECTS = $(SOURCES:.c=.o)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: clean
# Makefile
CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS =

SRC = uart_parser.c ioctl_wrapper.c
OBJ = $(SRC:.c=.o)
TARGET = uart_parser

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

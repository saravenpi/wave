CC = gcc
CFLAGS = -Wall -I./include
LDLIBS = -L./lib -lwave
LDFLAGS = -Wl,-rpath,./lib

SRCS = main.c
NAME = wave_server

OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS) $(LDFLAGS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: clean all

.PHONY: all clean re

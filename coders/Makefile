NAME	= codexion
CC		= cc
CFLAGS	= -Wall -Wextra -Werror -pthread
SRCS	= main.c parse.c heap.c heap_utils.c heap_ops.c dongle.c \
		  simu_init.c simu_destroy.c utils.c log.c dongle_ops.c coder.c simu_run.c \
		  monitor.c
OBJS	= $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c codexion.h
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -f $(OBJS)

fclean: clean
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
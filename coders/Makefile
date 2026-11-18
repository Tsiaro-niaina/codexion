
NAME    = codexion
CC      = cc
CFLAGS  = -Wall -Wextra -Werror -pthread

OBJ_DIR = obj

SRCS    = main.c parsing_arg.c heap.c heap_utils.c heap_ops.c dongle.c \
          simulation_init.c simulation_destroy.c utils.c log.c dongle_operations.c \
          coder.c simulation_run.c monitoring.c

OBJS    = $(addprefix $(OBJ_DIR)/, $(SRCS:.c=.o))

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: %.c codexion.h | $(OBJ_DIR)
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJ_DIR)

fclean: clean
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

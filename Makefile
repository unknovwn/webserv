NAME         = webserv

SRC_DIR      = src/
SOURCE_FILES = webserv.cpp
SRC          = $(addprefix $(SRC_DIR), $(SOURCE_FILES))
INCLD        = include/


CC           = clang++
CFLAGS       = -Wall -Wextra -Werror -std=c++2a -I$(INCLD)
DEBUG_FLAGS  = -g

OBJ          = $(SRC:.cpp=.o)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $(<:.cpp=.o)


$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

all: $(NAME)

debug: $(OBJ)
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) $(OBJ) -o $(NAME)


clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)

re: fclean all


lint:
	cpplint --quiet --filter=-legal/copyright --recursive .


.PHONY: all clean fclean debug lint re

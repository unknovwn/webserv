NAME         = webserv

SRC_DIR      = src/
SOURCE_FILES = webserv.cpp \
			   config_parser/config_parser.cpp \
			   location/location.cpp \
			   server/server.cpp \
			   utils/string.cpp

SRC          = $(addprefix $(SRC_DIR), $(SOURCE_FILES))
INCLD        = -Iinclude/ -Isrc/lexer/include

LIBS         = src/lexer/build/libwebserver_lexer_lib.a

CC           = clang++
CFLAGS       = -Wall -Wextra -Werror -std=c++2a -I$(INCLD)
DEBUG_FLAGS  = -g

OBJ          = $(SRC:.cpp=.o)

.cpp.o:
	$(CC) $(CFLAGS) $(INCLD) -c $< -o $(<:.cpp=.o)


$(NAME): $(OBJ)
	cd src/lexer && mkdir -p build && cd build && cmake .. && make
	$(CC) $(CFLAGS) $(OBJ) $(LIBS) -o $(NAME)

all: $(NAME)

debug: $(OBJ)
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) $(OBJ) -o $(NAME)


clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)
	rm -rf src/lexer/build

re: fclean all


lint:
	cpplint --quiet --filter=-legal/copyright,-runtime/ --recursive .


.PHONY: all clean fclean debug lint re

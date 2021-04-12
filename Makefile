NAME         = webserv

SRC_DIR      = src/
SOURCE_FILES = webserv.cpp \
			   config_parser/config_parser.cpp \
			   location/location.cpp \
			   server/server.cpp \
			   utils/string.cpp

SRC          = $(addprefix $(SRC_DIR), $(SOURCE_FILES))
INCLD        = -Iinclude/ \
			   -Isrc/lexer/include/ \
			   -Isrc/request_parser/include \
			   -Isrc/webserver_client/include

LIBS         = src/lexer/build/libwebserver_lexer_lib.a \
			   src/request_parser/build/librequest_parser.a \
			   src/webserver_client/build/libwebserver_client.a

CC           = clang++
DEBUG_FLAGS  = -g -fsanitize=address
CFLAGS       = -Wall -Wextra -Werror -std=c++2a $(INCLD)

OBJ          = $(SRC:.cpp=.o)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $(<:.cpp=.o)


$(NAME): $(OBJ)
	cd src/lexer && mkdir -p build && cd build && cmake .. && make
	cd src/request_parser && mkdir -p build && cd build && cmake .. && make
	cd src/webserver_client && mkdir -p build && cd build && cmake .. && make
	$(CC) $(OBJ) $(LIBS) -o $(NAME)

all: $(NAME)

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)
	rm -rf src/lexer/build
	rm -rf src/request_parser/build
	rm -rf src/webserver_client/build

re: fclean all


lint:
	cpplint --quiet --filter=-legal/copyright,-runtime/ --recursive .


.PHONY: all clean fclean debug lint re

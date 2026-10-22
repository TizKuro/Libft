NAME = libft.a

OBJS = srcs/ft_bzero.o srcs/ft_isalnum.o srcs/ft_isalpha.o srcs/ft_isascii.o srcs/ft_isdigit.o srcs/ft_isprint.o srcs/ft_memcpy.o srcs/ft_memmove.o srcs/ft_memset.o srcs/ft_strchr.o srcs/ft_strlcat.o srcs/ft_strlcpy.o srcs/ft_strlen.o srcs/ft_strncmp.o srcs/ft_strrchr.o srcs/ft_tolower.o srcs/ft_toupper.o srcs/ft_atoi.o srcs/ft_memchr.o srcs/ft_memcmp.o srcs/ft_strdup.o srcs/ft_calloc.o srcs/ft_strnstr.o

.DEFAULT: all

.PHONY: all clean fclean re

.SUFFIXES: .c.o

all: ${NAME}

clean:
	rm -f ${OBJS}

fclean: clean 
	rm -f ${NAME} 

re: fclean all

${NAME}: $(OBJS)
	ar rcs ${NAME} ${OBJS}

.c.o:
	cc -Wall -Wextra -Werror -c -o$@ $< -I includes/

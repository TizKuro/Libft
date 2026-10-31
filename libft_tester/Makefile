# Testeur Libft (sujet v19.3)
# Par défaut, ta libft est cherchée dans ../libft  ->  make LIBFT=chemin

LIBFT	?= /home/tristan/42T/42CC/Libft
NAME	= tester
CC		= cc
CFLAGS	= -Wall -Wextra -g -O0 -fno-builtin -fno-omit-frame-pointer
WRAP	= -Wl,--wrap=malloc,--wrap=free,--wrap=calloc,--wrap=realloc
SRCS	= framework.c test_part1.c test_part2.c test_part3.c
OBJS	= $(SRCS:%.c=obj/%.o)
LIBA	= $(LIBFT)/libft.a

# Arguments passés au testeur :  make test F="ft_split itoa"  /  make test F=3
F		?=

all: check test

test: libft $(NAME)
	@./$(NAME) $(F) || true

verbose: libft $(NAME)
	@./$(NAME) -v $(F) || true

check:
	@bash ./check_project.sh $(LIBFT) || true

valgrind: libft $(NAME)
	TESTER_TIMEOUT=120 valgrind -q --leak-check=full --error-exitcode=1 ./$(NAME) --no-soft $(F)

libft:
	@test -d "$(LIBFT)" || { echo "Dossier $(LIBFT) introuvable : make LIBFT=chemin/vers/ta/libft"; exit 1; }
	@$(MAKE) -s -C $(LIBFT)

# (relink à chaque fois : on peut changer de LIBFT entre deux appels)
$(NAME): $(OBJS) FORCE
	@$(CC) $(CFLAGS) -no-pie $(OBJS) -Wl,--whole-archive $(LIBA) -Wl,--no-whole-archive $(WRAP) -o $(NAME)

obj/%.o: %.c tester.h protos.h
	@mkdir -p obj
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf obj

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all test verbose check valgrind libft clean fclean re FORCE
FORCE:

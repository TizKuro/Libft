#!/usr/bin/env bash
# Vérifications « hors fonctions » : fichiers rendus, Makefile, libft.h,
# fonctions externes, variables globales, README, norminette.
# usage : ./check_project.sh [chemin/vers/libft]

LIBFT="${1:-../libft}"
CC="${CC:-cc}"
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

if [ -t 1 ] && [ -z "$NO_COLOR" ]; then
	R=$'\033[31m'; G=$'\033[32m'; Y=$'\033[33m'; B=$'\033[1m'; D=$'\033[2m'; Z=$'\033[0m'
else
	R=""; G=""; Y=""; B=""; D=""; Z=""
fi
OK=0; KO=0; WA=0
ok()   { echo "  ${G}✔${Z} $*"; OK=$((OK+1)); }
ko()   { echo "  ${R}✘${Z} $*"; KO=$((KO+1)); }
warn() { echo "  ${Y}⚠${Z} $*"; WA=$((WA+1)); }
info() { echo "    ${D}$*${Z}"; }
title(){ echo; echo "${B}━━ $* ━━${Z}"; }

if [ ! -d "$LIBFT" ]; then
	echo "${R}Dossier introuvable : $LIBFT${Z}"
	echo "Indique le chemin : make check LIBFT=chemin/vers/libft"
	exit 2
fi
LIBFT="$(cd "$LIBFT" && pwd)"
echo "${B}Projet analysé :${Z} $LIBFT"

FUNCS="isalpha isdigit isalnum isascii isprint strlen memset bzero memcpy
memmove strlcpy strlcat toupper tolower strchr strrchr strncmp memchr memcmp
strnstr atoi calloc strdup substr strjoin strtrim split itoa strmapi striteri
putchar_fd putstr_fd putendl_fd putnbr_fd lstnew lstadd_front lstsize lstlast
lstadd_back lstdelone lstclear lstiter lstmap"

# ---------------------------------------------------------------------------
title "Fichiers rendus"
# ---------------------------------------------------------------------------
[ -f "$LIBFT/Makefile" ] && ok "Makefile présent" || ko "Makefile absent"
[ -f "$LIBFT/libft.h" ] && ok "libft.h présent à la racine" || ko "libft.h absent de la racine"
nsrc=$(ls "$LIBFT"/ft_*.c 2>/dev/null | wc -l)
[ "$nsrc" -gt 0 ] && ok "$nsrc fichier(s) ft_*.c à la racine" || ko "aucun fichier ft_*.c à la racine"
extra=""
while IFS= read -r f; do
	b=$(basename "$f")
	case "$b" in
		Makefile|libft.h|README.md|ft_*.c|.git|.gitignore|libft.a|*.o) ;;
		*) extra="$extra $b" ;;
	esac
done < <(find "$LIBFT" -mindepth 1 -maxdepth 1)
if [ -n "$extra" ]; then
	warn "éléments en trop à la racine (« Submitting unused files is not allowed ») :$extra"
	info "vérifie qu'ils ne seront pas poussés sur le dépôt (main.c de test, dossiers, a.out…)"
else
	ok "aucun fichier superflu"
fi
missing=""
for f in $FUNCS; do
	[ -f "$LIBFT/ft_$f.c" ] || missing="$missing ft_$f.c"
done
[ -n "$missing" ] && warn "fichiers attendus absents (ok si les fonctions sont ailleurs) :$missing"
if grep -q "restrict" "$LIBFT"/libft.h "$LIBFT"/ft_*.c 2>/dev/null; then
	ko "le mot-clé « restrict » est utilisé (interdit par le sujet)"
else
	ok "pas de « restrict »"
fi

# ---------------------------------------------------------------------------
title "Makefile"
# ---------------------------------------------------------------------------
MK="$LIBFT/Makefile"
if [ -f "$MK" ]; then
	for rule in all clean fclean re; do
		grep -Eq "^$rule[[:space:]]*:" "$MK" && ok "règle $rule" || ko "règle $rule absente"
	done
	grep -Eq '^\$\(NAME\)[[:space:]]*:' "$MK" && ok 'règle $(NAME)' || ko 'règle $(NAME) absente'
	grep -Eq '^NAME[[:space:]]*:?=[[:space:]]*libft\.a' "$MK" && ok "NAME = libft.a" || warn "NAME ne semble pas valoir libft.a"
	dry="$(make -C "$LIBFT" -n -B all 2>/dev/null)"
	for fl in -Wall -Wextra -Werror; do
		echo "$dry" | grep -q -- "$fl" && ok "compilation avec $fl" || ko "flag $fl absent de la compilation"
	done
	echo "$dry" | grep -Eq '(^|[[:space:]/])cc[[:space:]]' && ok "compilateur cc" || warn "le compilateur ne semble pas être cc"
	echo "$dry" | grep -Eq '(^|[[:space:]])ar[[:space:]]' && ok "archive créée avec ar" || ko "ar n'est pas utilisé"
	echo "$dry" | grep -q "libtool" && ko "libtool utilisé (interdit)"
	echo "$dry" | grep -q -- "-std=c99" && ko "-std=c99 utilisé (interdit)"
	grep -Eq '\*\.c|wildcard' "$MK" && warn "wildcard / *.c dans le Makefile : interdit par la norme des Makefiles à 42 (liste les fichiers explicitement)"

	make -s -C "$LIBFT" fclean >/dev/null 2>&1
	if out="$(make -C "$LIBFT" 2>&1)"; then
		ok "make compile sans erreur"
	else
		ko "make échoue :"; echo "$out" | tail -15 | sed 's/^/      /'
	fi
	if [ -f "$LIBFT/libft.a" ]; then
		ok "libft.a créée à la racine"
		t1=$(stat -c %Y "$LIBFT/libft.a")
		sleep 1
		out2="$(make -C "$LIBFT" 2>&1)"
		t2=$(stat -c %Y "$LIBFT/libft.a")
		if [ "$t1" = "$t2" ] && ! echo "$out2" | grep -Eq '(^|[[:space:]])(cc|ar|gcc|clang)[[:space:]]'; then
			ok "pas de relink (2e make ne refait rien)"
		else
			ko "RELINK : un 2e « make » recompile ou recrée libft.a"
			echo "$out2" | head -5 | sed 's/^/      /'
		fi
		make -s -C "$LIBFT" clean >/dev/null 2>&1
		if ls "$LIBFT"/*.o >/dev/null 2>&1 || find "$LIBFT" -name '*.o' | grep -q .; then
			ko "make clean laisse des .o"
		else
			ok "make clean supprime les .o"
		fi
		[ -f "$LIBFT/libft.a" ] && ok "make clean garde libft.a" || warn "make clean a supprimé libft.a (c'est le rôle de fclean)"
		make -s -C "$LIBFT" >/dev/null 2>&1
		make -s -C "$LIBFT" fclean >/dev/null 2>&1
		[ ! -f "$LIBFT/libft.a" ] && ok "make fclean supprime libft.a" || ko "make fclean ne supprime pas libft.a"
		make -s -C "$LIBFT" re >/dev/null 2>&1
		[ -f "$LIBFT/libft.a" ] && ok "make re reconstruit libft.a" || ko "make re ne reconstruit pas libft.a"
	else
		ko "libft.a n'a pas été créée à la racine"
	fi
fi

# ---------------------------------------------------------------------------
title "libft.h (prototypes exacts du sujet v19.3)"
# ---------------------------------------------------------------------------
if [ -f "$LIBFT/libft.h" ]; then
	printf '#include "libft.h"\n#include "libft.h"\nint main(void){return 0;}\n' > "$TMP/guard.c"
	if $CC -Wall -Wextra -Werror -I"$LIBFT" -c "$TMP/guard.c" -o "$TMP/g.o" 2>"$TMP/err"; then
		ok "libft.h compile seul et peut être inclus deux fois (include guard)"
	else
		ko "libft.h ne compile pas seul / pas d'include guard :"; head -5 "$TMP/err" | sed 's/^/      /'
	fi
	cat > "$TMP/tlist.c" <<'EOF'
#include <stddef.h>
#include "libft.h"
_Static_assert(offsetof(t_list, content) == 0, "content en premier");
_Static_assert(__builtin_types_compatible_p(__typeof__(((t_list *)0)->content), void *), "content void *");
_Static_assert(__builtin_types_compatible_p(__typeof__(((t_list *)0)->next), struct s_list *), "next struct s_list *");
_Static_assert(sizeof(t_list) == 2 * sizeof(void *), "rien d'autre");
EOF
	if $CC -I"$LIBFT" -c "$TMP/tlist.c" -o "$TMP/t.o" 2>"$TMP/err"; then
		ok "t_list conforme (content, next)"
	else
		ko "t_list non conforme au sujet :"; grep -m3 error "$TMP/err" | sed 's/^/      /'
	fi
	PKO=$KO
	while IFS='|' read -r name type; do
		[ -z "$name" ] && continue
		cat > "$TMP/p.c" <<EOF
#include "libft.h"
_Static_assert(__builtin_types_compatible_p(__typeof__(&$name), $type), "x");
EOF
		if $CC -I"$LIBFT" -c "$TMP/p.c" -o "$TMP/p.o" 2>"$TMP/err"; then
			:
		elif grep -qE "undeclared|implicit" "$TMP/err"; then
			ko "$name non déclarée dans libft.h"
		else
			ko "$name : prototype différent du sujet, attendu  $(echo "$type" | sed 's/(\*)/ '"$name"'/')"
			[ "$name" = ft_lstsize ] && info "le sujet v19.3 indique « unsigned int ft_lstsize(t_list *lst); »"
		fi
	done <<'EOF'
ft_isalpha|int (*)(int)
ft_isdigit|int (*)(int)
ft_isalnum|int (*)(int)
ft_isascii|int (*)(int)
ft_isprint|int (*)(int)
ft_strlen|size_t (*)(const char *)
ft_memset|void *(*)(void *, int, size_t)
ft_bzero|void (*)(void *, size_t)
ft_memcpy|void *(*)(void *, const void *, size_t)
ft_memmove|void *(*)(void *, const void *, size_t)
ft_strlcpy|size_t (*)(char *, const char *, size_t)
ft_strlcat|size_t (*)(char *, const char *, size_t)
ft_toupper|int (*)(int)
ft_tolower|int (*)(int)
ft_strchr|char *(*)(const char *, int)
ft_strrchr|char *(*)(const char *, int)
ft_strncmp|int (*)(const char *, const char *, size_t)
ft_memchr|void *(*)(const void *, int, size_t)
ft_memcmp|int (*)(const void *, const void *, size_t)
ft_strnstr|char *(*)(const char *, const char *, size_t)
ft_atoi|int (*)(const char *)
ft_calloc|void *(*)(size_t, size_t)
ft_strdup|char *(*)(const char *)
ft_substr|char *(*)(char const *, unsigned int, size_t)
ft_strjoin|char *(*)(char const *, char const *)
ft_strtrim|char *(*)(char const *, char const *)
ft_split|char **(*)(char const *, char)
ft_itoa|char *(*)(int)
ft_strmapi|char *(*)(char const *, char (*)(unsigned int, char))
ft_striteri|void (*)(char *, void (*)(unsigned int, char *))
ft_putchar_fd|void (*)(char, int)
ft_putstr_fd|void (*)(char *, int)
ft_putendl_fd|void (*)(char *, int)
ft_putnbr_fd|void (*)(int, int)
ft_lstnew|t_list *(*)(void *)
ft_lstadd_front|void (*)(t_list **, t_list *)
ft_lstsize|unsigned int (*)(t_list *)
ft_lstlast|t_list *(*)(t_list *)
ft_lstadd_back|void (*)(t_list **, t_list *)
ft_lstdelone|void (*)(t_list *, void (*)(void *))
ft_lstclear|void (*)(t_list **, void (*)(void *))
ft_lstiter|void (*)(t_list *, void (*)(void *))
ft_lstmap|t_list *(*)(t_list *, void *(*)(void *), void (*)(void *))
EOF
	[ "$KO" -eq "$PKO" ] && ok "les 43 prototypes correspondent au sujet"
fi

# ---------------------------------------------------------------------------
title "Symboles de libft.a"
# ---------------------------------------------------------------------------
make -s -C "$LIBFT" >/dev/null 2>&1
A="$LIBFT/libft.a"
if [ -f "$A" ]; then
	for f in $FUNCS; do
		nm "$A" 2>/dev/null | grep -Eq "[[:space:]]T ft_$f\$" || ko "ft_$f absente de libft.a"
	done
	und=$(nm -u "$A" 2>/dev/null | awk '{print $NF}' | grep -v ':$' | sort -u \
		| grep -vE '^(ft_|malloc$|free$|write$|__stack_chk_fail|_GLOBAL_OFFSET_TABLE_$|__errno_location$)')
	if [ -n "$und" ]; then
		ko "fonctions externes non autorisées utilisées : $(echo $und)"
		info "seules malloc, free et write sont permises (memset/memcpy peuvent aussi"
		info "être insérés par le compilateur en -O2 : compile sans -O pour vérifier)"
	else
		ok "fonctions externes : uniquement malloc / free / write"
	fi
	glob=$(nm "$A" 2>/dev/null | awk '$2 ~ /^[DBCRG]$/ {print $3}' | sort -u)
	[ -n "$glob" ] && ko "variable(s) globale(s) (interdit) : $(echo $glob)" || ok "aucune variable globale"
	sglob=$(nm "$A" 2>/dev/null | awk '$2 ~ /^[db]$/ {print $3}' | grep -vE '^\.|^__' | sort -u)
	[ -n "$sglob" ] && warn "variable(s) static de fichier / de fonction : $(echo $sglob) (à éviter, souvent refusé en correction)"
	helpers=$(nm "$A" 2>/dev/null | awk '$2 == "T" {print $3}' | grep -v '^ft_' | sort -u)
	if [ -n "$helpers" ]; then
		ko "fonction(s) auxiliaire(s) non « static » : $(echo $helpers)"
		info "le sujet demande de déclarer les fonctions d'aide en static"
	else
		ok "fonctions d'aide bien déclarées static"
	fi
	nm "$A" 2>/dev/null | grep -Eq '[[:space:]]T main$' && ko "un main est présent dans libft.a"
else
	ko "libft.a introuvable"
fi

# ---------------------------------------------------------------------------
title "README.md"
# ---------------------------------------------------------------------------
RD="$LIBFT/README.md"
if [ -f "$RD" ]; then
	ok "README.md présent"
	first=$(head -n 1 "$RD")
	if echo "$first" | grep -Eq '^[*_]This project has been created as part of the 42 curriculum by [^*_]+\.?[*_]$'; then
		ok "1re ligne en italique conforme"
	else
		ko "1re ligne non conforme ; attendu en italique :"
		info "*This project has been created as part of the 42 curriculum by <login>.*"
		info "obtenu : $first"
	fi
	for s in Description Instructions Resources; do
		grep -Eiq "^#+[[:space:]]*$s" "$RD" && ok "section « $s »" || ko "section « $s » absente (titre markdown # $s)"
	done
	grep -Eiq '\b(AI|IA|ChatGPT|Claude|intelligence artificielle)\b' "$RD" \
		&& ok "l'usage de l'IA est mentionné" \
		|| ko "la section Resources doit décrire comment l'IA a été utilisée (et pour quoi)"
	grep -Eiq 'ft_(split|substr|lstmap)' "$RD" \
		&& ok "description détaillée de la bibliothèque (fonctions citées)" \
		|| warn "le sujet demande une description détaillée de la bibliothèque (liste/rôle des fonctions)"
else
	ko "README.md absent (obligatoire)"
fi

# ---------------------------------------------------------------------------
title "Norme"
# ---------------------------------------------------------------------------
if command -v norminette >/dev/null 2>&1; then
	nout="$(cd "$LIBFT" && norminette libft.h ft_*.c 2>&1)"
	nerr=$(echo "$nout" | grep -c "Error")
	if [ "$nerr" -eq 0 ]; then ok "norminette : aucune erreur"
	else
		ko "norminette : $nerr ligne(s) d'erreur"
		echo "$nout" | grep -B1 "Error" | head -20 | sed 's/^/      /'
	fi
else
	warn "norminette non installée ici (pip install norminette) — à lancer sur les machines de l'école"
fi

echo
echo "${B}━━ Bilan projet ━━${Z}  ${G}$OK ✔${Z}   ${R}$KO ✘${Z}   ${Y}$WA ⚠${Z}"
[ "$KO" -eq 0 ]

/*
** Partie 2 — fonctions supplémentaires.
** Les chaînes passées en entrée sont en LECTURE SEULE et entourées de pages
** inaccessibles : modifier l'entrée ou lire après le '\0' fait crasher.
*/
#include "tester.h"
#include <unistd.h>

#define N(a) (sizeof(a) / sizeof(*(a)))

static void	str_result(int line, const char *call, char *got, const char *exp)
{
	if (!t_ok(got != NULL, line, "%s : NULL renvoyé, %s attendu", call, R(exp)))
		return ;
	t_ok(t_is_live(got), line, "%s : le résultat ne vient pas de malloc "
		"(ou ce n'est pas le début du bloc)", call);
	t_ok(strcmp(got, exp) == 0, line, "%s : attendu %s, obtenu %s", call,
		R(exp), R(got));
	free(got);
}

/* ====================================================================== */
/* substr                                                                   */
/* ====================================================================== */
typedef struct s_sub
{
	const char		*s;
	unsigned int	start;
	size_t			len;
	const char		*exp;
}	t_sub;

TEST(2, ft_substr, tableau_de_cas)
{
	t_sub	t[] = {
		{"hello", 0, 5, "hello"}, {"hello", 0, 10, "hello"},
		{"hello", 1, 3, "ell"}, {"hello", 4, 1, "o"}, {"hello", 4, 10, "o"},
		{"hello", 5, 1, ""}, {"hello", 5, 0, ""}, {"hello", 0, 0, ""},
		{"hello", 6, 3, ""}, {"hello", 100, 3, ""}, {"hello", 2, SIZE_MAX, "llo"},
		{"hello", UINT_MAX, 5, ""}, {"", 0, 5, ""}, {"", 1, 1, ""},
		{"lorem ipsum dolor", 6, 5, "ipsum"}, {"lorem ipsum dolor", 0, 1, "l"},
		{"\x80\xff\x01", 1, 1, "\xff"},
	};
	char	call[300];

	for (size_t i = 0; i < N(t); i++)
	{
		char	*s = t_rostr(t[i].s);

		snprintf(call, sizeof call, "ft_substr(%s, %u, %zu)", R(t[i].s),
			t[i].start, t[i].len);
		STEP("%s%s", call, t[i].start > strlen(t[i].s) ? " — start dépasse la "
			"chaîne : ne rien lire au-delà du '\\0'" : "");
		t_reset_counters();
		str_result(__LINE__, call, ft_substr(s, t[i].start, t[i].len), t[i].exp);
	}
}

SOFT_TEST(2, ft_substr, taille_allouee)
{
	t_sub	t[] = {{"hello", 1, 1000, "ello"}, {"hello", 10, 5, ""},
		{"hello world", 0, 3, "hel"}};

	for (size_t i = 0; i < N(t); i++)
	{
		t_reset_counters();
		STEP("ft_substr(%s, %u, %zu)", R(t[i].s), t[i].start, t[i].len);
		char *r = ft_substr(t_rostr(t[i].s), t[i].start, t[i].len);
		CHECK(t_max_request() <= strlen(t[i].exp) + 1, "ft_substr(%s, %u, %zu) "
			": %zu octets alloués pour un résultat de %zu caractères "
			"(n'alloue que le nécessaire : min(len, strlen(s) - start) + 1)",
			R(t[i].s), t[i].start, t[i].len, t_max_request(), strlen(t[i].exp));
		free(r);
	}
}

TEST(2, ft_substr, echec_de_malloc)
{
	char	*s = t_rostr("hello world");

	FAIL_LOOP(char *, r, ft_substr(s, 2, 5), free(r), "ft_substr(%s, 2, 5)",
		R(s));
}

SOFT_TEST(2, ft_substr, null)
{
	STEP("ft_substr(NULL, 0, 5)");
	char *r = ft_substr(NULL, 0, 5);
	CHECK(r == NULL, "ft_substr(NULL, 0, 5) : NULL attendu");
	free(r);
}

/* ====================================================================== */
/* strjoin                                                                  */
/* ====================================================================== */
TEST(2, ft_strjoin, tableau_de_cas)
{
	const char	*t[][3] = {
		{"ab", "cd", "abcd"}, {"", "", ""}, {"", "xyz", "xyz"},
		{"xyz", "", "xyz"}, {"hello ", "world", "hello world"},
		{"\x80\xff", "\x01", "\x80\xff\x01"}, {"a", "b", "ab"},
	};
	char		call[300];

	for (size_t i = 0; i < N(t); i++)
	{
		char	*a = t_rostr(t[i][0]);
		char	*b = t_rostr(t[i][1]);

		snprintf(call, sizeof call, "ft_strjoin(%s, %s)", R(t[i][0]),
			R(t[i][1]));
		STEP("%s", call);
		char *r = ft_strjoin(a, b);
		CHECK(r != a && r != b, "%s : doit renvoyer une NOUVELLE chaîne", call);
		str_result(__LINE__, call, r, t[i][2]);
	}
}

TEST(2, ft_strjoin, longues_chaines)
{
	size_t	n = 300000;
	char	*a = t_guard_after(n + 1);
	char	*b = t_guard_after(n + 1);
	char	*r;

	memset(a, 'a', n);
	a[n] = 0;
	memset(b, 'b', n);
	b[n] = 0;
	STEP("ft_strjoin(%zu × 'a', %zu × 'b')", n, n);
	r = ft_strjoin(a, b);
	if (!CHECK(r != NULL, "ft_strjoin(longues) : NULL inattendu"))
		return ;
	CHECK(strlen(r) == 2 * n && r[0] == 'a' && r[n - 1] == 'a' && r[n] == 'b'
		&& r[2 * n - 1] == 'b', "ft_strjoin(longues) : résultat incorrect");
	free(r);
}

TEST(2, ft_strjoin, echec_de_malloc)
{
	char	*a = t_rostr("foo");
	char	*b = t_rostr("bar");

	FAIL_LOOP(char *, r, ft_strjoin(a, b), free(r), "ft_strjoin(%s, %s)", R(a),
		R(b));
}

SOFT_TEST(2, ft_strjoin, null)
{
	char	*r;

	STEP("ft_strjoin(NULL, \"abc\")");
	r = ft_strjoin(NULL, "abc");
	free(r);
	STEP("ft_strjoin(\"abc\", NULL)");
	r = ft_strjoin("abc", NULL);
	free(r);
	STEP("ft_strjoin(NULL, NULL)");
	r = ft_strjoin(NULL, NULL);
	CHECK(r == NULL, "ft_strjoin(NULL, NULL) : NULL attendu");
	free(r);
}

/* ====================================================================== */
/* strtrim                                                                  */
/* ====================================================================== */
TEST(2, ft_strtrim, tableau_de_cas)
{
	const char	*t[][3] = {
		{"  hello  ", " ", "hello"}, {"xxhixx", "x", "hi"},
		{"abcHelloWorldcba", "abc", "HelloWorld"}, {"aaaa", "a", ""},
		{"", "a", ""}, {"", "", ""}, {"hello", "", "hello"},
		{"  a b  ", " ", "a b"}, {"\t\n x \n", " \n\t", "x"},
		{"ab", "b", "a"}, {"ba", "b", "a"}, {"b", "b", ""},
		{"hello", "xyz", "hello"}, {"   ", " ", ""},
		{"lorem ipsum dolor sit amet", "te", "lorem ipsum dolor sit am"},
		{" \t lorem \t ", " \t", "lorem"}, {"xyzzy", "zyx", ""},
		{"\xff" "abc" "\xff", "\xff", "abc"}, {"abcba", "ab", "c"},
	};
	char		call[300];

	for (size_t i = 0; i < N(t); i++)
	{
		snprintf(call, sizeof call, "ft_strtrim(%s, %s)", R(t[i][0]),
			R(t[i][1]));
		STEP("%s", call);
		str_result(__LINE__, call, ft_strtrim(t_rostr(t[i][0]),
				t_rostr(t[i][1])), t[i][2]);
	}
}

TEST(2, ft_strtrim, echec_de_malloc)
{
	char	*s = t_rostr("  -- bonjour --  ");
	char	*set = t_rostr(" -");

	FAIL_LOOP(char *, r, ft_strtrim(s, set), free(r), "ft_strtrim(%s, %s)",
		R(s), R(set));
}

SOFT_TEST(2, ft_strtrim, null)
{
	char	*r;

	STEP("ft_strtrim(NULL, \" \")");
	r = ft_strtrim(NULL, " ");
	CHECK(r == NULL, "ft_strtrim(NULL, \" \") : NULL attendu");
	free(r);
	STEP("ft_strtrim(\" a \", NULL)");
	r = ft_strtrim(" a ", NULL);
	free(r);
}

/* ====================================================================== */
/* split                                                                    */
/* ====================================================================== */
static void	free_tab(char **r, size_t n)
{
	for (size_t i = 0; i < n; i++)
		if (t_is_live(r[i]))
			free(r[i]);
	if (t_is_live(r))
		free(r);
}

static void	check_split(int line, const char *s, char c, const char **exp)
{
	char	call[300];
	size_t	ne;
	size_t	slots;
	size_t	ng;

	snprintf(call, sizeof call, "ft_split(%s, %s)", R(s), RC((unsigned char)c));
	STEP("%s", call);
	char **r = ft_split(t_rostr(s), c);
	if (!t_ok(r != NULL, line, "%s : NULL renvoyé", call))
		return ;
	if (!t_ok(t_is_live(r), line, "%s : le tableau renvoyé ne vient pas de "
			"malloc", call))
		return ;
	ne = 0;
	while (exp[ne])
		ne++;
	slots = t_block_size(r) / sizeof(char *);
	ng = 0;
	while (ng < slots && r[ng])
		ng++;
	if (!t_ok(ng < slots, line, "%s : tableau non terminé par NULL (ou alloué "
			"trop petit : %zu case(s) pour %zu mot(s) + NULL)", call, slots, ne))
	{
		free_tab(r, slots);
		return ;
	}
	t_ok(ng == ne, line, "%s : %zu mot(s) attendu(s), %zu obtenu(s)", call, ne,
		ng);
	for (size_t i = 0; i < ng && i < ne; i++)
	{
		if (!t_ok(t_is_live(r[i]), line, "%s : le mot %zu n'est pas un bloc "
				"alloué séparément (chaque mot doit avoir son propre malloc)",
				call, i))
			continue ;
		if (!t_ok(memchr(r[i], 0, t_block_size(r[i])) != NULL, line, "%s : le "
				"mot %zu n'est pas terminé par '\\0'", call, i))
			continue ;
		t_ok(strcmp(r[i], exp[i]) == 0, line, "%s : mot %zu attendu %s, obtenu "
			"%s", call, i, R(exp[i]), R(r[i]));
	}
	STEP("libération du résultat de %s (free de chaque mot puis du tableau)",
		call);
	free_tab(r, ng);
}

#define SPLIT(s, c, ...) do { \
	const char *e_[] = {__VA_ARGS__}; \
	check_split(__LINE__, s, c, e_); \
} while (0)

TEST(2, ft_split, tableau_de_cas)
{
	SPLIT("hello world", ' ', "hello", "world", NULL);
	SPLIT("  hello   world  ", ' ', "hello", "world", NULL);
	SPLIT("", ' ', NULL);
	SPLIT("     ", ' ', NULL);
	SPLIT("hello", ' ', "hello", NULL);
	SPLIT(" hello", ' ', "hello", NULL);
	SPLIT("hello ", ' ', "hello", NULL);
	SPLIT("a,b,,c", ',', "a", "b", "c", NULL);
	SPLIT(",,,a,,,", ',', "a", NULL);
	SPLIT("xxaxxbxx", 'x', "a", "b", NULL);
	SPLIT("a b c d e f", ' ', "a", "b", "c", "d", "e", "f", NULL);
	SPLIT("hello", '\0', "hello", NULL);
	SPLIT("", '\0', NULL);
	SPLIT("a\xff" "b\xff\xff" "c", '\xff', "a", "b", "c", NULL);
	SPLIT("lorem ipsum dolor sit amet", 'i', "lorem ", "psum dolor s",
		"t amet", NULL);
	SPLIT("\t\ttab\t", '\t', "tab", NULL);
	SPLIT("abc", 'z', "abc", NULL);
	SPLIT("z", 'z', NULL);
}

TEST(2, ft_split, beaucoup_de_mots)
{
	size_t	nw = 5000;
	char	*s = t_guard_after(nw * 3 + 1);
	char	**r;
	size_t	i;

	for (i = 0; i < nw; i++)
		memcpy(s + i * 3, i % 2 ? "ab " : "cd ", 3);
	s[nw * 3] = 0;
	STEP("ft_split(%zu mots, ' ') — doit finir en moins de 5 s", nw);
	r = ft_split(s, ' ');
	if (!CHECK(r != NULL, "ft_split(5000 mots) : NULL inattendu"))
		return ;
	for (i = 0; r[i] && i < nw; i++)
		if (strcmp(r[i], i % 2 ? "ab" : "cd"))
			break ;
	CHECK(i == nw && r[nw] == NULL, "ft_split(5000 mots) : résultat incorrect "
		"(arrêt au mot %zu)", i);
	free_tab(r, i);
}

static void	free_split_all(char **r)
{
	size_t	i;

	if (!r)
		return ;
	for (i = 0; r[i]; i++)
		free(r[i]);
	free(r);
}

TEST(2, ft_split, echec_de_malloc)
{
	char	*s = t_rostr("  un deux  trois quatre ");

	FAIL_LOOP(char **, r, ft_split(s, ' '), free_split_all(r),
		"ft_split(%s, ' ')", R(s));
	s = t_rostr("");
	FAIL_LOOP(char **, r, ft_split(s, ' '), free_split_all(r),
		"ft_split(%s, ' ')", R(s));
}

SOFT_TEST(2, ft_split, null)
{
	STEP("ft_split(NULL, ' ')");
	char **r = ft_split(NULL, ' ');
	CHECK(r == NULL, "ft_split(NULL, ' ') : NULL attendu");
	free_split_all(r);
}

/* ====================================================================== */
/* itoa                                                                     */
/* ====================================================================== */
static void	itoa_cmp(int line, int n)
{
	char	exp[32];
	char	call[64];

	snprintf(exp, sizeof exp, "%d", n);
	snprintf(call, sizeof call, "ft_itoa(%d)", n);
	STEP("%s", call);
	str_result(line, call, ft_itoa(n), exp);
}

TEST(2, ft_itoa, cas_classiques)
{
	int	t[] = {0, 1, -1, 9, -9, 10, -10, 42, -42, 99, 100, -100, 101, 1000,
		12345, -12345, 999999999, -999999999, 1000000000, -1000000000,
		INT_MAX, INT_MIN, INT_MAX - 1, INT_MIN + 1};

	for (size_t i = 0; i < N(t); i++)
		itoa_cmp(__LINE__, t[i]);
}

TEST(2, ft_itoa, puissances_de_10_et_aleatoires)
{
	unsigned int	seed = 2024;
	long			p;

	for (p = 1; p <= 1000000000L; p *= 10)
	{
		itoa_cmp(__LINE__, (int)p);
		itoa_cmp(__LINE__, (int)p - 1);
		itoa_cmp(__LINE__, (int)p + 1);
		itoa_cmp(__LINE__, -(int)p);
		itoa_cmp(__LINE__, -(int)p + 1);
		itoa_cmp(__LINE__, -(int)p - 1);
	}
	for (int i = 0; i < 1000; i++)
	{
		seed = seed * 1103515245u + 12345u;
		itoa_cmp(__LINE__, (int)(seed ^ (seed >> 9)));
	}
}

TEST(2, ft_itoa, echec_de_malloc)
{
	FAIL_LOOP(char *, r, ft_itoa(INT_MIN), free(r), "ft_itoa(%d)", INT_MIN);
	FAIL_LOOP(char *, r, ft_itoa(0), free(r), "ft_itoa(%d)", 0);
}

/* ====================================================================== */
/* strmapi / striteri                                                       */
/* ====================================================================== */
static unsigned int	g_idx[64];
static char			g_ch[64];
static char			*g_ptr[64];
static int			g_ncalls;

static char	f_upper_even(unsigned int i, char c)
{
	if (g_ncalls < 64)
	{
		g_idx[g_ncalls] = i;
		g_ch[g_ncalls] = c;
	}
	g_ncalls++;
	if (i % 2 == 0 && c >= 'a' && c <= 'z')
		return (c - 32);
	return (c);
}

static char	f_plus_index(unsigned int i, char c)
{
	g_ncalls++;
	return (c + i);
}

static void	check_calls(int line, const char *call, const char *s)
{
	int	n = (int)strlen(s);

	t_ok(g_ncalls == n, line, "%s : f appelée %d fois, %d attendu(es)", call,
		g_ncalls, n);
	for (int i = 0; i < n && i < g_ncalls && i < 64; i++)
	{
		if (!t_ok(g_idx[i] == (unsigned int)i && g_ch[i] == s[i], line,
				"%s : appel n°%d de f reçu (%u, %s), attendu (%d, %s)", call, i,
				g_idx[i], RC((unsigned char)g_ch[i]), i, RC((unsigned char)s[i])))
			break ;
	}
}

TEST(2, ft_strmapi, resultats)
{
	const char	*t[][2] = {{"hello world", "HeLlO WoRlD"}, {"", ""},
		{"a", "A"}, {"ab", "Ab"}, {"ABC", "ABC"}, {"\x80" "a", "\x80" "a"}};
	char		call[200];

	for (size_t i = 0; i < N(t); i++)
	{
		char	*s = t_rostr(t[i][0]);

		g_ncalls = 0;
		snprintf(call, sizeof call, "ft_strmapi(%s, f)", R(t[i][0]));
		STEP("%s", call);
		char *r = ft_strmapi(s, f_upper_even);
		CHECK(r != s, "%s : doit renvoyer une NOUVELLE chaîne", call);
		check_calls(__LINE__, call, t[i][0]);
		str_result(__LINE__, call, r, t[i][1]);
	}
	g_ncalls = 0;
	STEP("ft_strmapi(\"aaaa\", c + index)");
	str_result(__LINE__, "ft_strmapi(\"aaaa\", c + index)",
		ft_strmapi(t_rostr("aaaa"), f_plus_index), "abcd");
}

TEST(2, ft_strmapi, echec_de_malloc)
{
	char	*s = t_rostr("hello");

	g_ncalls = 0;
	FAIL_LOOP(char *, r, ft_strmapi(s, f_upper_even), free(r),
		"ft_strmapi(%s, f)", R(s));
}

SOFT_TEST(2, ft_strmapi, null)
{
	char	*r;

	STEP("ft_strmapi(NULL, f)");
	r = ft_strmapi(NULL, f_upper_even);
	CHECK(r == NULL, "ft_strmapi(NULL, f) : NULL attendu");
	free(r);
	STEP("ft_strmapi(\"abc\", NULL)");
	r = ft_strmapi("abc", NULL);
	CHECK(r == NULL, "ft_strmapi(\"abc\", NULL) : NULL attendu");
	free(r);
}

static void	f_iter(unsigned int i, char *c)
{
	if (g_ncalls < 64)
	{
		g_idx[g_ncalls] = i;
		g_ch[g_ncalls] = *c;
		g_ptr[g_ncalls] = c;
	}
	g_ncalls++;
	if (i % 2 == 0 && *c >= 'a' && *c <= 'z')
		*c -= 32;
}

TEST(2, ft_striteri, modification_sur_place)
{
	const char	*t[][2] = {{"hello world", "HeLlO WoRlD"}, {"", ""},
		{"a", "A"}, {"xyz", "XyZ"}};
	char		call[200];

	for (size_t i = 0; i < N(t); i++)
	{
		char	*s = t_gstr(t[i][0]);
		size_t	n = strlen(s);

		g_ncalls = 0;
		snprintf(call, sizeof call, "ft_striteri(%s, f)", R(t[i][0]));
		STEP("%s", call);
		ft_striteri(s, f_iter);
		check_calls(__LINE__, call, t[i][0]);
		for (size_t k = 0; k < n && (int)k < g_ncalls; k++)
			if (!CHECK(g_ptr[k] == s + k, "%s : f doit recevoir l'ADRESSE du "
					"caractère d'indice %zu (s + %zu)", call, k, k))
				break ;
		CHECK(strcmp(s, t[i][1]) == 0, "%s : chaîne attendue %s, obtenue %s",
			call, R(t[i][1]), R(s));
	}
}

SOFT_TEST(2, ft_striteri, null)
{
	char	s[] = "abc";

	STEP("ft_striteri(NULL, f)");
	ft_striteri(NULL, f_iter);
	STEP("ft_striteri(\"abc\", NULL)");
	ft_striteri(s, NULL);
	CHECK(1, "ok");
}

/* ====================================================================== */
/* put*_fd                                                                  */
/* ====================================================================== */
#define CAPTURE(len, out, CALL) do { \
	int fd_ = t_cap_open(); \
	(void)fd_; \
	CALL; \
	out = t_cap_close(&len); \
} while (0)

TEST(2, ft_putchar_fd, tous_les_octets)
{
	size_t		len;
	const char	*out;

	for (int c = 0; c < 256; c++)
	{
		STEP("ft_putchar_fd(%s, fd)", RC(c));
		CAPTURE(len, out, ft_putchar_fd((char)c, fd_));
		CHECK(len == 1 && (unsigned char)out[0] == c, "ft_putchar_fd(%s, fd) : "
			"%zu octet(s) écrit(s) : %s", RC(c), len, RM(out, len));
	}
}

TEST(2, ft_putchar_fd, fd_invalide)
{
	STEP("ft_putchar_fd('a', -1)");
	ft_putchar_fd('a', -1);
	CHECK(1, "ok");
}

TEST(2, ft_putstr_fd, chaines)
{
	const char	*t[] = {"hello", "", "a", "hello\nworld\n", "\x80\xff\x01",
		"\t tab"};
	size_t		len;
	const char	*out;

	for (size_t i = 0; i < N(t); i++)
	{
		char	*s = t_rostr(t[i]);

		STEP("ft_putstr_fd(%s, fd)", R(t[i]));
		CAPTURE(len, out, ft_putstr_fd(s, fd_));
		CHECK(len == strlen(t[i]) && !memcmp(out, t[i], len), "ft_putstr_fd(%s, "
			"fd) : écrit %s", R(t[i]), RM(out, len));
	}
}

TEST(2, ft_putstr_fd, longue_chaine_et_fd_invalide)
{
	size_t		n = 500000;
	char		*s = t_guard_after(n + 1);
	size_t		len;
	const char	*out;

	memset(s, 'p', n);
	s[n] = 0;
	STEP("ft_putstr_fd(chaîne de %zu car., fd)", n);
	CAPTURE(len, out, ft_putstr_fd(s, fd_));
	CHECK(len == n && out[0] == 'p' && out[n - 1] == 'p', "ft_putstr_fd(longue)"
		" : %zu octets écrits au lieu de %zu", len, n);
	STEP("ft_putstr_fd(\"abc\", -1)");
	ft_putstr_fd(t_rostr("abc"), -1);
}

TEST(2, ft_putendl_fd, chaines)
{
	const char	*t[][2] = {{"hello", "hello\n"}, {"", "\n"}, {"a\nb", "a\nb\n"},
		{"\xff", "\xff\n"}};
	size_t		len;
	const char	*out;

	for (size_t i = 0; i < N(t); i++)
	{
		char	*s = t_rostr(t[i][0]);

		STEP("ft_putendl_fd(%s, fd)", R(t[i][0]));
		CAPTURE(len, out, ft_putendl_fd(s, fd_));
		CHECK(len == strlen(t[i][1]) && !memcmp(out, t[i][1], len),
			"ft_putendl_fd(%s, fd) : attendu %s, écrit %s", R(t[i][0]),
			R(t[i][1]), RM(out, len));
	}
	STEP("ft_putendl_fd(\"abc\", -1)");
	ft_putendl_fd(t_rostr("abc"), -1);
}

static void	putnbr_cmp(int n)
{
	char		exp[32];
	size_t		len;
	const char	*out;

	snprintf(exp, sizeof exp, "%d", n);
	STEP("ft_putnbr_fd(%d, fd)", n);
	CAPTURE(len, out, ft_putnbr_fd(n, fd_));
	CHECK(len == strlen(exp) && !memcmp(out, exp, len), "ft_putnbr_fd(%d, fd) : "
		"attendu \"%s\", écrit %s", n, exp, RM(out, len));
}

TEST(2, ft_putnbr_fd, nombres)
{
	int				t[] = {0, 1, -1, 9, -9, 10, -10, 42, -42, 100, -100, 12345,
		INT_MAX, INT_MIN, INT_MAX - 1, INT_MIN + 1, 1000000000, -1000000000};
	unsigned int	seed = 99;

	for (size_t i = 0; i < N(t); i++)
		putnbr_cmp(t[i]);
	for (int i = 0; i < 500; i++)
	{
		seed = seed * 1103515245u + 12345u;
		putnbr_cmp((int)(seed ^ (seed >> 11)));
	}
	STEP("ft_putnbr_fd(42, -1)");
	ft_putnbr_fd(42, -1);
}

SOFT_TEST(2, ft_putnbr_fd, sans_malloc)
{
	size_t		len;
	const char	*out;

	t_reset_counters();
	STEP("ft_putnbr_fd(INT_MIN, fd)");
	CAPTURE(len, out, ft_putnbr_fd(INT_MIN, fd_));
	(void)out;
	CHECK(t_malloc_count() == 0, "ft_putnbr_fd : %ld appel(s) à malloc — la "
		"seule fonction externe autorisée est write", t_malloc_count());
}

SOFT_TEST(2, ft_putstr_fd, null)
{
	size_t		len;
	const char	*out;

	STEP("ft_putstr_fd(NULL, fd)");
	CAPTURE(len, out, ft_putstr_fd(NULL, fd_));
	if (ft_putendl_fd)
	{
		STEP("ft_putendl_fd(NULL, fd)");
		CAPTURE(len, out, ft_putendl_fd(NULL, fd_));
	}
	(void)out;
	CHECK(1, "ok");
}

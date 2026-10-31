/*
** Partie 1 — fonctions de la libc.
** Quand la libc fournit la fonction, ta version est comparée à l'originale.
** Sinon (strlcpy, strlcat, strnstr ne sont pas dans toutes les glibc),
** les résultats attendus sont écrits à la main, d'après le man BSD.
*/
#include "tester.h"
#include <ctype.h>
#include <strings.h>

#define N(a) (sizeof(a) / sizeof(*(a)))

/* ====================================================================== */
/* is*                                                                      */
/* ====================================================================== */
#define CLASS_TESTS(ft, ref) \
TEST(1, ft, de_EOF_a_255) \
{ \
	for (int c = -1; c <= 255; c++) \
	{ \
		int exp = ref(c) ? 1 : 0; \
		STEP(#ft "(%d)", c); \
		int got = ft(c); \
		CHECK(got == exp, #ft "(%s) : attendu %d, obtenu %d%s", RC(c), exp, \
			got, (exp && got && got != 1) ? " — le sujet exige exactement 1, " \
			"pas juste une valeur non nulle" : ""); \
	} \
}

CLASS_TESTS(ft_isalpha, isalpha)
CLASS_TESTS(ft_isdigit, isdigit)
CLASS_TESTS(ft_isalnum, isalnum)
CLASS_TESTS(ft_isascii, isascii)
CLASS_TESTS(ft_isprint, isprint)

#define CLASS_OUT_OF_RANGE(ft) \
SOFT_TEST(1, ft, hors_plage) \
{ \
	int v[] = {256 + 'a', 256 + 'Z', 256 + '5', 256 + ' ', 300, 1000, -2, \
		-129, -1000, INT_MAX, INT_MIN}; \
	for (size_t i = 0; i < N(v); i++) \
	{ \
		STEP(#ft "(%d)", v[i]); \
		int got = ft(v[i]); \
		CHECK(got == 0, #ft "(%d) : 0 attendu pour une valeur hors de " \
			"[-1, 255], obtenu %d (un cast en char ou unsigned char " \
			"transforme 256+'a' en 'a')", v[i], got); \
	} \
}

CLASS_OUT_OF_RANGE(ft_isalpha)
CLASS_OUT_OF_RANGE(ft_isdigit)
CLASS_OUT_OF_RANGE(ft_isalnum)
CLASS_OUT_OF_RANGE(ft_isprint)

/* isascii est défini pour TOUS les int : pas de comportement indéfini */
TEST(1, ft_isascii, tous_les_int)
{
	int	v[] = {128, 200, 256, 256 + 'a', 1000, -2, -128, -129, INT_MAX,
		INT_MIN, 0x7f, 0x80};

	for (size_t i = 0; i < N(v); i++)
	{
		int	exp = isascii(v[i]) ? 1 : 0;

		STEP("ft_isascii(%d)", v[i]);
		int got = ft_isascii(v[i]);
		CHECK(got == exp, "ft_isascii(%d) : attendu %d, obtenu %d", v[i], exp,
			got);
	}
}

/* ====================================================================== */
/* toupper / tolower                                                        */
/* ====================================================================== */
#define CASE_TESTS(ft, ref) \
TEST(1, ft, de_EOF_a_255) \
{ \
	for (int c = -1; c <= 255; c++) \
	{ \
		int exp = ref(c); \
		STEP(#ft "(%d)", c); \
		int got = ft(c); \
		CHECK(got == exp, #ft "(%s) : attendu %d, obtenu %d", RC(c), exp, got); \
	} \
} \
SOFT_TEST(1, ft, hors_plage) \
{ \
	int v[] = {256 + 'a', 256 + 'A', 1000, -2, -200, INT_MAX, INT_MIN}; \
	for (size_t i = 0; i < N(v); i++) \
	{ \
		STEP(#ft "(%d)", v[i]); \
		int got = ft(v[i]); \
		CHECK(got == v[i], #ft "(%d) : valeur inchangée attendue, obtenu %d", \
			v[i], got); \
	} \
}

CASE_TESTS(ft_toupper, toupper)
CASE_TESTS(ft_tolower, tolower)

/* ====================================================================== */
/* strlen                                                                   */
/* ====================================================================== */
TEST(1, ft_strlen, chaines_variees)
{
	const char	*t[] = {"", "a", "ab", "hello", "hello world", " \t\n\v\f\r",
		"é à ü", "\x80\xff\x01", "abc\0def", "0123456789abcdefghijklmnopqrstuv"};

	for (size_t i = 0; i < N(t); i++)
	{
		char	*s = t_gstr(t[i]);

		STEP("ft_strlen(%s)", R(t[i]));
		size_t got = ft_strlen(s);
		CHECK(got == strlen(t[i]), "ft_strlen(%s) : attendu %zu, obtenu %zu",
			R(t[i]), strlen(t[i]), got);
	}
}

TEST(1, ft_strlen, longue_chaine)
{
	size_t	n = 3000000;
	char	*s = t_guard_after(n + 1);

	memset(s, 'x', n);
	s[n] = 0;
	STEP("ft_strlen(chaîne de %zu caractères)", n);
	size_t got = ft_strlen(s);
	CHECK(got == n, "ft_strlen(chaîne de %zu car.) : obtenu %zu", n, got);
	STEP("ft_strlen(s + 3) (adresse non alignée)");
	got = ft_strlen(s + 3);
	CHECK(got == n - 3, "ft_strlen(s + 3) : attendu %zu, obtenu %zu", n - 3,
		got);
}

/* ====================================================================== */
/* memset / bzero                                                           */
/* ====================================================================== */
static void	fill_pattern(unsigned char *a, unsigned char *b, size_t n)
{
	for (size_t k = 0; k < n; k++)
		a[k] = b[k] = (unsigned char)(k * 7 + 1);
}

TEST(1, ft_memset, comparaison_libc)
{
	static unsigned char	a[1200];
	static unsigned char	b[1200];
	size_t					ns[] = {0, 1, 2, 3, 7, 8, 9, 15, 16, 17, 31, 33, 64,
		100, 511, 1000};
	int						vs[] = {0, 'A', 127, 128, 255, -1, 256 + 'B',
		0x12345, INT_MIN, -129};
	size_t					offs[] = {0, 1, 3};

	for (size_t o = 0; o < N(offs); o++)
		for (size_t i = 0; i < N(ns); i++)
			for (size_t j = 0; j < N(vs); j++)
			{
				size_t	off = offs[o];

				fill_pattern(a, b, sizeof a);
				memset(a + off, vs[j], ns[i]);
				STEP("ft_memset(buf+%zu, %d, %zu)", off, vs[j], ns[i]);
				void *r = ft_memset(b + off, vs[j], ns[i]);
				CHECK(r == b + off, "ft_memset(buf, %d, %zu) : doit renvoyer "
					"buf, obtenu %s", vs[j], ns[i], RP(r, b + off));
				long d = t_memdiff(a, b, sizeof a);
				CHECK(d < 0, "ft_memset(buf+%zu, %d, %zu) : différent de memset "
					"à l'octet %ld (attendu 0x%02x, obtenu 0x%02x)%s", off,
					vs[j], ns[i], d - (long)off, d >= 0 ? a[d] : 0,
					d >= 0 ? b[d] : 0, d >= (long)(off + ns[i])
					? " — écriture au-delà de n !" : (d >= 0 && d < (long)off)
					? " — écriture AVANT le début !" : "");
			}
}

TEST(1, ft_memset, limites_strictes)
{
	size_t	ns[] = {0, 1, 5, 4095, 4096, 4097, 100000};

	for (size_t i = 0; i < N(ns); i++)
	{
		unsigned char	*p = t_guard_after(ns[i]);

		STEP("ft_memset(p, 'z', %zu) sur un buffer d'exactement %zu octets "
			"(un accès au-delà crashe)", ns[i], ns[i]);
		ft_memset(p, 'z', ns[i]);
		size_t k = 0;
		while (k < ns[i] && p[k] == 'z')
			k++;
		CHECK(k == ns[i], "ft_memset(p, 'z', %zu) : octet %zu non rempli",
			ns[i], k);
	}
}

TEST(1, ft_bzero, comparaison_libc)
{
	static unsigned char	a[1200];
	static unsigned char	b[1200];
	size_t					ns[] = {0, 1, 2, 3, 7, 8, 9, 16, 17, 64, 100, 1000};
	size_t					offs[] = {0, 1, 5};

	for (size_t o = 0; o < N(offs); o++)
		for (size_t i = 0; i < N(ns); i++)
		{
			fill_pattern(a, b, sizeof a);
			bzero(a + offs[o], ns[i]);
			STEP("ft_bzero(buf+%zu, %zu)", offs[o], ns[i]);
			ft_bzero(b + offs[o], ns[i]);
			long d = t_memdiff(a, b, sizeof a);
			CHECK(d < 0, "ft_bzero(buf+%zu, %zu) : différent de bzero à "
				"l'octet %ld%s", offs[o], ns[i], d - (long)offs[o],
				d >= (long)(offs[o] + ns[i]) ? " — écriture au-delà de n !" : "");
		}
}

TEST(1, ft_bzero, limites_strictes)
{
	size_t	ns[] = {0, 1, 7, 4096, 10000};

	for (size_t i = 0; i < N(ns); i++)
	{
		unsigned char	*p = t_guard_after(ns[i]);

		memset(p, 'x', ns[i]);
		STEP("ft_bzero(p, %zu) sur un buffer d'exactement %zu octets",
			ns[i], ns[i]);
		ft_bzero(p, ns[i]);
		size_t k = 0;
		while (k < ns[i] && p[k] == 0)
			k++;
		CHECK(k == ns[i], "ft_bzero(p, %zu) : octet %zu non mis à 0", ns[i], k);
	}
}

/* ====================================================================== */
/* memcpy / memmove                                                         */
/* ====================================================================== */
static void	fill_src(unsigned char *s, size_t n)
{
	for (size_t k = 0; k < n; k++)
		s[k] = (k % 5 == 0) ? 0 : (unsigned char)(k * 13 + 7);
}

TEST(1, ft_memcpy, comparaison_libc)
{
	static unsigned char	src[1100];
	static unsigned char	a[1200];
	static unsigned char	b[1200];
	size_t					ns[] = {0, 1, 2, 3, 4, 7, 8, 9, 15, 16, 17, 33, 64,
		100, 1000};
	size_t					offs[] = {0, 1, 3};

	fill_src(src, sizeof src);
	for (size_t i = 0; i < N(ns); i++)
		for (size_t o = 0; o < N(offs); o++)
		{
			fill_pattern(a, b, sizeof a);
			memcpy(a + offs[o], src + (2 - o), ns[i]);
			STEP("ft_memcpy(dst+%zu, src+%zu, %zu)", offs[o], 2 - o, ns[i]);
			void *r = ft_memcpy(b + offs[o], src + (2 - o), ns[i]);
			CHECK(r == b + offs[o], "ft_memcpy(dst, src, %zu) : doit renvoyer "
				"dst, obtenu %s", ns[i], RP(r, b + offs[o]));
			long d = t_memdiff(a, b, sizeof a);
			CHECK(d < 0, "ft_memcpy(dst, src, %zu) : différent de memcpy à "
				"l'octet %ld%s", ns[i], d - (long)offs[o],
				d >= (long)(offs[o] + ns[i]) ? " — écriture au-delà de n !"
				: " (la source contient des octets nuls : memcpy ne s'arrête "
				"PAS au '\\0')");
		}
}

TEST(1, ft_memcpy, octets_nuls_au_milieu)
{
	char	dst[8];

	memset(dst, 'Z', sizeof dst);
	STEP("ft_memcpy(dst, \"ab\\0cd\", 6)");
	ft_memcpy(dst, "ab\0cd", 6);
	CHECK(memcmp(dst, "ab\0cd\0ZZ", 8) == 0, "ft_memcpy(dst, \"ab\\0cd\", 6) : "
		"dst = %s au lieu de \"ab\\0cd\\0ZZ\" (ne pas s'arrêter au '\\0', ne pas "
		"dépasser n)", RM(dst, 8));
}

TEST(1, ft_memcpy, limites_strictes)
{
	size_t	ns[] = {0, 1, 3, 4096, 5000, 100000};
	static unsigned char	ref[100000];

	fill_src(ref, sizeof ref);
	for (size_t i = 0; i < N(ns); i++)
	{
		unsigned char	*s = t_gmem(ref, ns[i]);
		unsigned char	*d = t_guard_after(ns[i]);

		STEP("ft_memcpy(dst, src, %zu) — dst et src font exactement %zu octets "
			"(tout accès au-delà crashe)", ns[i], ns[i]);
		ft_memcpy(d, s, ns[i]);
		CHECK(t_memdiff(d, ref, ns[i]) < 0, "ft_memcpy(dst, src, %zu) : copie "
			"incorrecte", ns[i]);
	}
}

SOFT_TEST(1, ft_memcpy, null_null)
{
	STEP("ft_memcpy(NULL, NULL, 0)");
	void *r = ft_memcpy(NULL, NULL, 0);
	CHECK(r == NULL, "ft_memcpy(NULL, NULL, 0) : NULL attendu");
	STEP("ft_memcpy(NULL, NULL, 3) (comportement indéfini, mais souvent testé "
		"en correction)");
	r = ft_memcpy(NULL, NULL, 3);
	CHECK(r == NULL, "ft_memcpy(NULL, NULL, 3) : NULL attendu sans crash");
}

TEST(1, ft_memmove, chevauchements_exhaustifs)
{
	static unsigned char	a[160];
	static unsigned char	b[160];
	size_t					ns[] = {0, 1, 2, 3, 5, 8, 13, 16, 21, 40};

	for (int so = 0; so < 24; so++)
		for (int dof = 0; dof < 24; dof++)
			for (size_t i = 0; i < N(ns); i++)
			{
				fill_src(a, sizeof a);
				fill_src(b, sizeof b);
				memmove(a + 30 + dof, a + 30 + so, ns[i]);
				STEP("ft_memmove(buf+%d, buf+%d, %zu)", dof, so, ns[i]);
				void *r = ft_memmove(b + 30 + dof, b + 30 + so, ns[i]);
				CHECK(r == b + 30 + dof, "ft_memmove(buf+%d, buf+%d, %zu) : doit "
					"renvoyer dst", dof, so, ns[i]);
				long d = t_memdiff(a, b, sizeof a);
				CHECK(d < 0, "ft_memmove(buf+%d, buf+%d, %zu) : différent de "
					"memmove%s", dof, so, ns[i], (dof > so && dof < so + (int)ns[i])
					? " — ici dst est APRÈS src et les zones se chevauchent : "
					"copier du début vers la fin écrase la source avant de "
					"l'avoir lue" : (dof < so && so < dof + (int)ns[i])
					? " — ici dst est AVANT src et les zones se chevauchent"
					: "");
			}
}

TEST(1, ft_memmove, limites_strictes_et_grandes_tailles)
{
	size_t			n = 1 << 20;
	unsigned char	*ref = t_guard_after(n);
	unsigned char	*buf = t_guard_after(n + 1);
	unsigned char	*s;
	unsigned char	*d;

	fill_src(ref, n);
	memcpy(buf, ref, n);
	STEP("ft_memmove(buf+1, buf, %zu) (1 Mo, décalage d'un octet)", n);
	ft_memmove(buf + 1, buf, n);
	CHECK(buf[0] == ref[0] && t_memdiff(buf + 1, ref, n) < 0,
		"ft_memmove(buf+1, buf, 1 Mo) : résultat incorrect");
	memcpy(buf + 1, ref, n);
	STEP("ft_memmove(buf, buf+1, %zu)", n);
	ft_memmove(buf, buf + 1, n);
	CHECK(t_memdiff(buf, ref, n) < 0, "ft_memmove(buf, buf+1, 1 Mo) : résultat "
		"incorrect");
	for (size_t k = 0; k < 4; k++)
	{
		size_t	m = (size_t[]){0, 1, 7, 4096}[k];

		s = t_gmem(ref, m);
		d = t_guard_after(m);
		STEP("ft_memmove(dst, src, %zu) sans chevauchement, buffers exacts", m);
		ft_memmove(d, s, m);
		CHECK(t_memdiff(d, ref, m) < 0, "ft_memmove(dst, src, %zu) : copie "
			"incorrecte", m);
	}
	s = t_gmem(ref, 10);
	STEP("ft_memmove(p, p, 10) (même adresse)");
	CHECK(ft_memmove(s, s, 10) == s && t_memdiff(s, ref, 10) < 0,
		"ft_memmove(p, p, 10) : doit renvoyer p et ne rien changer");
}

SOFT_TEST(1, ft_memmove, null_null)
{
	STEP("ft_memmove(NULL, NULL, 0)");
	CHECK(ft_memmove(NULL, NULL, 0) == NULL, "ft_memmove(NULL, NULL, 0) : "
		"NULL attendu");
	STEP("ft_memmove(NULL, NULL, 5) (comportement indéfini, souvent testé)");
	CHECK(ft_memmove(NULL, NULL, 5) == NULL, "ft_memmove(NULL, NULL, 5) : "
		"NULL attendu sans crash");
}

/* ====================================================================== */
/* strlcpy / strlcat  (valeurs attendues d'après le man BSD)                 */
/* ====================================================================== */
typedef struct s_lcpy
{
	const char	*src;
	size_t		size;
	const char	*exp;
	size_t		ret;
}	t_lcpy;

static void	lcpy_run(const t_lcpy *t, size_t n)
{

	for (size_t i = 0; i < n; i++)
	{
		char	*src = t_gstr(t[i].src);
		char	*dst = t_guard_after(t[i].size);

		memset(dst, 'Z', t[i].size);
		STEP("ft_strlcpy(dst, %s, %zu) — dst fait exactement %zu octet(s), "
			"rien n'est lisible après le '\\0' de src", R(t[i].src), t[i].size,
			t[i].size);
		size_t r = ft_strlcpy(dst, src, t[i].size);
		CHECK(r == t[i].ret, "ft_strlcpy(dst, %s, %zu) : retour attendu %zu "
			"(toujours la longueur de src), obtenu %zu", R(t[i].src), t[i].size,
			t[i].ret, r);
		if (!t[i].exp)
			continue ;
		if (!CHECK(memchr(dst, 0, t[i].size) != NULL, "ft_strlcpy(dst, %s, "
				"%zu) : dst n'est pas terminé par '\\0' (contenu %s)",
				R(t[i].src), t[i].size, RM(dst, t[i].size)))
			continue ;
		CHECK(strcmp(dst, t[i].exp) == 0, "ft_strlcpy(dst, %s, %zu) : dst "
			"attendu %s, obtenu %s", R(t[i].src), t[i].size, R(t[i].exp),
			R(dst));
	}
}

TEST(1, ft_strlcpy, tableau_de_cas)
{
	t_lcpy	t[] = {
		{"hello", 1, "", 5},
		{"hello", 2, "h", 5},
		{"hello", 5, "hell", 5},
		{"hello", 6, "hello", 5},
		{"hello", 100, "hello", 5},
		{"", 1, "", 0},
		{"", 42, "", 0},
		{"a", 1, "", 1},
		{"a", 2, "a", 1},
		{"lorem ipsum dolor", 15, "lorem ipsum do", 17},
		{"\x80\xff\x01z", 3, "\x80\xff", 4},
	};

	lcpy_run(t, N(t));
}

/* size == 0 : rien ne doit être écrit (dst pointe sur une zone inaccessible) */
TEST(1, ft_strlcpy, size_zero)
{
	t_lcpy	t[] = {{"hello", 0, NULL, 5}, {"", 0, NULL, 0}};

	lcpy_run(t, N(t));
}

TEST(1, ft_strlcpy, longue_source)
{
	size_t	n = 100000;
	char	*big = t_guard_after(n + 1);
	char	dst[11];

	memset(big, 'k', n);
	big[n] = 0;
	STEP("ft_strlcpy(dst, chaîne de %zu car., 11)", n);
	size_t r = ft_strlcpy(dst, big, 11);
	CHECK(r == n, "ft_strlcpy(dst, src de %zu car., 11) : retour attendu %zu, "
		"obtenu %zu", n, n, r);
	CHECK(memcmp(dst, "kkkkkkkkkk", 11) == 0, "ft_strlcpy(dst, src, 11) : dst "
		"attendu \"kkkkkkkkkk\", obtenu %s", RM(dst, 11));
}

typedef struct s_lcat
{
	const char	*dst0;
	const char	*src;
	size_t		size;
	const char	*exp;
	size_t		ret;
}	t_lcat;

static void	lcat_run(const t_lcat *t, size_t n)
{
	for (size_t i = 0; i < n; i++)
	{
		size_t	dl = strlen(t[i].dst0);
		char	*src = t_gstr(t[i].src);
		char	*dst = t_guard_after(t[i].size);
		char	init[64];

		for (size_t k = 0; k < t[i].size; k++)
			dst[k] = k <= dl ? t[i].dst0[k] : 'Z';
		memcpy(init, dst, t[i].size);
		STEP("ft_strlcat(dst=%s, %s, %zu) — dst fait exactement %zu octet(s)%s",
			R(t[i].dst0), R(t[i].src), t[i].size, t[i].size, t[i].size <= dl
			? " et n'est PAS terminé dans ces octets : interdit de lire plus "
			"loin que size" : "");
		size_t r = ft_strlcat(dst, src, t[i].size);
		CHECK(r == t[i].ret, "ft_strlcat(dst=%s, %s, %zu) : retour attendu %zu, "
			"obtenu %zu%s", R(t[i].dst0), R(t[i].src), t[i].size, t[i].ret, r,
			t[i].size <= dl ? " (quand size <= strlen(dst), le retour vaut "
			"size + strlen(src))" : " (strlen(dst) initiale + strlen(src))");
		if (!t[i].exp)
		{
			CHECK(t_memdiff(dst, init, t[i].size) < 0, "ft_strlcat(dst=%s, %s, "
				"%zu) : dst ne doit pas être modifié, devenu %s", R(t[i].dst0),
				R(t[i].src), t[i].size, RM(dst, t[i].size));
			continue ;
		}
		if (!CHECK(memchr(dst, 0, t[i].size) != NULL, "ft_strlcat(dst=%s, %s, "
				"%zu) : résultat non terminé par '\\0' (%s)", R(t[i].dst0),
				R(t[i].src), t[i].size, RM(dst, t[i].size)))
			continue ;
		CHECK(strcmp(dst, t[i].exp) == 0, "ft_strlcat(dst=%s, %s, %zu) : dst "
			"attendu %s, obtenu %s", R(t[i].dst0), R(t[i].src), t[i].size,
			R(t[i].exp), R(dst));
	}
}

/* size > strlen(dst) : il y a de la place pour concaténer (au moins le '\0') */
TEST(1, ft_strlcat, cas_normaux)
{
	t_lcat	t[] = {
		{"abc", "defgh", 4, "abc", 8},
		{"abc", "defgh", 5, "abcd", 8},
		{"abc", "defgh", 6, "abcde", 8},
		{"abc", "defgh", 8, "abcdefg", 8},
		{"abc", "defgh", 9, "abcdefgh", 8},
		{"abc", "defgh", 20, "abcdefgh", 8},
		{"", "hello", 1, "", 5},
		{"", "hello", 3, "he", 5},
		{"", "hello", 6, "hello", 5},
		{"", "hello", 10, "hello", 5},
		{"hello", "", 6, "hello", 5},
		{"hello", "", 10, "hello", 5},
		{"", "", 1, "", 0},
		{"rrrrrr", "lorem ipsum dolor sit amet", 15, "rrrrrrlorem ip", 32},
		{"\xff\x80", "\x01\xfe", 4, "\xff\x80\x01", 4},
	};

	lcat_run(t, N(t));
}

/* size <= strlen(dst) : dst ne contient pas de '\0' dans ses size premiers
** octets. strlcat ne doit RIEN écrire et ne rien lire au-delà de size. */
TEST(1, ft_strlcat, size_trop_petit)
{
	t_lcat	t[] = {
		{"abc", "defgh", 3, NULL, 8},
		{"abc", "defgh", 2, NULL, 7},
		{"abc", "defgh", 1, NULL, 6},
		{"hello", "", 3, NULL, 3},
	};

	lcat_run(t, N(t));
}

TEST(1, ft_strlcat, size_zero)
{
	t_lcat	t[] = {
		{"abc", "defgh", 0, NULL, 5},
		{"", "hello", 0, NULL, 5},
		{"", "", 0, NULL, 0},
	};

	lcat_run(t, N(t));
}

/* ====================================================================== */
/* strchr / strrchr                                                         */
/* ====================================================================== */
static const char	*g_chr_strings[] = {"hello world", "", "a", "abcabc",
	"\x80\xff\x7f\x01 é", "  \t\n", "zzzzz"};

static void	chr_cmp(const char *name, char *(*ft)(const char *, int),
	char *(*ref)(const char *, int), const char *g, int c)
{
	char	*exp = ref(g, c);

	STEP("%s(%s, %s)", name, R(g), RC(c));
	char *got = ft(g, c);
	CHECK(got == exp, "%s(%s, %s) : attendu %s, obtenu %s%s", name, R(g), RC(c),
		RP(exp, g), RP(got, g), c == 0 ? " — chercher '\\0' doit renvoyer un "
		"pointeur sur le '\\0' final" : (c < 0 || c > 255)
		? " — c doit être converti en char avant la comparaison" : "");
}

static char	*libc_strchr(const char *s, int c)
{
	return (strchr(s, c));
}

static char	*libc_strrchr(const char *s, int c)
{
	return (strrchr(s, c));
}

static void	chr_all(const char *name, char *(*ft)(const char *, int),
	char *(*ref)(const char *, int), int front)
{
	int	extra[] = {256 + 'o', 512 + 'l', 1024, -256 + 'e', 256, INT_MIN + 'a'};

	for (size_t i = 0; i < N(g_chr_strings); i++)
	{
		const char	*g = front ? t_gstr_front(g_chr_strings[i])
			: t_rostr(g_chr_strings[i]);

		for (int c = -1; c <= 255; c++)
			chr_cmp(name, ft, ref, g, c);
		for (size_t k = 0; k < N(extra); k++)
			chr_cmp(name, ft, ref, g, extra[k]);
	}
}

TEST(1, ft_strchr, comparaison_libc)
{
	chr_all("ft_strchr", ft_strchr, libc_strchr, 0);
}

TEST(1, ft_strrchr, comparaison_libc)
{
	chr_all("ft_strrchr", ft_strrchr, libc_strrchr, 0);
}

TEST(1, ft_strrchr, rien_lisible_avant_la_chaine)
{
	chr_all("ft_strrchr", ft_strrchr, libc_strrchr, 1);
}

TEST(1, ft_strchr, longue_chaine)
{
	size_t	n = 1000000;
	char	*s = t_guard_after(n + 1);

	memset(s, 'a', n);
	s[n - 1] = 'b';
	s[n] = 0;
	STEP("ft_strchr(1 Mo de 'a' + 'b', 'b')");
	CHECK(ft_strchr(s, 'b') == s + n - 1, "ft_strchr sur 1 Mo : mauvais "
		"résultat");
}

TEST(1, ft_strrchr, longue_chaine)
{
	size_t	n = 1000000;
	char	*s = t_guard_after(n + 1);

	memset(s, 'a', n);
	s[0] = 'b';
	s[n] = 0;
	STEP("ft_strrchr(1 Mo, 'a')");
	CHECK(ft_strrchr(s, 'a') == s + n - 1, "ft_strrchr sur 1 Mo : mauvais "
		"résultat");
	STEP("ft_strrchr(1 Mo, 'b')");
	CHECK(ft_strrchr(s, 'b') == s, "ft_strrchr sur 1 Mo : mauvais résultat");
}

/* ====================================================================== */
/* strncmp / memcmp                                                         */
/* ====================================================================== */
typedef struct s_cmp
{
	const char	*a;
	const char	*b;
	size_t		n;
}	t_cmp;

TEST(1, ft_strncmp, comparaison_libc)
{
	t_cmp	t[] = {
		{"abc", "abc", 3}, {"abc", "abc", 4}, {"abc", "abc", 100},
		{"abc", "abd", 2}, {"abc", "abd", 3}, {"abd", "abc", 3},
		{"abc", "ab", 3}, {"ab", "abc", 3}, {"abc", "ab", 2},
		{"", "", 0}, {"", "", 5}, {"a", "", 1}, {"", "a", 1},
		{"\x80", "", 1}, {"", "\x80", 1}, {"\xff", "\x01", 1},
		{"\x01", "\xff", 1}, {"test\200", "test\0", 6},
		{"test\200", "test\0", 5}, {"abc", "xyz", 0},
		{"abcdef", "abcxyz", 3}, {"abcdef", "abcxyz", 4},
		{"hello", "hello", SIZE_MAX}, {"hello", "hellz", SIZE_MAX},
		{"a\0b", "a\0c", 3}, {"zz", "zzzzzzz", 2},
	};

	for (size_t i = 0; i < N(t); i++)
	{
		char	*a = t_rostr(t[i].a);
		char	*b = t_rostr(t[i].b);
		int		exp = t_sign(strncmp(t[i].a, t[i].b, t[i].n));

		STEP("ft_strncmp(%s, %s, %zu)", R(t[i].a), R(t[i].b), t[i].n);
		int got = ft_strncmp(a, b, t[i].n);
		CHECK(t_sign(got) == exp, "ft_strncmp(%s, %s, %zu) : signe attendu %d, "
			"obtenu %d (valeur %d)%s", R(t[i].a), R(t[i].b), t[i].n, exp,
			t_sign(got), got, ((unsigned char)t[i].a[0] > 127
			|| (unsigned char)t[i].b[0] > 127 || strchr(t[i].a, '\200'))
			? " — compare les octets en unsigned char" : "");
	}
}

TEST(1, ft_strncmp, ne_lit_pas_au_dela_de_n)
{
	char	*a = t_gmem("abcd", 4);
	char	*b = t_gmem("abcd", 4);
	char	*c = t_gmem("abcX", 4);

	STEP("ft_strncmp(a, b, 4) — a et b font 4 octets sans '\\0' : l'octet "
		"d'indice 4 ne doit pas être lu");
	CHECK(ft_strncmp(a, b, 4) == 0, "ft_strncmp(\"abcd\", \"abcd\", 4) : 0 "
		"attendu");
	STEP("ft_strncmp(a, c, 3) — buffers de 4 octets sans '\\0'");
	CHECK(ft_strncmp(a, c, 3) == 0, "ft_strncmp(\"abcd\", \"abcX\", 3) : 0 "
		"attendu");
	STEP("ft_strncmp(a, c, 4)");
	CHECK(ft_strncmp(a, c, 4) > 0, "ft_strncmp(\"abcd\", \"abcX\", 4) : "
		"positif attendu");
}

SOFT_TEST(1, ft_strncmp, n_zero_ne_lit_rien)
{
	char	*inacc = t_guard_after(0);

	STEP("ft_strncmp(p, p, 0) avec p inaccessible");
	CHECK(ft_strncmp(inacc, inacc, 0) == 0, "n == 0 : 0 attendu");
}

TEST(1, ft_memcmp, comparaison_libc)
{
	t_cmp	t[] = {
		{"abc", "abc", 3}, {"abc", "abd", 3}, {"abd", "abc", 3},
		{"abc", "abd", 2}, {"ab\0c", "ab\0d", 4}, {"ab\0d", "ab\0c", 4},
		{"ab\0c", "ab\0d", 3}, {"\x80", "\0", 1}, {"\0", "\x80", 1},
		{"\xff", "\x7f", 1}, {"xyz", "abc", 0}, {"\0\0\0\0", "\0\0\0\x01", 4},
		{"0123456789", "0123456789", 10}, {"0123456789", "012345678A", 10},
	};
	size_t	lens[] = {3, 3, 3, 3, 4, 4, 4, 1, 1, 1, 3, 4, 10, 10};

	for (size_t i = 0; i < N(t); i++)
	{
		void	*a = t_gmem(t[i].a, t[i].n);
		void	*b = t_gmem(t[i].b, t[i].n);
		int		exp = t_sign(memcmp(t[i].a, t[i].b, t[i].n));

		(void)lens;
		STEP("ft_memcmp(%s, %s, %zu) — buffers d'exactement n octets",
			RM(t[i].a, t[i].n), RM(t[i].b, t[i].n), t[i].n);
		int got = ft_memcmp(a, b, t[i].n);
		CHECK(t_sign(got) == exp, "ft_memcmp(%s, %s, %zu) : signe attendu %d, "
			"obtenu %d%s", RM(t[i].a, t[i].n), RM(t[i].b, t[i].n), t[i].n, exp,
			t_sign(got), memchr(t[i].a, 0, t[i].n) ? " — memcmp ne s'arrête "
			"pas au '\\0'" : " — compare en unsigned char");
	}
}

TEST(1, ft_memcmp, grand_buffer)
{
	size_t			n = 1 << 20;
	unsigned char	*a = t_guard_after(n);
	unsigned char	*b = t_guard_after(n);

	fill_src(a, n);
	memcpy(b, a, n);
	STEP("ft_memcmp(a, b, 1 Mo) identiques");
	CHECK(ft_memcmp(a, b, n) == 0, "ft_memcmp sur 1 Mo identique : 0 attendu");
	b[n - 1] ^= 0x80;
	int exp = t_sign(memcmp(a, b, n));
	STEP("ft_memcmp(a, b, 1 Mo) différence au dernier octet");
	CHECK(t_sign(ft_memcmp(a, b, n)) == exp, "ft_memcmp : différence au "
		"dernier octet non détectée ou mauvais signe");
}

/* ====================================================================== */
/* memchr                                                                   */
/* ====================================================================== */
TEST(1, ft_memchr, comparaison_libc)
{
	const unsigned char	data[] = "abc\0def\xff\x80zz\0";
	size_t				ns[] = {0, 1, 3, 4, 8, 9, 12};
	int					extra[] = {256 + 'd', 512, -1, -128, 1024 + 'z'};

	for (size_t i = 0; i < N(ns); i++)
	{
		unsigned char	*m = t_gmem(data, ns[i]);

		for (int c = -1; c < 256 + (int)N(extra); c++)
		{
			int		cc = c <= 255 ? c : extra[c - 256];
			void	*exp = memchr(m, cc, ns[i]);

			STEP("ft_memchr(%s, %s, %zu) — buffer d'exactement n octets",
				RM(data, ns[i]), RC(cc), ns[i]);
			void *got = ft_memchr(m, cc, ns[i]);
			CHECK(got == exp, "ft_memchr(%s, %s, %zu) : attendu %s, obtenu "
				"%s%s", RM(data, ns[i]), RC(cc), ns[i], RP(exp, m), RP(got, m),
				(cc < 0 || cc > 255) ? " — c doit être converti en unsigned "
				"char" : "");
		}
	}
}

/* ====================================================================== */
/* strnstr (attendus écrits à la main, d'après le man BSD)                   */
/* ====================================================================== */
typedef struct s_nstr
{
	const char	*big;
	const char	*little;
	size_t		len;
	long		exp;
}	t_nstr;

TEST(1, ft_strnstr, tableau_de_cas)
{
	t_nstr	t[] = {
		{"hello world", "world", 11, 6}, {"hello world", "world", 10, -1},
		{"hello world", "world", 100, 6}, {"hello world", "o", 5, 4},
		{"hello world", "o", 4, -1}, {"hello world", "", 0, 0},
		{"hello world", "", 5, 0}, {"", "", 0, 0}, {"", "", 10, 0},
		{"", "a", 1, -1}, {"abc", "abcd", 10, -1}, {"aaab", "aab", 4, 1},
		{"aaab", "aab", 3, -1}, {"lorem ipsum dolor sit", "dolor", 17, 12},
		{"lorem ipsum dolor sit", "dolor", 16, -1}, {"abc", "c", SIZE_MAX, 2},
		{"abc", "abc", 3, 0}, {"abc", "abc", 2, -1},
		{"mississippi", "issip", 11, 4}, {"mississippi", "issipi", 11, -1},
		{"abc", "b", 0, -1}, {"aaaaaaaab", "aaab", 9, 5},
		{"\xff\x80\xff", "\x80\xff", 3, 1}, {"abcabc", "cab", SIZE_MAX, 2},
	};

	for (size_t i = 0; i < N(t); i++)
	{
		char	*big = t_rostr(t[i].big);
		char	*lit = t_rostr(t[i].little);

		STEP("ft_strnstr(%s, %s, %zu)", R(t[i].big), R(t[i].little), t[i].len);
		char *got = ft_strnstr(big, lit, t[i].len);
		CHECK(got == (t[i].exp < 0 ? NULL : big + t[i].exp), "ft_strnstr(%s, "
			"%s, %zu) : attendu %s, obtenu %s%s", R(t[i].big), R(t[i].little),
			t[i].len, t[i].exp < 0 ? "NULL" : RP(big + t[i].exp, big),
			RP(got, big), !t[i].little[0] ? " — little vide : renvoyer big"
			: t[i].exp < 0 ? " — l'occurrence dépasse len (ou n'existe pas)"
			: "");
	}
}

SOFT_TEST(1, ft_strnstr, big_non_termine)
{
	char	*big = t_gmem("abcdef", 6);

	STEP("ft_strnstr(big, \"xyz\", 6) — big = 6 octets SANS '\\0' : on ne "
		"doit jamais lire big[6]");
	CHECK(ft_strnstr(big, "xyz", 6) == NULL, "NULL attendu");
	STEP("ft_strnstr(big, \"ef\", 6) — big sans '\\0'");
	CHECK(ft_strnstr(big, "ef", 6) == big + 4, "s+4 attendu");
}

/* ====================================================================== */
/* atoi                                                                     */
/* ====================================================================== */
static void	atoi_cmp(const char *s)
{
	char	*g = t_rostr(s);
	int		exp = atoi(s);

	STEP("ft_atoi(%s)", R(s));
	int got = ft_atoi(g);
	CHECK(got == exp, "ft_atoi(%s) : attendu %d, obtenu %d", R(s), exp, got);
}

TEST(1, ft_atoi, cas_classiques)
{
	const char	*t[] = {"0", "42", "-42", "+42", "   42", "\t\n\v\f\r 42",
		" \t-0042abc", "+-42", "-+42", "--42", "++42", "42abc", "abc", "",
		" ", "- 42", "+ 42", " 12 34", "2147483647", "-2147483648", "0000042",
		"-0", "+0", "\x1b" "42", "4\x80" "2", "1", "-1", "9", "-9", "10",
		"999999999", "-999999999", "1000000000", "-1000000000", "\n\n\n-7x",
		"  +1", "0x1A", "  \t \t  -2147483647", "123456789"};

	for (size_t i = 0; i < N(t); i++)
		atoi_cmp(t[i]);
}

TEST(1, ft_atoi, nombres_aleatoires)
{
	char			buf[64];
	unsigned int	seed = 4242;
	const char		*ws[] = {"", " ", "\t", "\n", "\v", "\f", "\r", "  \t"};

	for (int i = 0; i < 2000; i++)
	{
		seed = seed * 1103515245u + 12345u;
		int v = (int)(seed ^ (seed >> 7));
		if (i % 3 == 0)
			v %= 100000;
		snprintf(buf, sizeof buf, "%s%s%d%s", ws[i % N(ws)],
			(v >= 0 && i % 5 == 0) ? "+" : "", v, i % 4 ? "" : "xyz");
		atoi_cmp(buf);
	}
}

/* ====================================================================== */
/* calloc                                                                   */
/* ====================================================================== */
TEST(1, ft_calloc, mise_a_zero)
{
	size_t	t[][2] = {{1, 1}, {5, 4}, {4, 5}, {1, 100}, {100, 1}, {7, 3},
		{1000, 1000}, {3, 8}};

	for (size_t i = 0; i < N(t); i++)
	{
		size_t	n = t[i][0] * t[i][1];

		STEP("ft_calloc(%zu, %zu)", t[i][0], t[i][1]);
		unsigned char *p = ft_calloc(t[i][0], t[i][1]);
		if (!CHECK(p != NULL, "ft_calloc(%zu, %zu) : NULL inattendu", t[i][0],
				t[i][1]))
			continue ;
		CHECK(t_block_size(p) >= n || !t_is_live(p), "ft_calloc(%zu, %zu) : "
			"bloc de %zu octets alloué, %zu nécessaires", t[i][0], t[i][1],
			t_block_size(p), n);
		CHECK(t_is_live(p), "ft_calloc(%zu, %zu) : le pointeur ne vient pas de "
			"malloc (ou pas du début du bloc)", t[i][0], t[i][1]);
		size_t k = 0;
		while (k < n && p[k] == 0)
			k++;
		CHECK(k == n, "ft_calloc(%zu, %zu) : octet %zu non mis à zéro (malloc "
			"ne met PAS la mémoire à 0)", t[i][0], t[i][1], k);
		memset(p, 'w', n);
		free(p);
	}
}

TEST(1, ft_calloc, taille_zero)
{
	size_t	t[][2] = {{0, 0}, {0, 5}, {5, 0}, {0, SIZE_MAX}, {SIZE_MAX, 0}};

	for (size_t i = 0; i < N(t); i++)
	{
		STEP("ft_calloc(%zu, %zu)", t[i][0], t[i][1]);
		void *p = ft_calloc(t[i][0], t[i][1]);
		STEP("ft_calloc(%zu, %zu) (2e appel)", t[i][0], t[i][1]);
		void *q = ft_calloc(t[i][0], t[i][1]);
		CHECK(p != NULL && q != NULL, "ft_calloc(%zu, %zu) : le sujet exige un "
			"pointeur unique, non NULL, qu'on peut passer à free()",
			t[i][0], t[i][1]);
		CHECK(p != q || !p, "ft_calloc(%zu, %zu) : deux appels ont renvoyé le "
			"même pointeur (il doit être unique)", t[i][0], t[i][1]);
		STEP("free(ft_calloc(%zu, %zu))", t[i][0], t[i][1]);
		free(p);
		free(q);
	}
}

TEST(1, ft_calloc, depassement_de_capacite)
{
	size_t	t[][2] = {{SIZE_MAX, 2}, {2, SIZE_MAX}, {SIZE_MAX / 2 + 1, 2},
		{(size_t)1 << 33, (size_t)1 << 33}, {SIZE_MAX, SIZE_MAX},
		{SIZE_MAX / 4 + 1, 8}};

	for (size_t i = 0; i < N(t); i++)
	{
		STEP("ft_calloc(%zu, %zu)", t[i][0], t[i][1]);
		void *p = ft_calloc(t[i][0], t[i][1]);
		CHECK(p == NULL, "ft_calloc(%zu, %zu) : nmemb * size dépasse SIZE_MAX "
			"-> NULL attendu (sinon la multiplication « boucle » et on "
			"obtient un bloc minuscule)", t[i][0], t[i][1]);
		free(p);
	}
}

TEST(1, ft_calloc, echec_de_malloc)
{
	FAIL_LOOP(void *, r, ft_calloc(10, 4), free(r), "ft_calloc(10, 4)%s", "");
}

/* ====================================================================== */
/* strdup                                                                   */
/* ====================================================================== */
TEST(1, ft_strdup, copies)
{
	const char	*t[] = {"", "a", "hello", "hello world !", "\x80\xff\x01",
		"é ü ß", "\t\n"};

	for (size_t i = 0; i < N(t); i++)
	{
		char	*s = t_rostr(t[i]);

		STEP("ft_strdup(%s)", R(t[i]));
		char *d = ft_strdup(s);
		if (!CHECK(d != NULL, "ft_strdup(%s) : NULL inattendu", R(t[i])))
			continue ;
		CHECK(d != s, "ft_strdup(%s) : doit renvoyer une NOUVELLE chaîne",
			R(t[i]));
		CHECK(t_is_live(d), "ft_strdup(%s) : le résultat ne vient pas de malloc",
			R(t[i]));
		CHECK(strcmp(d, t[i]) == 0, "ft_strdup(%s) : obtenu %s", R(t[i]), R(d));
		free(d);
	}
}

TEST(1, ft_strdup, longue_chaine)
{
	size_t	n = 200000;
	char	*s = t_guard_after(n + 1);

	memset(s, 'q', n);
	s[n] = 0;
	STEP("ft_strdup(chaîne de %zu car.)", n);
	char *d = ft_strdup(s);
	if (CHECK(d != NULL, "ft_strdup(longue) : NULL inattendu"))
	{
		CHECK(strcmp(d, s) == 0, "ft_strdup(longue) : copie incorrecte");
		free(d);
	}
}

TEST(1, ft_strdup, echec_de_malloc)
{
	char	*s = t_rostr("bonjour");

	FAIL_LOOP(char *, r, ft_strdup(s), free(r), "ft_strdup(%s)", R(s));
}

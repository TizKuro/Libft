#ifndef TESTER_H
# define TESTER_H

# ifndef _GNU_SOURCE
#  define _GNU_SOURCE
# endif
# include <stddef.h>
# include <stdint.h>
# include <limits.h>
# include <string.h>
# include <stdlib.h>
# include <stdio.h>
# include "protos.h"

/* ---------------------------------------------------------------- */
/* Déclaration des tests                                            */
/* ---------------------------------------------------------------- */
typedef void	(*t_testfn)(void);

void	t_register(int part, const char *func, void *addr,
			const char *name, t_testfn fn, int soft);

# define TEST_IMPL(part, func, name, soft) \
	static void test_##func##__##name(void); \
	__attribute__((constructor)) static void reg_##func##__##name(void) \
	{ t_register(part, #func, (void *)(func), #name, \
		test_##func##__##name, soft); } \
	static void test_##func##__##name(void)

/* TEST      : comportement exigé (sujet / man) -> compte dans le score  */
/* SOFT_TEST : non exigé (NULL, hors plage…)   -> simple avertissement  */
# define TEST(part, func, name) TEST_IMPL(part, func, name, 0)
# define SOFT_TEST(part, func, name) TEST_IMPL(part, func, name, 1)

/* ---------------------------------------------------------------- */
/* Vérifications                                                    */
/* ---------------------------------------------------------------- */
int		t_ok(int cond, int line, const char *fmt, ...)
			__attribute__((format(printf, 3, 4)));
void	t_warn(int line, const char *fmt, ...)
			__attribute__((format(printf, 2, 3)));
void	t_step(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

# define CHECK(cond, ...) t_ok(!!(cond), __LINE__, __VA_ARGS__)
# define WARN(...) t_warn(__LINE__, __VA_ARGS__)
/* STEP : décrit l'appel en cours, affiché si ça crashe */
# define STEP(...) t_step(__VA_ARGS__)

/* Représentations lisibles (buffers tournants, pas besoin de free) */
const char	*R(const char *s);
const char	*RM(const void *p, size_t n);
const char	*RC(int c);
const char	*RP(const void *p, const void *base);
int			t_sign(long long x);
long		t_memdiff(const void *a, const void *b, size_t n);

/* ---------------------------------------------------------------- */
/* Mémoire (malloc/free sont interceptés)                           */
/* ---------------------------------------------------------------- */
unsigned long	t_serial(void);
int				t_leaks_since(unsigned long serial, int line,
					const char *fmt, ...)
					__attribute__((format(printf, 3, 4)));
int				t_is_live(const void *p);
size_t			t_block_size(const void *p);
void			t_fail_malloc_at(long n);
int				t_fail_triggered(void);
long			t_malloc_count(void);
size_t			t_max_request(void);
void			t_reset_counters(void);
void			*t_malloc_nofail(size_t n);

/*
** FAIL_LOOP : rejoue CALL en faisant échouer le 1er malloc, puis le 2e,
** etc. À chaque échec : CALL doit renvoyer NULL et ne rien laisser fuir.
** S'arrête quand un appel aboutit sans avoir subi d'échec (FREE_RES
** libère alors le résultat).
*/
# define FAIL_LOOP(TYPE, RES, CALL, FREE_RES, FMT, ...) do { \
	long k_ = 0; \
	while (k_ < 400) { \
		unsigned long s_ = t_serial(); \
		STEP(FMT " avec échec simulé du malloc n°%ld", __VA_ARGS__, k_ + 1); \
		t_fail_malloc_at(k_); \
		TYPE RES = (CALL); \
		int trig_ = t_fail_triggered(); \
		t_fail_malloc_at(-1); \
		if (!trig_) { \
			CHECK(k_ > 0, FMT " : aucun appel à malloc détecté " \
				"(le résultat doit être alloué avec malloc)", __VA_ARGS__); \
			FREE_RES; \
			break ; \
		} \
		if (!CHECK(RES == NULL, FMT " : le malloc n°%ld a échoué -> NULL " \
			"attendu, obtenu un pointeur non NULL", __VA_ARGS__, k_ + 1)) \
			break ; \
		t_leaks_since(s_, __LINE__, FMT " après l'échec du malloc n°%ld " \
			"(il faut libérer ce qui a déjà été alloué)", __VA_ARGS__, k_ + 1); \
		k_++; \
	} \
} while (0)

/* ---------------------------------------------------------------- */
/* Buffers protégés : tout accès hors limites => crash immédiat      */
/* ---------------------------------------------------------------- */
void	*t_guard_after(size_t n);	/* p[n] est inaccessible          */
void	*t_guard_before(size_t n);	/* p[-1] est inaccessible         */
char	*t_gstr(const char *s);		/* copie, rien de lisible après '\0' */
char	*t_gstr_front(const char *s);	/* copie, rien de lisible avant */
void	*t_gmem(const void *src, size_t n);
void	t_readonly(const void *p, size_t n);
char	*t_rostr(const char *s);	/* t_gstr + lecture seule          */

/* ---------------------------------------------------------------- */
/* Capture de ce qui est écrit sur un fd                             */
/* ---------------------------------------------------------------- */
int			t_cap_open(void);
const char	*t_cap_close(size_t *len);

#endif

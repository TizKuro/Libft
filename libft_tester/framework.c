/*
** Moteur du testeur : enregistrement des tests, exécution isolée (fork),
** interception de malloc/free, buffers protégés, affichage.
*/
#include "tester.h"
#include <stdarg.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>

#define MAX_TESTS 4096
#define LOG_SIZE 65536
#define TIMEOUT_S 5

/* ====================================================================== */
/* État partagé parent <-> enfant                                          */
/* ====================================================================== */
typedef struct s_shared
{
	int		checks;
	int		fails;
	int		warns;
	char	step[700];
	size_t	loglen;
	char	log[LOG_SIZE];
}	t_shared;

static t_shared		*g_sh;
static int			g_soft;
static const char	*g_curname = "";

static void	log_add(const char *kind, int line, const char *msg)
{
	char	buf[1400];
	int		n;

	if (!g_sh)
		return ;
	if (line > 0)
		n = snprintf(buf, sizeof buf, "%s [%s] %s  (tests l.%d)\n",
				kind, g_curname, msg, line);
	else
		n = snprintf(buf, sizeof buf, "%s [%s] %s\n", kind, g_curname, msg);
	if (n < 0)
		return ;
	if ((size_t)n >= sizeof buf)
		n = sizeof buf - 1;
	if (g_sh->loglen + n >= LOG_SIZE - 1)
		return ;
	memcpy(g_sh->log + g_sh->loglen, buf, n);
	g_sh->loglen += n;
	g_sh->log[g_sh->loglen] = 0;
}

int	t_ok(int cond, int line, const char *fmt, ...)
{
	char	msg[1100];
	va_list	ap;

	g_sh->checks++;
	if (cond)
		return (1);
	va_start(ap, fmt);
	vsnprintf(msg, sizeof msg, fmt, ap);
	va_end(ap);
	if (g_soft)
	{
		g_sh->warns++;
		log_add("⚠", line, msg);
	}
	else
	{
		g_sh->fails++;
		log_add("✘", line, msg);
	}
	return (0);
}

void	t_warn(int line, const char *fmt, ...)
{
	char	msg[1100];
	va_list	ap;

	va_start(ap, fmt);
	vsnprintf(msg, sizeof msg, fmt, ap);
	va_end(ap);
	g_sh->warns++;
	log_add("⚠", line, msg);
}

static void	t_fail_internal(const char *fmt, ...)
{
	char	msg[1100];
	char	full[1900];
	va_list	ap;

	va_start(ap, fmt);
	vsnprintf(msg, sizeof msg, fmt, ap);
	va_end(ap);
	snprintf(full, sizeof full, "%s — pendant : %s", msg, g_sh->step);
	g_sh->checks++;
	if (g_soft)
		g_sh->warns++;
	else
		g_sh->fails++;
	log_add(g_soft ? "⚠" : "✘", 0, full);
}

void	t_step(const char *fmt, ...)
{
	va_list	ap;

	va_start(ap, fmt);
	vsnprintf(g_sh->step, sizeof g_sh->step, fmt, ap);
	va_end(ap);
}

/* ====================================================================== */
/* Représentations lisibles                                                */
/* ====================================================================== */
#define NBUF 24
#define RLEN 320

static char	g_rb[NBUF][RLEN];
static int	g_ri;

static char	*rbuf(void)
{
	g_ri = (g_ri + 1) % NBUF;
	return (g_rb[g_ri]);
}

static size_t	put_esc(char *o, size_t k, unsigned char c)
{
	static const char	hex[] = "0123456789abcdef";

	if (c == '\n')
		return (o[k] = '\\', o[k + 1] = 'n', k + 2);
	if (c == '\t')
		return (o[k] = '\\', o[k + 1] = 't', k + 2);
	if (c == '\r')
		return (o[k] = '\\', o[k + 1] = 'r', k + 2);
	if (c == '\v')
		return (o[k] = '\\', o[k + 1] = 'v', k + 2);
	if (c == '\f')
		return (o[k] = '\\', o[k + 1] = 'f', k + 2);
	if (c == 0)
		return (o[k] = '\\', o[k + 1] = '0', k + 2);
	if (c == '"' || c == '\\')
		return (o[k] = '\\', o[k + 1] = c, k + 2);
	if (c < 32 || c >= 127)
	{
		o[k] = '\\';
		o[k + 1] = 'x';
		o[k + 2] = hex[c >> 4];
		o[k + 3] = hex[c & 15];
		return (k + 4);
	}
	o[k] = c;
	return (k + 1);
}

const char	*RM(const void *p, size_t n)
{
	const unsigned char	*s = p;
	char				*o;
	size_t				k;
	size_t				i;

	if (!p)
		return ("NULL");
	o = rbuf();
	k = 0;
	o[k++] = '"';
	for (i = 0; i < n; i++)
	{
		if (k > 200)
		{
			snprintf(o + k, RLEN - k, "\"… (%zu octets)", n);
			return (o);
		}
		k = put_esc(o, k, s[i]);
	}
	o[k++] = '"';
	o[k] = 0;
	return (o);
}

const char	*R(const char *s)
{
	if (!s)
		return ("NULL");
	return (RM(s, strlen(s)));
}

const char	*RC(int c)
{
	char	*o;
	char	tmp[8];
	size_t	k;

	o = rbuf();
	if (c >= 0 && c <= 255)
	{
		k = put_esc(tmp, 0, (unsigned char)c);
		tmp[k] = 0;
		snprintf(o, RLEN, "'%s' (%d)", tmp, c);
	}
	else
		snprintf(o, RLEN, "%d", c);
	return (o);
}

const char	*RP(const void *p, const void *base)
{
	char	*o;

	if (!p)
		return ("NULL");
	o = rbuf();
	snprintf(o, RLEN, "s+%td", (const char *)p - (const char *)base);
	return (o);
}

int	t_sign(long long x)
{
	return ((x > 0) - (x < 0));
}

long	t_memdiff(const void *a, const void *b, size_t n)
{
	const unsigned char	*x = a;
	const unsigned char	*y = b;
	size_t				i;

	for (i = 0; i < n; i++)
		if (x[i] != y[i])
			return ((long)i);
	return (-1);
}

/* ====================================================================== */
/* Interception de malloc / free                                           */
/*  - chaque bloc est suivi (taille, numéro de série)                      */
/*  - 16 octets « canari » après chaque bloc : détecte les débordements    */
/*  - le contenu neuf est rempli de 0xA5 (pas de zéros gratuits)           */
/*  - on peut faire échouer le n-ième malloc                               */
/* ====================================================================== */
void	*__real_malloc(size_t n);
void	__real_free(void *p);
void	*__real_calloc(size_t a, size_t b);
void	*__real_realloc(void *p, size_t n);

#define CANARY 16
#define TBL (1u << 18)
#define HUGE_ALLOC ((size_t)1 << 32)

typedef struct s_blk
{
	void			*p;
	size_t			size;
	unsigned long	serial;
}	t_blk;

static t_blk			g_tbl[TBL];
static unsigned long	g_serial = 1;
static int				g_track;
static long				g_fail_at = -1;
static long				g_mcount;
static int				g_fail_trig;
static size_t			g_maxreq;

static size_t	hsh(const void *p)
{
	uint64_t	x;

	x = (uint64_t)(uintptr_t)p;
	x ^= x >> 17;
	x *= 0x9E3779B97F4A7C15ULL;
	return ((size_t)(x >> 24) & (TBL - 1));
}

static t_blk	*tbl_find(const void *p)
{
	size_t	i;

	i = hsh(p);
	while (g_tbl[i].p)
	{
		if (g_tbl[i].p == p)
			return (&g_tbl[i]);
		i = (i + 1) & (TBL - 1);
	}
	return (NULL);
}

static void	tbl_put(void *p, size_t size)
{
	size_t	i;

	i = hsh(p);
	while (g_tbl[i].p)
		i = (i + 1) & (TBL - 1);
	g_tbl[i].p = p;
	g_tbl[i].size = size;
	g_tbl[i].serial = g_serial++;
}

static void	tbl_del(t_blk *e)
{
	size_t	i;
	size_t	j;
	size_t	k;

	i = (size_t)(e - g_tbl);
	j = i;
	for (;;)
	{
		j = (j + 1) & (TBL - 1);
		if (!g_tbl[j].p)
			break ;
		k = hsh(g_tbl[j].p);
		if ((i <= j) ? (i < k && k <= j) : (i < k || k <= j))
			continue ;
		g_tbl[i] = g_tbl[j];
		i = j;
	}
	g_tbl[i].p = NULL;
}

static void	*track_alloc(size_t n, int zero)
{
	unsigned char	*p;

	if (n > HUGE_ALLOC)
		return (NULL);
	p = __real_malloc(n + CANARY);
	if (!p)
		return (NULL);
	memset(p, zero ? 0 : 0xA5, n);
	memset(p + n, 0xCD, CANARY);
	tbl_put(p, n);
	return (p);
}

void	*__wrap_malloc(size_t n)
{
	if (!g_track)
		return (__real_malloc(n));
	g_mcount++;
	if (n > g_maxreq)
		g_maxreq = n;
	if (g_fail_at >= 0 && g_mcount - 1 == g_fail_at)
	{
		g_fail_trig = 1;
		return (NULL);
	}
	return (track_alloc(n, 0));
}

static int	canary_ok(const t_blk *e)
{
	const unsigned char	*c = (const unsigned char *)e->p + e->size;
	int					i;

	for (i = 0; i < CANARY; i++)
		if (c[i] != 0xCD)
			return (0);
	return (1);
}

void	__wrap_free(void *p)
{
	t_blk	*e;

	if (!p)
		return ;
	if (!g_track)
	{
		__real_free(p);
		return ;
	}
	e = tbl_find(p);
	if (!e)
	{
		t_fail_internal("free() d'un pointeur invalide : double free, "
			"pointeur qui ne vient pas de malloc, ou pointeur décalé "
			"(pas le début du bloc)");
		return ;
	}
	if (!canary_ok(e))
		t_fail_internal("débordement de tas : écriture APRÈS la fin d'un "
			"bloc de %zu octet(s) alloué avec malloc (taille trop petite ? "
			"oubli de la place du '\\0' ?)", e->size);
	tbl_del(e);
	__real_free(p);
}

void	*__wrap_calloc(size_t a, size_t b)
{
	if (!g_track)
		return (__real_calloc(a, b));
	t_fail_internal("appel au calloc de la libc (interdit : utilise malloc)");
	if (b && a > SIZE_MAX / b)
		return (NULL);
	return (track_alloc(a * b, 1));
}

void	*__wrap_realloc(void *p, size_t n)
{
	t_blk	*e;
	void	*q;

	if (!g_track)
		return (__real_realloc(p, n));
	t_fail_internal("appel à realloc (fonction interdite)");
	q = track_alloc(n, 0);
	if (q && p && (e = tbl_find(p)))
	{
		memcpy(q, p, e->size < n ? e->size : n);
		tbl_del(e);
		__real_free(p);
	}
	return (q);
}

unsigned long	t_serial(void)
{
	return (g_serial);
}

int	t_is_live(const void *p)
{
	return (tbl_find(p) != NULL);
}

size_t	t_block_size(const void *p)
{
	t_blk	*e;

	e = tbl_find(p);
	return (e ? e->size : 0);
}

void	t_fail_malloc_at(long n)
{
	g_fail_at = n;
	g_mcount = 0;
	g_fail_trig = 0;
}

int	t_fail_triggered(void)
{
	return (g_fail_trig);
}

long	t_malloc_count(void)
{
	return (g_mcount);
}

size_t	t_max_request(void)
{
	return (g_maxreq);
}

void	t_reset_counters(void)
{
	g_mcount = 0;
	g_maxreq = 0;
}

void	*t_malloc_nofail(size_t n)
{
	return (track_alloc(n, 0));
}

int	t_leaks_since(unsigned long serial, int line, const char *fmt, ...)
{
	char	ctx[900];
	char	sizes[200];
	size_t	k;
	size_t	i;
	int		count;
	size_t	bytes;
	va_list	ap;

	count = 0;
	bytes = 0;
	k = 0;
	sizes[0] = 0;
	for (i = 0; i < TBL; i++)
	{
		if (g_tbl[i].p && g_tbl[i].serial >= serial)
		{
			count++;
			bytes += g_tbl[i].size;
			if (k < 150)
				k += snprintf(sizes + k, sizeof sizes - k, "%s%zu",
						count > 1 ? ", " : "", g_tbl[i].size);
		}
	}
	if (!count)
		return (0);
	if (k >= 150)
		snprintf(sizes + 150, sizeof sizes - 150, "…");
	va_start(ap, fmt);
	vsnprintf(ctx, sizeof ctx, fmt, ap);
	va_end(ap);
	t_ok(0, line, "fuite mémoire : %d bloc(s) non libéré(s), %zu octet(s) "
		"[tailles : %s] — %s", count, bytes, sizes, ctx);
	/* on nettoie pour ne pas signaler la même fuite plusieurs fois */
	for (;;)
	{
		static void	*batch[4096];
		size_t		nb;
		size_t		b;

		nb = 0;
		for (i = 0; i < TBL && nb < 4096; i++)
			if (g_tbl[i].p && g_tbl[i].serial >= serial)
				batch[nb++] = g_tbl[i].p;
		if (!nb)
			break ;
		for (b = 0; b < nb; b++)
		{
			t_blk	*e = tbl_find(batch[b]);

			if (e)
				tbl_del(e);
			__real_free(batch[b]);
		}
	}
	return (count);
}

/* ====================================================================== */
/* Buffers protégés par des pages inaccessibles                            */
/* ====================================================================== */
static size_t	pagesz(void)
{
	static size_t	ps;

	if (!ps)
		ps = (size_t)sysconf(_SC_PAGESIZE);
	return (ps);
}

void	*t_guard_after(size_t n)
{
	size_t			ps;
	size_t			len;
	unsigned char	*base;

	ps = pagesz();
	len = (n + ps - 1) / ps * ps;
	base = mmap(NULL, len + ps, PROT_READ | PROT_WRITE,
			MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (base == MAP_FAILED)
	{
		perror("mmap");
		_exit(90);
	}
	mprotect(base + len, ps, PROT_NONE);
	return (base + len - n);
}

void	*t_guard_before(size_t n)
{
	size_t			ps;
	size_t			len;
	unsigned char	*base;

	ps = pagesz();
	len = (n + ps - 1) / ps * ps;
	if (len == 0)
		len = ps;
	base = mmap(NULL, len + ps, PROT_READ | PROT_WRITE,
			MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (base == MAP_FAILED)
	{
		perror("mmap");
		_exit(90);
	}
	mprotect(base, ps, PROT_NONE);
	return (base + ps);
}

char	*t_gstr(const char *s)
{
	size_t	n;
	char	*p;

	n = strlen(s) + 1;
	p = t_guard_after(n);
	memcpy(p, s, n);
	return (p);
}

char	*t_gstr_front(const char *s)
{
	size_t	n;
	char	*p;

	n = strlen(s) + 1;
	p = t_guard_before(n);
	memcpy(p, s, n);
	return (p);
}

void	*t_gmem(const void *src, size_t n)
{
	void	*p;

	p = t_guard_after(n);
	if (n)
		memcpy(p, src, n);
	return (p);
}

void	t_readonly(const void *p, size_t n)
{
	uintptr_t	a;
	uintptr_t	b;
	size_t		ps;

	ps = pagesz();
	a = (uintptr_t)p / ps * ps;
	b = ((uintptr_t)p + n + ps - 1) / ps * ps;
	if (b > a)
		mprotect((void *)a, b - a, PROT_READ);
}

char	*t_rostr(const char *s)
{
	char	*p;

	p = t_gstr(s);
	t_readonly(p, strlen(s) + 1);
	return (p);
}

/* ====================================================================== */
/* Capture d'un fd (fichier temporaire)                                    */
/* ====================================================================== */
static int	g_capfd = -1;
static char	g_capbuf[1 << 21];

int	t_cap_open(void)
{
	char	tmpl[] = "/tmp/libft_tester_XXXXXX";

	g_capfd = mkstemp(tmpl);
	if (g_capfd < 0)
	{
		perror("mkstemp");
		_exit(91);
	}
	unlink(tmpl);
	return (g_capfd);
}

const char	*t_cap_close(size_t *len)
{
	size_t	n;
	ssize_t	r;

	n = 0;
	lseek(g_capfd, 0, SEEK_SET);
	while (n < sizeof g_capbuf - 1
		&& (r = read(g_capfd, g_capbuf + n, sizeof g_capbuf - 1 - n)) > 0)
		n += (size_t)r;
	close(g_capfd);
	g_capfd = -1;
	g_capbuf[n] = 0;
	*len = n;
	return (g_capbuf);
}

/* ====================================================================== */
/* Enregistrement et exécution                                             */
/* ====================================================================== */
typedef struct s_entry
{
	int			part;
	const char	*func;
	void		*addr;
	const char	*name;
	t_testfn	fn;
	int			soft;
}	t_entry;

static t_entry	g_tests[MAX_TESTS];
static int		g_ntests;

void	t_register(int part, const char *func, void *addr,
			const char *name, t_testfn fn, int soft)
{
	if (g_ntests >= MAX_TESTS)
		return ;
	g_tests[g_ntests++] = (t_entry){part, func, addr, name, fn, soft};
}

static const char	*g_order[] = {
	"ft_isalpha", "ft_isdigit", "ft_isalnum", "ft_isascii", "ft_isprint",
	"ft_strlen", "ft_memset", "ft_bzero", "ft_memcpy", "ft_memmove",
	"ft_strlcpy", "ft_strlcat", "ft_toupper", "ft_tolower", "ft_strchr",
	"ft_strrchr", "ft_strncmp", "ft_memchr", "ft_memcmp", "ft_strnstr",
	"ft_atoi", "ft_calloc", "ft_strdup",
	"ft_substr", "ft_strjoin", "ft_strtrim", "ft_split", "ft_itoa",
	"ft_strmapi", "ft_striteri", "ft_putchar_fd", "ft_putstr_fd",
	"ft_putendl_fd", "ft_putnbr_fd",
	"ft_lstnew", "ft_lstadd_front", "ft_lstsize", "ft_lstlast",
	"ft_lstadd_back", "ft_lstdelone", "ft_lstclear", "ft_lstiter",
	"ft_lstmap", NULL
};

static const char	*g_part_title[] = {"",
	"Partie 1 — fonctions de la libc",
	"Partie 2 — fonctions supplémentaires",
	"Partie 3 — listes chaînées"};

static int	g_color;
#define C_RED (g_color ? "\033[31m" : "")
#define C_GRN (g_color ? "\033[32m" : "")
#define C_YEL (g_color ? "\033[33m" : "")
#define C_BLU (g_color ? "\033[36m" : "")
#define C_DIM (g_color ? "\033[2m" : "")
#define C_BLD (g_color ? "\033[1m" : "")
#define C_RST (g_color ? "\033[0m" : "")

static const char	*sig_hint(int sig)
{
	if (sig == SIGSEGV)
		return ("SIGSEGV (segmentation fault) : accès mémoire invalide — "
			"lecture/écriture hors du buffer ou après le '\\0', écriture dans "
			"une entrée en lecture seule (paramètre const modifié ?), "
			"déréférencement de NULL, ou récursion infinie");
	if (sig == SIGBUS)
		return ("SIGBUS (bus error) : accès mémoire invalide — souvent "
			"écriture dans une chaîne en lecture seule (l'entrée « const » "
			"a été modifiée ?)");
	if (sig == SIGABRT)
		return ("SIGABRT : abort — souvent un double free ou une corruption "
			"du tas détectée par la libc");
	if (sig == SIGFPE)
		return ("SIGFPE : erreur arithmétique (division par zéro ?)");
	if (sig == SIGALRM)
		return ("délai dépassé : boucle infinie probable (ou algorithme "
			"beaucoup trop lent)");
	return (strsignal(sig));
}

typedef struct s_res
{
	int		checks;
	int		fails;
	int		warns;
	int		crashes;
	int		soft_crashes;
	size_t	len;
	char	text[1 << 18];
}	t_res;

static t_res	g_res;

static void	res_add(const char *s)
{
	size_t	n;

	n = strlen(s);
	if (g_res.len + n >= sizeof g_res.text - 1)
		return ;
	memcpy(g_res.text + g_res.len, s, n + 1);
	g_res.len += n;
}

static void	run_one(const t_entry *t)
{
	pid_t	pid;
	int		st;
	char	line[1200];

	memset(g_sh, 0, sizeof *g_sh);
	snprintf(g_sh->step, sizeof g_sh->step, "(début du test)");
	fflush(stdout);
	fflush(stderr);
	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		exit(2);
	}
	if (pid == 0)
	{
		unsigned long	s0;

		g_curname = t->name;
		g_soft = t->soft;
		g_track = 1;
		alarm(getenv("TESTER_TIMEOUT") ? (unsigned)atoi(getenv("TESTER_TIMEOUT")) : TIMEOUT_S);
		s0 = t_serial();
		t->fn();
		snprintf(g_sh->step, sizeof g_sh->step, "(fin du test)");
		t_leaks_since(s0, 0, "à la fin du test (un résultat n'a pas été "
			"libéré, ou la fonction a perdu de la mémoire)");
		_exit(77);
	}
	while (waitpid(pid, &st, 0) < 0 && errno == EINTR)
		;
	g_res.checks += g_sh->checks;
	g_res.fails += g_sh->fails;
	g_res.warns += g_sh->warns;
	res_add(g_sh->log);
	if (WIFEXITED(st) && WEXITSTATUS(st) == 77)
		return ;
	if (t->soft && WIFSIGNALED(st))
		snprintf(line, sizeof line, "⚠ [%s] crash (%s) — non exigé par le "
			"sujet (comportement indéfini), simple information\n      appel : "
			"%s\n", t->name, strsignal(WTERMSIG(st)), g_sh->step);
	else if (WIFSIGNALED(st))
		snprintf(line, sizeof line, "%s [%s] %s %s\n      dernier appel : %s\n",
			t->soft ? "⚠" : "💥", t->name,
			WTERMSIG(st) == SIGALRM ? "TIMEOUT" : "CRASH",
			sig_hint(WTERMSIG(st)), g_sh->step);
	else
		snprintf(line, sizeof line, "%s [%s] le processus s'est arrêté "
			"(exit(%d) appelé ?)\n      dernier appel : %s\n",
			t->soft ? "⚠" : "💥", t->name,
			WIFEXITED(st) ? WEXITSTATUS(st) : -1, g_sh->step);
	if (t->soft)
	{
		g_res.soft_crashes++;
		g_res.warns++;
	}
	else
		g_res.crashes++;
	res_add(line);
}

static void	print_details(int verbose)
{
	char	*p;
	char	*nl;
	int		shown;
	int		total;
	int		last_shown;
	char	*q;

	last_shown = 0;
	total = 0;
	for (q = g_res.text; *q; q++)
		total += (*q == '\n' && q[1] != ' ');
	shown = 0;
	p = g_res.text;
	while (*p)
	{
		nl = strchr(p, '\n');
		if (!nl)
			nl = p + strlen(p);
		if (p[0] != ' ')
			last_shown = (verbose || shown < 8);
		if (last_shown)
		{
			const char	*col = C_RED;

			if (!strncmp(p, "⚠", strlen("⚠")))
				col = C_YEL;
			else if (p[0] == ' ')
				col = C_DIM;
			printf("    %s%.*s%s\n", col, (int)(nl - p), p, C_RST);
			if (p[0] != ' ')
				shown++;
		}
		p = *nl ? nl + 1 : nl;
	}
	if (!verbose && total > shown)
		printf("    %s… et %d autre(s) message(s) (relance avec -v pour tout "
			"voir)%s\n", C_DIM, total - shown, C_RST);
}

static int	wanted(const char *func, int part, int ac, char **av)
{
	int	i;
	int	any;

	any = 0;
	for (i = 1; i < ac; i++)
	{
		const char	*a = av[i];

		if (a[0] == '-')
			continue ;
		any = 1;
		if (!strcmp(a, func) || (!strncmp(func, "ft_", 3)
				&& !strcmp(a, func + 3)))
			return (1);
		if ((a[0] == '1' || a[0] == '2' || a[0] == '3') && !a[1]
			&& a[0] - '0' == part)
			return (1);
		if (!strncmp(a, "part", 4) && a[4] - '0' == part && !a[5])
			return (1);
	}
	return (!any);
}

static int	has_flag(int ac, char **av, const char *f1, const char *f2)
{
	int	i;

	for (i = 1; i < ac; i++)
		if (!strcmp(av[i], f1) || (f2 && !strcmp(av[i], f2)))
			return (1);
	return (0);
}

int	main(int ac, char **av)
{
	int		verbose;
	int		nosoft;
	int		f;
	int		i;
	int		cur_part;
	int		nfunc;
	int		nok;
	int		nmissing;
	int		tot_checks;
	int		tot_fails;
	char	failed_list[4096];

	if (has_flag(ac, av, "-h", "--help"))
	{
		printf("usage : %s [-v] [--no-soft] [fonction|1|2|3 ...]\n"
			"  -v          affiche tous les messages\n"
			"  --no-soft   ignore les tests non exigés (NULL, hors plage)\n"
			"  exemples    %s ft_split itoa   |   %s 3\n", av[0], av[0], av[0]);
		return (0);
	}
	verbose = has_flag(ac, av, "-v", "--verbose");
	nosoft = has_flag(ac, av, "--no-soft", NULL);
	g_color = isatty(1) && !getenv("NO_COLOR");
	setvbuf(stdout, NULL, _IOLBF, 0);
	signal(SIGPIPE, SIG_IGN);
	g_sh = mmap(NULL, sizeof *g_sh, PROT_READ | PROT_WRITE,
			MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (g_sh == MAP_FAILED)
	{
		perror("mmap");
		return (2);
	}
	cur_part = 0;
	nfunc = 0;
	nok = 0;
	nmissing = 0;
	tot_checks = 0;
	tot_fails = 0;
	failed_list[0] = 0;
	for (f = 0; g_order[f]; f++)
	{
		const t_entry	*first = NULL;

		for (i = 0; i < g_ntests; i++)
			if (!strcmp(g_tests[i].func, g_order[f]))
			{
				first = &g_tests[i];
				break ;
			}
		if (!first || !wanted(first->func, first->part, ac, av))
			continue ;
		if (first->part != cur_part)
		{
			cur_part = first->part;
			printf("\n%s━━ %s ━━%s\n", C_BLD, g_part_title[cur_part], C_RST);
		}
		nfunc++;
		if (!first->addr)
		{
			printf("%-16s %s⊘ absente de libft.a (non trouvée à l'édition de "
				"liens)%s\n", first->func, C_RED, C_RST);
			nmissing++;
			snprintf(failed_list + strlen(failed_list),
				sizeof failed_list - strlen(failed_list), " %s", first->func);
			continue ;
		}
		memset(&g_res, 0, sizeof g_res);
		for (i = 0; i < g_ntests; i++)
			if (!strcmp(g_tests[i].func, g_order[f])
				&& !(nosoft && g_tests[i].soft))
				run_one(&g_tests[i]);
		tot_checks += g_res.checks;
		tot_fails += g_res.fails + g_res.crashes;
		if (g_res.crashes)
			printf("%-16s %s💥 CRASH%s  %s(%d vérification(s) réussie(s) "
				"sur %d effectuée(s))%s\n", g_order[f], C_RED, C_RST, C_DIM,
				g_res.checks - g_res.fails, g_res.checks, C_RST);
		else if (g_res.fails)
			printf("%-16s %s✘ %d/%d%s\n", g_order[f], C_RED,
				g_res.checks - g_res.fails, g_res.checks, C_RST);
		else
		{
			nok++;
			printf("%-16s %s✔ %d/%d%s", g_order[f], C_GRN, g_res.checks,
				g_res.checks, C_RST);
			if (g_res.warns)
				printf("  %s(%d avertissement(s) non bloquant(s))%s",
					C_YEL, g_res.warns, C_RST);
			printf("\n");
		}
		if (g_res.crashes || g_res.fails)
			snprintf(failed_list + strlen(failed_list),
				sizeof failed_list - strlen(failed_list), " %s", g_order[f]);
		if (g_res.len)
			print_details(verbose);
	}
	if (!nfunc)
	{
		printf("Aucune fonction ne correspond. Essaie : %s -h\n", av[0]);
		return (2);
	}
	printf("\n%s━━ Bilan ━━%s\n", C_BLD, C_RST);
	printf("%s%d/%d fonction(s) sans erreur%s   (%d vérifications au total)\n",
		nok == nfunc ? C_GRN : C_RED, nok, nfunc, C_RST, tot_checks);
	if (nmissing)
		printf("%s%d fonction(s) absente(s)%s\n", C_RED, nmissing, C_RST);
	if (failed_list[0])
		printf("À revoir :%s%s%s\n", C_RED, failed_list, C_RST);
	if (nok == nfunc)
		printf("%sTout passe. Pense aussi à `make check` (Makefile, norme, "
			"README…) et à valgrind.%s\n", C_GRN, C_RST);
	(void)tot_fails;
	return (nok == nfunc ? 0 : 1);
}

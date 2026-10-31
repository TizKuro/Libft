/*
** Partie 3 — listes chaînées.
*/
#include "tester.h"

#define N(a) (sizeof(a) / sizeof(*(a)))

static void		*g_del[4096];
static int		g_ndel;
static void		*g_iter[4096];
static int		g_niter;

static void	del_free(void *p)
{
	if (g_ndel < 4096)
		g_del[g_ndel] = p;
	g_ndel++;
	free(p);
}

static char	*dupstr(const char *s)
{
	char	*d = malloc(strlen(s) + 1);

	strcpy(d, s);
	return (d);
}

/* nœud construit par le testeur (indépendant de ft_lstnew) */
static t_list	*mk(void *content)
{
	t_list	*n = malloc(sizeof(t_list));

	n->content = content;
	n->next = NULL;
	return (n);
}

static t_list	*mk_list(const char **words, size_t n, t_list **nodes)
{
	t_list	*head = NULL;
	t_list	*prev = NULL;

	for (size_t i = 0; i < n; i++)
	{
		t_list	*x = mk(dupstr(words[i]));

		if (nodes)
			nodes[i] = x;
		if (prev)
			prev->next = x;
		else
			head = x;
		prev = x;
	}
	return (head);
}

static void	free_list(t_list *l)
{
	while (l)
	{
		t_list	*nx = l->next;

		if (t_is_live(l->content))
			free(l->content);
		free(l);
		l = nx;
	}
}

/* ====================================================================== */
/* lstnew                                                                   */
/* ====================================================================== */
TEST(3, ft_lstnew, creation)
{
	char	data[] = "contenu";
	int		x = 42;

	STEP("ft_lstnew(\"contenu\")");
	t_list *n = ft_lstnew(data);
	if (CHECK(n != NULL, "ft_lstnew(ptr) : NULL inattendu"))
	{
		CHECK(t_is_live(n), "ft_lstnew : le nœud doit être alloué avec malloc");
		CHECK(t_block_size(n) >= sizeof(t_list), "ft_lstnew : %zu octets "
			"alloués, il en faut sizeof(t_list) = %zu (attention : "
			"sizeof(t_list *) n'est pas sizeof(t_list))", t_block_size(n),
			sizeof(t_list));
		CHECK(n->content == data, "ft_lstnew(ptr) : content doit être ptr "
			"lui-même (pas une copie)");
		CHECK(n->next == NULL, "ft_lstnew : next doit valoir NULL");
		free(n);
	}
	STEP("ft_lstnew(&x)");
	n = ft_lstnew(&x);
	if (CHECK(n != NULL && n->content == &x && n->next == NULL,
			"ft_lstnew(&int) : nœud incorrect"))
		free(n);
	STEP("ft_lstnew(NULL)");
	n = ft_lstnew(NULL);
	if (CHECK(n != NULL, "ft_lstnew(NULL) : un nœud doit être créé (contenu "
			"NULL autorisé)"))
	{
		CHECK(n->content == NULL && n->next == NULL, "ft_lstnew(NULL) : nœud "
			"incorrect");
		free(n);
	}
}

TEST(3, ft_lstnew, echec_de_malloc)
{
	char	data[] = "x";

	FAIL_LOOP(t_list *, r, ft_lstnew(data), free(r), "ft_lstnew(%s)", R(data));
}

/* ====================================================================== */
/* lstadd_front                                                             */
/* ====================================================================== */
TEST(3, ft_lstadd_front, ajouts)
{
	t_list	*lst = NULL;
	t_list	*a = mk("a");
	t_list	*b = mk("b");
	t_list	*c = mk("c");

	STEP("ft_lstadd_front(&lst, a) avec lst vide");
	ft_lstadd_front(&lst, a);
	CHECK(lst == a && a->next == NULL, "ajout dans une liste vide : lst doit "
		"pointer sur le nouveau nœud, et son next rester NULL");
	STEP("ft_lstadd_front(&lst, b)");
	ft_lstadd_front(&lst, b);
	CHECK(lst == b && b->next == a && a->next == NULL, "après ajout de b : "
		"liste attendue b -> a");
	STEP("ft_lstadd_front(&lst, c)");
	ft_lstadd_front(&lst, c);
	CHECK(lst == c && c->next == b && b->next == a && a->next == NULL,
		"après ajout de c : liste attendue c -> b -> a");
	free(a);
	free(b);
	free(c);
}

SOFT_TEST(3, ft_lstadd_front, null)
{
	t_list	*lst = NULL;
	t_list	*a = mk("a");

	STEP("ft_lstadd_front(NULL, a)");
	ft_lstadd_front(NULL, a);
	STEP("ft_lstadd_front(&lst, NULL)");
	ft_lstadd_front(&lst, NULL);
	CHECK(lst == NULL, "ft_lstadd_front(&lst, NULL) : la liste ne devrait pas "
		"changer");
	free(a);
}

/* ====================================================================== */
/* lstsize / lstlast                                                        */
/* ====================================================================== */
static t_list	g_nodes[5000];

static t_list	*chain(size_t n)
{
	for (size_t i = 0; i < n; i++)
	{
		g_nodes[i].content = NULL;
		g_nodes[i].next = (i + 1 < n) ? &g_nodes[i + 1] : NULL;
	}
	return (n ? &g_nodes[0] : NULL);
}

TEST(3, ft_lstsize, tailles)
{
	size_t	t[] = {0, 1, 2, 3, 10, 5000};

	for (size_t i = 0; i < N(t); i++)
	{
		t_list	*l = chain(t[i]);

		STEP("ft_lstsize(liste de %zu nœud(s))", t[i]);
		unsigned int r = ft_lstsize(l);
		CHECK(r == t[i], "ft_lstsize(liste de %zu nœud(s)) : obtenu %u%s", t[i],
			r, t[i] == 0 ? " (lst == NULL : la liste vide a 0 nœud)" : "");
	}
	chain(3);
	STEP("ft_lstsize(&nœud[1]) (milieu de liste)");
	CHECK(ft_lstsize(&g_nodes[1]) == 2, "ft_lstsize depuis le 2e nœud d'une "
		"liste de 3 : 2 attendu");
}

TEST(3, ft_lstlast, dernier)
{
	size_t	t[] = {1, 2, 3, 10, 5000};

	STEP("ft_lstlast(NULL)");
	CHECK(ft_lstlast(NULL) == NULL, "ft_lstlast(NULL) : NULL attendu");
	for (size_t i = 0; i < N(t); i++)
	{
		t_list	*l = chain(t[i]);

		STEP("ft_lstlast(liste de %zu nœud(s))", t[i]);
		t_list *r = ft_lstlast(l);
		CHECK(r == &g_nodes[t[i] - 1], "ft_lstlast(liste de %zu nœud(s)) : "
			"mauvais nœud renvoyé (%s)", t[i], r ? "pas le dernier" : "NULL");
	}
}

/* ====================================================================== */
/* lstadd_back                                                              */
/* ====================================================================== */
TEST(3, ft_lstadd_back, ajouts)
{
	t_list	*lst = NULL;
	t_list	*n[5];

	for (int i = 0; i < 5; i++)
		n[i] = mk(NULL);
	STEP("ft_lstadd_back(&lst, n0) avec lst vide");
	ft_lstadd_back(&lst, n[0]);
	CHECK(lst == n[0], "ajout dans une liste vide : lst doit devenir le "
		"nouveau nœud (*lst == NULL est un cas normal, pas une erreur)");
	for (int i = 1; i < 5; i++)
	{
		STEP("ft_lstadd_back(&lst, n%d)", i);
		ft_lstadd_back(&lst, n[i]);
	}
	t_list *p = lst;
	int i = 0;
	while (p && i < 6)
	{
		if (!CHECK(p == n[i], "après 5 ajouts : le nœud %d n'est pas à sa place",
				i))
			break ;
		p = p->next;
		i++;
	}
	CHECK(i == 5 && p == NULL, "après 5 ajouts : liste de %d nœud(s) au lieu "
		"de 5", i);
	CHECK(lst == n[0], "ft_lstadd_back ne doit pas changer la tête d'une "
		"liste non vide");
	for (i = 0; i < 5; i++)
		free(n[i]);
}

TEST(3, ft_lstadd_back, longue_liste)
{
	t_list	*lst = NULL;
	t_list	*l = chain(4999);

	(void)lst;
	g_nodes[4999].content = NULL;
	g_nodes[4999].next = NULL;
	STEP("ft_lstadd_back(&lst, x) sur une liste de 4999 nœuds");
	ft_lstadd_back(&l, &g_nodes[4999]);
	CHECK(g_nodes[4998].next == &g_nodes[4999] && l == &g_nodes[0],
		"ajout à la fin d'une liste de 4999 nœuds : incorrect");
}

SOFT_TEST(3, ft_lstadd_back, null)
{
	t_list	*lst = NULL;
	t_list	*a = mk("a");

	STEP("ft_lstadd_back(NULL, a)");
	ft_lstadd_back(NULL, a);
	STEP("ft_lstadd_back(&lst, NULL)");
	ft_lstadd_back(&lst, NULL);
	CHECK(lst == NULL, "ft_lstadd_back(&lst, NULL) : la liste ne devrait pas "
		"changer");
	free(a);
}

/* ====================================================================== */
/* lstdelone / lstclear                                                     */
/* ====================================================================== */
TEST(3, ft_lstdelone, un_seul_noeud)
{
	const char	*w[] = {"premier", "second"};
	t_list		*nodes[2];
	t_list		*l = mk_list(w, 2, nodes);
	void		*c0 = nodes[0]->content;
	void		*c1 = nodes[1]->content;

	g_ndel = 0;
	STEP("ft_lstdelone(premier_noeud, del)");
	ft_lstdelone(l, del_free);
	CHECK(g_ndel == 1, "ft_lstdelone : del doit être appelée exactement 1 fois "
		"(appelée %d fois)", g_ndel);
	CHECK(g_ndel < 1 || g_del[0] == c0, "ft_lstdelone : del doit recevoir le "
		"CONTENT du nœud (pas le nœud lui-même)");
	CHECK(!t_is_live(nodes[0]), "ft_lstdelone : le nœud lui-même doit être "
		"libéré avec free");
	CHECK(t_is_live(nodes[1]) && t_is_live(c1)
		&& !strcmp((char *)nodes[1]->content, "second"),
		"ft_lstdelone : le nœud suivant ne doit PAS être touché");
	free_list(nodes[1]);
}

SOFT_TEST(3, ft_lstdelone, null)
{
	STEP("ft_lstdelone(NULL, del)");
	ft_lstdelone(NULL, del_free);
	CHECK(1, "ok");
}

TEST(3, ft_lstclear, liste_complete)
{
	const char	*w[] = {"a", "bb", "ccc", "dddd", "eeeee"};
	t_list		*nodes[5];
	void		*cont[5];
	t_list		*l = mk_list(w, 5, nodes);

	for (int i = 0; i < 5; i++)
		cont[i] = nodes[i]->content;
	g_ndel = 0;
	STEP("ft_lstclear(&lst, del) sur une liste de 5 nœuds");
	ft_lstclear(&l, del_free);
	CHECK(l == NULL, "ft_lstclear : *lst doit valoir NULL à la fin");
	CHECK(g_ndel == 5, "ft_lstclear : del appelée %d fois au lieu de 5", g_ndel);
	for (int i = 0; i < 5; i++)
	{
		int	found = 0;

		for (int k = 0; k < g_ndel && k < 4096; k++)
			found += (g_del[k] == cont[i]);
		CHECK(found == 1, "ft_lstclear : le contenu du nœud %d a été passé %d "
			"fois à del (1 attendu)", i, found);
		CHECK(!t_is_live(nodes[i]), "ft_lstclear : le nœud %d n'a pas été "
			"libéré", i);
	}
}

TEST(3, ft_lstclear, liste_vide_et_longue)
{
	t_list	*l = NULL;
	t_list	*head = NULL;

	g_ndel = 0;
	STEP("ft_lstclear(&lst, del) avec lst == NULL");
	ft_lstclear(&l, del_free);
	CHECK(l == NULL && g_ndel == 0, "ft_lstclear sur une liste vide : rien à "
		"faire, del ne doit pas être appelée");
	for (int i = 0; i < 3000; i++)
	{
		t_list	*x = mk(malloc(4));

		x->next = head;
		head = x;
	}
	g_ndel = 0;
	STEP("ft_lstclear(&lst, del) sur 3000 nœuds");
	ft_lstclear(&head, del_free);
	CHECK(head == NULL && g_ndel == 3000, "ft_lstclear(3000 nœuds) : del "
		"appelée %d fois, *lst %s", g_ndel, head ? "non NULL" : "NULL");
}

SOFT_TEST(3, ft_lstclear, null)
{
	STEP("ft_lstclear(NULL, del)");
	ft_lstclear(NULL, del_free);
	CHECK(1, "ok");
}

/* ====================================================================== */
/* lstiter                                                                  */
/* ====================================================================== */
static void	f_incr(void *p)
{
	if (g_niter < 4096)
		g_iter[g_niter] = p;
	g_niter++;
	(*(int *)p)++;
}

TEST(3, ft_lstiter, application)
{
	int		v[5] = {0, 10, 20, 30, 40};
	t_list	n[5];

	for (int i = 0; i < 5; i++)
	{
		n[i].content = &v[i];
		n[i].next = i < 4 ? &n[i + 1] : NULL;
	}
	g_niter = 0;
	STEP("ft_lstiter(liste de 5, f)");
	ft_lstiter(&n[0], f_incr);
	CHECK(g_niter == 5, "ft_lstiter : f appelée %d fois au lieu de 5", g_niter);
	for (int i = 0; i < 5 && i < g_niter; i++)
		CHECK(g_iter[i] == &v[i], "ft_lstiter : appel n°%d de f : doit recevoir "
			"le content du nœud %d, dans l'ordre", i, i);
	CHECK(v[0] == 1 && v[4] == 41, "ft_lstiter : les contenus n'ont pas été "
		"modifiés par f");
	g_niter = 0;
	STEP("ft_lstiter(NULL, f)");
	ft_lstiter(NULL, f_incr);
	CHECK(g_niter == 0, "ft_lstiter(NULL, f) : f ne doit pas être appelée");
}

/* ====================================================================== */
/* lstmap                                                                   */
/* ====================================================================== */
static int	g_nf;

static void	*f_upper_dup(void *p)
{
	char	*s = p;
	char	*d = malloc(strlen(s) + 1);
	size_t	i;

	g_nf++;
	for (i = 0; s[i]; i++)
		d[i] = (s[i] >= 'a' && s[i] <= 'z') ? s[i] - 32 : s[i];
	d[i] = 0;
	return (d);
}

/* même chose, mais son malloc ne peut pas échouer (tests d'échec) */
static void	*f_upper_dup_nofail(void *p)
{
	char	*s = p;
	char	*d = t_malloc_nofail(strlen(s) + 1);
	size_t	i;

	g_nf++;
	for (i = 0; s[i]; i++)
		d[i] = (s[i] >= 'a' && s[i] <= 'z') ? s[i] - 32 : s[i];
	d[i] = 0;
	return (d);
}

TEST(3, ft_lstmap, nouvelle_liste)
{
	const char	*w[] = {"un", "deux", "trois", "quatre"};
	t_list		*nodes[4];
	t_list		*l = mk_list(w, 4, nodes);
	const char	*exp[] = {"UN", "DEUX", "TROIS", "QUATRE"};
	t_list		*m;
	t_list		*p;
	int			i;

	g_ndel = 0;
	g_nf = 0;
	STEP("ft_lstmap(liste de 4, f, del)");
	m = ft_lstmap(l, f_upper_dup, del_free);
	if (!CHECK(m != NULL, "ft_lstmap : NULL inattendu"))
	{
		free_list(l);
		return ;
	}
	CHECK(g_nf == 4, "ft_lstmap : f appelée %d fois au lieu de 4", g_nf);
	CHECK(g_ndel == 0, "ft_lstmap : del ne doit pas être appelée quand tout "
		"se passe bien (appelée %d fois)", g_ndel);
	p = m;
	i = 0;
	while (p && i < 4)
	{
		int	k;

		for (k = 0; k < 4; k++)
			if (p == nodes[k])
				break ;
		if (!CHECK(k == 4, "ft_lstmap : le nœud %d de la nouvelle liste est un "
				"nœud de l'ancienne (il faut créer de nouveaux nœuds)", i))
			break ;
		CHECK(t_is_live(p), "ft_lstmap : nœud %d non alloué avec malloc", i);
		CHECK(!strcmp(p->content, exp[i]), "ft_lstmap : nœud %d : contenu %s au "
			"lieu de %s", i, R(p->content), R(exp[i]));
		p = p->next;
		i++;
	}
	CHECK(i == 4 && p == NULL, "ft_lstmap : la nouvelle liste n'a pas 4 nœuds");
	for (i = 0; i < 4; i++)
		CHECK(!strcmp(nodes[i]->content, w[i]), "ft_lstmap : l'ancienne liste "
			"a été modifiée (nœud %d)", i);
	free_list(l);
	free_list(m);
}

TEST(3, ft_lstmap, liste_vide)
{
	g_nf = 0;
	STEP("ft_lstmap(NULL, f, del)");
	t_list *m = ft_lstmap(NULL, f_upper_dup, del_free);
	CHECK(m == NULL && g_nf == 0, "ft_lstmap(NULL, f, del) : NULL attendu, "
		"sans appeler f");
	free_list(m);
}

static void	free_mapped(t_list *m)
{
	free_list(m);
}

TEST(3, ft_lstmap, echec_de_malloc)
{
	const char	*w[] = {"a", "b", "c"};
	t_list		*l = mk_list(w, 3, NULL);
	unsigned long	before;

	g_ndel = 0;
	before = t_serial();
	(void)before;
	FAIL_LOOP(t_list *, r, ft_lstmap(l, f_upper_dup_nofail, del_free),
		free_mapped(r), "ft_lstmap(%s, f, del) (f alloue un nouveau contenu ; "
		"si ft_lstnew échoue, ce contenu doit être passé à del)",
		"[\"a\",\"b\",\"c\"]");
	for (t_list *p = l; p; p = p->next)
		CHECK(t_is_live(p) && t_is_live(p->content), "ft_lstmap : en cas "
			"d'échec, l'ANCIENNE liste ne doit pas être libérée");
	free_list(l);
}

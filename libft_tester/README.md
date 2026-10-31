# libft_tester — sujet Libft v19.3

Testeur poussé pour les 43 fonctions de la Libft (parties 1, 2 et 3), plus une
vérification du projet (Makefile, libft.h, symboles, README, norme).

Il ne contient **aucune implémentation** des fonctions : il te dit *ce qui ne
va pas* et *dans quel cas*, à toi de trouver *pourquoi*.

## Installation

Place le dossier **à côté** de ta libft (pas dedans : le sujet interdit les
fichiers inutiles dans ton dépôt) :

```
mon_dossier/
├── libft/          <- ton projet
└── libft_tester/
```

```sh
cd libft_tester
make                      # vérifs du projet + tous les tests
make test                 # seulement les tests de fonctions
make test F=ft_split      # une fonction (le préfixe ft_ est optionnel)
make test F="itoa strjoin"
make test F=3             # une partie (1, 2 ou 3)
make verbose F=memmove    # affiche tous les messages
make check                # Makefile, libft.h, nm, README, norminette
make valgrind             # tout sous valgrind (lent)
make LIBFT=../autre/chemin
```

Testé sous Linux (glibc). Sous macOS, l'option d'édition de liens `--wrap`
n'existe pas : utilise une machine de l'école.

## Ce qui est vérifié

**Pour chaque fonction**
- comparaison avec la vraie libc quand elle existe (valeurs de -1 à 255,
  octets > 127, chaînes vides, très longues…), sinon résultats attendus écrits
  à la main d'après le man BSD (`strlcpy`, `strlcat`, `strnstr`) ;
- **lectures/écritures hors limites** : les buffers de test sont collés à une
  page mémoire inaccessible, donc lire un octet après le `'\0'` (ou après `n`)
  fait crasher immédiatement, avec l'appel en cause affiché ;
- **entrées en lecture seule** : modifier un paramètre `const` fait crasher ;
- **malloc** : chaque bloc est suivi. Sont détectés les fuites, les double
  free, les `free` d'un pointeur décalé, les écritures après la fin d'un bloc
  (oubli du `+ 1` pour le `'\0'`), et la mémoire neuve est remplie de déchets
  (un `calloc` qui ne met pas à zéro se voit) ;
- **échecs de malloc simulés** : le 1er malloc échoue, puis le 2e, etc. La
  fonction doit renvoyer `NULL` *sans rien laisser fuir* (crucial pour
  `ft_split` et `ft_lstmap`) ;
- boucles infinies (délai de 5 s par groupe de tests) et crashs, isolés :
  un crash n'arrête pas le reste.

**Avertissements ⚠ (non bloquants)** : comportements *non exigés* par le
sujet mais souvent testés en correction (paramètres `NULL`, valeurs hors de
[-1, 255] pour `is*`, `ft_putnbr_fd` sans malloc…). `--no-soft` les masque.

**`make check`** : fichiers en trop, règles du Makefile, flags, `cc`, `ar`,
relink, `clean`/`fclean`/`re`, include guard, prototypes **exacts** du sujet
(attention : `unsigned int ft_lstsize` dans la v19.3), structure `t_list`,
fonctions externes interdites, variables globales, fonctions d'aide non
`static`, 1re ligne et sections du README, norminette si installée.

## Lire un résultat

```
ft_strlcat       💥 CRASH
    💥 [size_trop_petit] CRASH SIGSEGV ...
          dernier appel : ft_strlcat(dst="abc", "defgh", 3) — dst fait exactement
          3 octet(s) et n'est PAS terminé dans ces octets : interdit de lire plus
          loin que size
```

- `[size_trop_petit]` : le nom du groupe de tests, à retrouver dans
  `test_part1.c` (cherche `size_trop_petit`) ;
- `(tests l.123)` : la ligne du testeur où est faite la vérification ;
- reproduis le cas dans un petit `main` à toi pour comprendre.

## Organisation

| fichier | rôle |
|---|---|
| `framework.c` | moteur : fork par test, interception de malloc, affichage |
| `test_part1.c` / `test_part2.c` / `test_part3.c` | les tests |
| `protos.h` | prototypes du sujet (le testeur n'inclut pas ton `libft.h`) |
| `check_project.sh` | vérifications du projet |

Ajouter un test : copie un bloc `TEST(partie, ft_fonction, nom) { ... }`
existant et utilise `CHECK(condition, "message", ...)`.

## Rappel

Le sujet encourage à écrire ses propres tests et autorise à les utiliser en
correction. Un testeur qui passe au vert ne remplace pas le fait de savoir
expliquer chaque ligne de ton code : c'est ça qui sera évalué.

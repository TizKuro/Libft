
#include "libft.h"
static char	*ft_strcat(char *new, const char *s1, const char *s2)
{
	size_t	d;
	size_t	i;

	i = 0;
	d = 0;
	while (s1[i])
	{
		new[d] = s1[i];
		i++;
		d++;
	}
	i = 0;
	while (s2[i])
	{
		new[d] = s2[i];
		i++;
		d++;
	}
	new[d] = '\0';
	return (new);
}

char *ft_strjoin(char const *s1, char const *s2)
{
	char	*rawr;
	size_t	lens1;
	size_t	lens2;

	if (!s2 || !s1)
		return (0);
	lens1 = ft_strlen(s1);
	lens2 = ft_strlen(s2);
	rawr = malloc(sizeof(char) * (lens1 + lens2 + 1));
	if (!rawr)
		return (0);
	return (ft_strcat(rawr,s1, s2));
}

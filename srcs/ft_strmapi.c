#include "libft.h"
char	*ft_strmapi (char const *s, char (*f)(unsigned int, char))
{
	int	i;
	char *new;
	size_t	len;

	if (!s ||  !f)
		return (0);
	i = 0;
	len = ft_strlen(s);
	new = malloc(sizeof(char) * (len + 1));
	if (!new)
		return (0);
	while (s[i])
	{
		 new[i] = f(i, s[i]);
		i++;
	}
	new[i] = '\0';
	return (new);
}


#include <stdlib.h>
#include <unistd.h>
static int	ft_len(long int n)
{
	int len;

	len = 0;
	if (n <= 0)
	{
		n *= -1;
		len++;
	}
	while (n > 0)
	{
		n /= 10;
		len++;
	}
	return (len);
}


char	*ft_itoa(int nbr)
{
	long int n;
	 int len;
	char *rawr;
	int	i;
	int	j;

	j = 0;
	n = nbr;
	len = ft_len(n);
	rawr = malloc(sizeof(char) * (len + 1));
	if (!rawr)
	{
		return (NULL);
	}
	if (n < 0)
	{
		rawr[j] = '-';
		n *= -1;
		j++;
	}
	i = len - 1;
	while (i >= j)
    {
		rawr[i] = n % 10 + '0';
		n /= 10;
		i--;
	}
	rawr[len] = '\0';
	return (rawr);
}

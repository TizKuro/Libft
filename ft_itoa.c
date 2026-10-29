/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trmorand <trmorand@student.42angouleme.fr  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 00:21:03 by trmorand          #+#    #+#             */
/*   Updated: 2026/10/29 18:50:16 by trmorand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_len(long int n)
{
	int	len;

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
	long int	n;
	char		*rawr;
	int			i;
	int			j;

	j = 0;
	n = nbr;
	i = ft_len(n) - 1;
	rawr = malloc(sizeof(char) * (ft_len(n) + 1));
	if (!rawr)
		return (NULL);
	if (n < 0)
	{
		rawr[j] = '-';
		n *= -1;
		j++;
	}
	while (i >= j)
	{
		rawr[i] = n % 10 + '0';
		n /= 10;
		i--;
	}
	rawr[ft_len(nbr)] = '\0';
	return (rawr);
}

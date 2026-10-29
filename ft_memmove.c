/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trmorand <trmorand@student.42angouleme.fr  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/20 14:50:05 by trmorand          #+#    #+#             */
/*   Updated: 2026/10/22 17:21:12 by trmorand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memmove(void *dest, const void *src, size_t n)
{
	size_t			i;
	unsigned char	*d;
	unsigned char	*s;

	i = n;
	if (n > 0)
		i = n - 1;
	d = (unsigned char *)dest;
	s = (unsigned char *)src;
	if (d < s)
		ft_memcpy(dest, src, n);
	else
	{
		while (n != 0)
		{
			d[i] = s[i];
			i--;
			n--;
		}
	}
	return (dest);
}

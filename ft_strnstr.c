/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trmorand <trmorand@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/22 13:24:10 by trmorand          #+#    #+#             */
/*   Updated: 2026/10/31 21:03:33 by trmorand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strnstr(const char *s1, const char *s2, size_t n)
{
	size_t				i;
	size_t				p;
	size_t				j;

	i = 0;
	p = 0;
	if (ft_strlen((char *)s2) == 0)
		return ((char *)s1);
	while (i < n && s1[i])
	{
		if (s1[i] == s2[p])
		{
			j = i;
			while (j < n && s1[j] == s2[p] && s2[p] != '\0')
			{
				j++;
				p++;
			}
			if (s2[p] == '\0')
				return ((char *)&s1[j - p]);
			p = 0;
		}
		i++;
	}
	return (0);
}

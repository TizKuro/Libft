/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trmorand <trmorand@student.42angouleme.fr  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/22 13:24:10 by trmorand          #+#    #+#             */
/*   Updated: 2026/10/22 17:24:09 by trmorand         ###   ########.fr       */
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
	while (s1[i] && i < n)
	{
		if (s1[i] == s2[p])
		{
			j = i;
			while (s1[j] == s2[p] && s2[p] != '\0' && j < n)
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

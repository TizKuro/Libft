/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trmorand <trmorand@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 00:16:05 by trmorand          #+#    #+#             */
/*   Updated: 2026/10/31 20:18:06 by trmorand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static	size_t	check(const char *s1, char c)
{
	while (*s1 && c != *s1)
		s1++;
	return (c == *s1);
}

char	*ft_strtrim(char const *s1, char const *set)
{
	size_t	p;
	size_t	l;
	char	*sub;
	char	*ret;

	if (!s1 || !set)
		return (0);
	p = 0;
	l = ft_strlen(s1);
	while (s1[p] && check(set, s1[p]))
		p++;
	if (p == l)
	{
		ret = ft_strdup("");
		return (ret);
	}
	while (check(set, s1[l - 1]))
		l--;
	sub = ft_substr(s1, p, l - p);
	return (sub);
}

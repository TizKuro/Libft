/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trmorand <trmorand@student.42angouleme.fr  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 00:11:47 by trmorand          #+#    #+#             */
/*   Updated: 2026/10/27 04:36:50 by trmorand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	start(char const *s, int i, char c)
{
	while (s[i] && s[i] == c)
		i++;
	return (i);
}

static int	end(char const *s, int i, char c)
{
	int	len;

	len = 0;
	while (s[i + len] && s[i + len] != c)
		len++;
	return (len);
}

static int	word_count(char const *s, char c)
{
	int	i;
	int	w;

	i = 0;
	w = 0;
	while (s[i])
	{
		if (s[i] != c && (i == 0 || s[i - 1] == c))
			w++;
		i++;
	}
	return (w);
}

static char	**ft_free(char **res, int i)
{
	int	w;

	w = 0;
	while (w < i)
	{
		free(res[w]);
		w++;
	}
	free(res);
	return (0);
}

char	**ft_split(char const *s, char c)
{
	int		i;
	int		w;
	char	**res;
	int		pos;
	int		begin;

	pos = 0;
	i = 0;
	if (!s)
		return (0);
	w = word_count(s, c);
	res = malloc(sizeof(char *) * (w + 1));
	if (!res)
		return (0);
	while (i < w)
	{
		begin = start(s, pos, c);
		res[i] = ft_substr(s, begin, end(s, begin, c));
		if (!res[i])
			return (ft_free(res, i));
		pos = begin + end(s, begin, c);
		i++;
	}
	res[i] = 0;
	return (res);
}

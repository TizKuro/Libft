/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trmorand <trmorand@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 00:11:47 by trmorand          #+#    #+#             */
/*   Updated: 2026/10/29 16:45:47 by trmorand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

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

static char	**fill_words(char **res, char const *s, char c, int w)
{
	int	i;
	int	pos;
	int	begin;
	int	len;

	i = 0;
	pos = 0;
	while (i < w)
	{
		while (s[pos] == c)
			pos++;
		begin = pos;
		len = 0;
		while (s[pos] && s[pos] != c)
		{
			len++;
			pos++;
		}
		res[i] = ft_substr(s, begin, len);
		if (!res[i])
			return (ft_free(res, i));
		i++;
	}
	res[i] = 0;
	return (res);
}

char	**ft_split(char const *s, char c)
{
	int		w;
	char	**res;

	if (!s)
		return (0);
	w = word_count(s, c);
	res = malloc(sizeof(char *) * (w + 1));
	if (!res)
		return (0);
	return (fill_words(res, s, c, w));
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trmorand <trmorand@student.42angouleme.fr  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/23 14:08:21 by trmorand          #+#    #+#             */
/*   Updated: 2026/10/23 14:36:58 by trmorand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char *new;
	size_t p;

	p = (unsigned int)start;
	p = ft_strlen((char *)s) - p;
	new = malloc(sizeof(char) * (p + 1));
	if (!new)
		return (0);
	while (start < len)
	{
		new[p] = s[start];
		start++;
		p++;
	}
	return (new);
}


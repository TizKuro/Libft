/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trmorand <trmorand@student.42angouleme.fr  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/23 14:08:21 by trmorand          #+#    #+#             */
/*   Updated: 2026/10/23 17:17:43 by trmorand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/libft.h"

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char *new;
	size_t	i;

	i = 0;
	if (!s || len < 0)
		return (0);
	if (start > ft_strlen((char *)s))
		len = 0;
	if (len > ft_strlen((char *)s) - start)
		len = ft_strlen((char*)s) - start;
	new = malloc(sizeof(char) * (len + 1));
	if (!new)
		return (0);
	while (i < len || s[start])
	{
		new[i] = s[start];
		start++;
		i++;
	}
	new[len] = '\0';
	return (new);
}

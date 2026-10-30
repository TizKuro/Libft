/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trmorand <trmorand@student.42angouleme.fr  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/21 08:30:33 by trmorand          #+#    #+#             */
/*   Updated: 2026/10/30 17:07:57 by trmorand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dst, const char *src, size_t dstsize)
{
	size_t	i;
	size_t	d;
	size_t	a;
	char	*sr;

	sr = (char *)src;
	a = ft_strlen(sr);
	d = 0;
	i = 0;
	while (d < dstsize && dst[d])
		d++;
	if (d == dstsize)
		return (dstsize + a);
	while (src[i] != '\0' && i + d + 1 < dstsize)
	{
		dst[d + i] = sr[i];
		i++;
	}
	dst[d + i] = '\0';
	return (d + a);
}

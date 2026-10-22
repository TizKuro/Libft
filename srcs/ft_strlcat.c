/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trmorand <trmorand@student.42angouleme.fr  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/21 08:30:33 by trmorand          #+#    #+#             */
/*   Updated: 2026/10/22 17:25:40 by trmorand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *restrict dst, const char *restrict src, size_t dstsize)
{
	size_t	i;
	size_t	d;
	char	*sr;

	sr = (char *)src;
	d = 0;
	i = 0;
	while (d < dstsize && dst[d])
		d++;
	if (d == dstsize)
		return (dstsize + ft_strlen(sr));
	while (src[i] != '\0' && i + d + 1 < dstsize)
	{
		dst[d + i] = sr[i];
		i++;
	}
	dst[d + i] = '\0';
	return (d + ft_strlen(sr));
}

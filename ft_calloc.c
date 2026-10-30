/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trmorand <trmorand@student.42angouleme.fr  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/22 14:00:48 by trmorand          #+#    #+#             */
/*   Updated: 2026/10/30 17:07:21 by trmorand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_calloc(size_t nelem, size_t elsize)
{
	void	*new;

	if (elsize != 0 && nelem > (size_t)-1 / elsize)
		return (NULL);
	new = malloc(nelem * elsize);
	if (!new)
		return (0);
	ft_bzero(new, (nelem * elsize));
	return (new);
}

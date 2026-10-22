/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trmorand <trmorand@student.42angouleme.fr  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/22 14:00:48 by trmorand          #+#    #+#             */
/*   Updated: 2026/10/22 17:54:54 by trmorand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_calloc(size_t nelem, size_t elsize)
{
	void	*new;

	if (nelem == 0 || elsize == 0)
		return (0);
	if ((nelem * elsize) / elsize != nelem)
		return (0);
	new = malloc(nelem * elsize);
	if (!new)
		return (0);
	ft_bzero(new, (nelem * elsize));
	return (new);
}

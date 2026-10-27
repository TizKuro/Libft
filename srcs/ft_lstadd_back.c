/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstadd_back.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trmorand <trmorand@student.42angouleme.fr  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 04:46:11 by trmorand          #+#    #+#             */
/*   Updated: 2026/10/27 05:24:08 by trmorand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstadd_back(t_list **lst, t_list *new)
{
	t_list	t;
	if (!lst || !new)
		return ;
	if (!*lst)
	{
		t = lst;
		return ;
	}
	while (t->next)
		t = t->next;
	t = t->next = new;
}

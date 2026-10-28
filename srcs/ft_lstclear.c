void	ft_lstclear(t_list **lst, void(*del)(void *))
{
	if (!*lst)
		return ;
	del(*lst->content);
	while(*lst->next)
	{
		free(*lst);
		*lst = *lst->next;
	}
}

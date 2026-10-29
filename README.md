*This project has been created as part of the 42 curriculum by trmorand*
# Libft

## Description
Libft is the first project of the 42 Common Core.

The goal is to create a custom C library ("libft.a") by reimplementing a selection of standard C library functions, along with additional memory management and linked-list utilities.

The entire project is written in C and follows the 42 norms.
## Libc fonction
- `ft_isalpha` Checks whether a character is a letter (a - z | A - Z).
- `ft_isdigit` Checks whether a character is a digit (0 - 9).
- `ft_isalnum` Checks whether a character is a letter or a digit.
- `ft_isascii` Checks whether a character belongs to the ASCII range (0-127).
- `ft_isprint` Checks whether a character is printable, including the space character.
- `ft_strlen` Returns the length of a string, excluding the terminating '\0'.
- `ft_memset` Fills a memory area with a specified byte.
- `ft_bzero` Fills a memory area with zeros.
- `ft_memcpy` Copies a specified number of bytes between memory areas that do not overlap.
- `ft_memmove` Copies a specified number of bytes between memory areas, even if they overlap.
- `ft_strlcpy` Copies a string into a buffer with a size limit and null-terminates it if the buffer size is greater than zero.
- `ft_strlcat` Appends a string to another string while respecting the destination buffer's size.
- `ft_toupper` Converts a lowercase letter to uppercase.
- `ft_tolower` Converts an uppercase letter to lowercase.
- `ft_strchr` Returns a pointer to the first occurrence of a character in a string.
- `ft_strrchr` Returns a pointer to the last occurrence of a character in a string.
- `ft_strncmp` Compares the first n characters of two strings.
- `ft_memchr` Searches for the first occurrence of a byte in a memory area.
- `ft_memcmp` Compares the first n bytes of two memory areas.
- `ft_strnstr` Searches for a substring within a string, limiting the search to len characters.
- `ft_atoi` Converts a string representing an integer into an int.
- `ft_calloc` Allocates memory for multiple elements and initializes all allocated bytes to zero.
- `ft_strdup` Allocates new memory and copies a string into it.
## Additional functions
- `ft_substr` Creates a new substring starting at a given index with a maximum specified length.
- `ft_strjoin` Creates a new string by concatenating two strings.
- `ft_strtrim` Removes characters belonging to a specified set from the beginning and end of a string.
- `ft_split` Splits a string into multiple substrings using a specified delimiter character.
- `ft_itoa` Converts an integer into a string, handling negative numbers as well.
- `ft_strmapi` Applies a function to each character using its index and character value, storing the results in a new string.
- `ft_striteri` Applies a function to each character using its index and its address, allowing the character to be modified.
- `ft_putchar_fd` Writes a character to a specified file descriptor.
- `ft_putstr_fd` Writes a string to a specified file descriptor.
- `ft_putlendl_fd` Writes a string followed by a newline to a specified file descriptor.
- `ft_putnbr_fd` Writes an integer to a specified file descriptor.
### Linked lists.
- `ft_lstnew` Creates a new node containing the given data and initializes its next pointer to NULL.
- `ft_lstadd_front`Adds a new node to the beginning of a linked list.
- `ft_lstsize` Returns the number of nodes in a linked list.
- `ft_lstlast` Returns a pointer to the last node of a linked list.
- `ft_lstadd_back` Adds a new node to the end of a linked list.
- `ft_lstdelone` Deletes a node and frees its content using del, without deleting the next node.
- `ft_lstclear` Deletes and frees every node in a list, then sets the list pointer to NULL.
- `ft_lstiter` Applies a function to the content of every node without creating a new list.
- `ft_lstmap` Applies a function to each node's content and creates a new list containing the results.

### Instructions
```Bash
make        # Build libft.a.

make clean  # remove object files.

make fclean # remove object files and libft.a
```
### Resources
- `man 3` Pages for the original libc fonction.
- [Libft_tester](https://github.com/nda-cunh/libft_tester/), a tester for the project created by [nda-cunh](https://github.com/nda-cunh)
- `AI` was used for some exemples.
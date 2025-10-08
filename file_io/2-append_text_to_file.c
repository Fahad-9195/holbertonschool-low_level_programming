#include "main.h"

/**
 * append_text_to_file - appends text at the end of a file
 * @filename: file name
 * @text_content: NULL-terminated string to append (or NULL to append nothing)
 * Return: 1 on success, -1 on failure
 */
int append_text_to_file(const char *filename, char *text_content)
{
	int fd;
	ssize_t w;
	size_t len = 0;

	if (!filename)
		return (-1);

	/* Do not create the file if it doesn't exist */
	fd = open(filename, O_WRONLY | O_APPEND);
	if (fd == -1)
		return (-1);

	if (text_content)
	{
		while (text_content[len] != '\0')
			len++;

		w = write(fd, text_content, len);
		if (w == -1 || (size_t)w != len)
		{
			close(fd);
			return (-1);
		}
	}

	if (close(fd) == -1)
		return (-1);

	return (1);
}

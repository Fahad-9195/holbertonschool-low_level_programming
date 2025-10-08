#include "main.h"

/**
 * read_textfile - reads a text file and prints it to STDOUT
 * @filename: file path
 * @letters: max bytes to read and print
 * Return: number of bytes printed, or 0 on error
 */
ssize_t read_textfile(const char *filename, size_t letters)
{
	int fd;
	ssize_t r, w;
	char *buf;

	if (!filename || !letters)
		return (0);

	fd = open(filename, O_RDONLY);
	if (fd == -1)
		return (0);

	buf = malloc(letters);
	if (!buf)
	{ close(fd); return (0); }

	r = read(fd, buf, letters);
	if (r <= 0)
	{ free(buf); close(fd); return (0); }

	w = write(STDOUT_FILENO, buf, r);
	if (w != r)
	{ free(buf); close(fd); return (0); }

	free(buf);
	close(fd);
	return (w);
}

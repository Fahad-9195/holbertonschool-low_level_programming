#include <stdio.h>      /* dprintf */
#include <fcntl.h>      /* open */
#include <unistd.h>     /* read, write, close */
#include <stdlib.h>     /* malloc, free, exit */
#include <sys/types.h>
#include <sys/stat.h>

#define BUFSIZE 1024

static char *create_buffer(const char *file_to)
{
	char *buf = malloc(BUFSIZE);

	if (buf == NULL)
	{
		dprintf(STDERR_FILENO, "Error: Can't write to %s\n", file_to);
		exit(99);
	}
	return (buf);
}

static void close_fd(int fd)
{
	if (close(fd) == -1)
	{
		dprintf(STDERR_FILENO, "Error: Can't close fd %d\n", fd);
		exit(100);
	}
}

static void write_chunk(int fd_to, char *buf, ssize_t r, const char *file_to)
{
	ssize_t total = 0;
	ssize_t w;

	while (total < r)
	{
		w = write(fd_to, buf + total, r - total);
		if (w == -1)
		{
			dprintf(STDERR_FILENO, "Error: Can't write to %s\n",
				file_to);
			exit(99);
		}
		total += w;
	}
}

static void copy_rest(int fd_from, const char *file_to, char *buf,
		      ssize_t r_first, const char *file_from)
{
	int fd_to;
	ssize_t r;

	fd_to = open(file_to, O_WRONLY | O_CREAT | O_TRUNC, 0664);
	if (fd_to == -1)
	{
		dprintf(STDERR_FILENO, "Error: Can't write to %s\n", file_to);
		exit(99);
	}

	if (r_first > 0)
		write_chunk(fd_to, buf, r_first, file_to);

	r = read(fd_from, buf, BUFSIZE);
	while (r > 0)
	{
		write_chunk(fd_to, buf, r, file_to);
		r = read(fd_from, buf, BUFSIZE);
	}
	if (r == -1)
	{
		dprintf(STDERR_FILENO, "Error: Can't read from file %s\n",
			file_from);
		close_fd(fd_to);
		exit(98);
	}

	close_fd(fd_to);
}

int main(int argc, char *argv[])
{
	int fd_from;
	ssize_t r_first;
	char *buf;

	if (argc != 3)
	{
		dprintf(STDERR_FILENO, "Usage: cp file_from file_to\n");
		exit(97);
	}

	buf = create_buffer(argv[2]);

	fd_from = open(argv[1], O_RDONLY);
	if (fd_from == -1)
	{
		dprintf(STDERR_FILENO, "Error: Can't read from file %s\n",
			argv[1]);
		free(buf);
		exit(98);
	}

	r_first = read(fd_from, buf, BUFSIZE);
	if (r_first == -1)
	{
		dprintf(STDERR_FILENO, "Error: Can't read from file %s\n",
			argv[1]);
		free(buf);
		close_fd(fd_from);
		exit(98);
	}

	copy_rest(fd_from, argv[2], buf, r_first, argv[1]);

	free(buf);
	close_fd(fd_from);
	return (0);
}

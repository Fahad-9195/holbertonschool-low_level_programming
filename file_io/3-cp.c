#include <stdio.h>      /* dprintf */
#include <fcntl.h>      /* open */
#include <unistd.h>     /* read, write, close */
#include <stdlib.h>     /* malloc, free, exit */
#include <sys/types.h>
#include <sys/stat.h>

#define BUFSIZE 1024

/**
 * create_buffer - allocate 1KB buffer and handle error
 * @file_to: name of destination file (for error message)
 * Return: pointer to buffer
 */
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

/**
 * close_fd - close a file descriptor with proper error handling
 * @fd: file descriptor
 */
static void close_fd(int fd)
{
	int c = close(fd);

	if (c == -1)
	{
		dprintf(STDERR_FILENO, "Error: Can't close fd %d\n", fd);
		exit(100);
	}
}

/**
 * main - copy content of one file to another
 * @argc: argument count
 * @argv: argument vector
 * Return: 0 on success, exits with codes 97/98/99/100 on failure
 */
int main(int argc, char *argv[])
{
	int fd_from, fd_to;
	ssize_t r, w;
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
		dprintf(STDERR_FILENO, "Error: Can't read from file %s\n", argv[1]);
		free(buf);
		exit(98);
	}

	fd_to = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0664);
	if (fd_to == -1)
	{
		dprintf(STDERR_FILENO, "Error: Can't write to %s\n", argv[2]);
		free(buf);
		close_fd(fd_from);
		exit(99);
	}

	while ((r = read(fd_from, buf, BUFSIZE)) > 0)
	{
		ssize_t total = 0;

		while (total < r)
		{
			w = write(fd_to, buf + total, r - total);
			if (w == -1)
			{
				dprintf(STDERR_FILENO, "Error: Can't write to %s\n", argv[2]);
				free(buf);
				close_fd(fd_from);
				close_fd(fd_to);
				exit(99);
			}
			total += w;
		}
	}
	if (r == -1)
	{
		dprintf(STDERR_FILENO, "Error: Can't read from file %s\n", argv[1]);
		free(buf);
		close_fd(fd_from);
		close_fd(fd_to);
		exit(98);
	}

	free(buf);
	close_fd(fd_from);
	close_fd(fd_to);
	return (0);
}


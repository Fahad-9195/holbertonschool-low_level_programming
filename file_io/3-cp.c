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

	if (!buf)
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

int main(int argc, char *argv[])
{
	int fd_from, fd_to;
	ssize_t r, w, total;
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
		free(buf), exit(98);
	}

	/* جرّب القراءة أولاً لتحديد حالة 98 قبل محاولة فتح الوجهة */
	r = read(fd_from, buf, BUFSIZE);
	if (r == -1)
	{
		dprintf(STDERR_FILENO, "Error: Can't read from file %s\n", argv[1]);
		free(buf), close_fd(fd_from), exit(98);
	}

	fd_to = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0664);
	if (fd_to == -1)
	{
		dprintf(STDERR_FILENO, "Error: Can't write to %s\n", argv[2]);
		free(buf), close_fd(fd_from), exit(99);
	}

	/* اكتب أول كتلة إن وُجدت، ثم أكمل القراءة/الكتابة */
	while (r > 0)
	{
		total = 0;
		while (total < r)
		{
			w = write(fd_to, buf + total, r - total);
			if (w == -1)
			{
				dprintf(STDERR_FILENO, "Error: Can't write to %s\n", argv[2]);
				free(buf), close_fd(fd_from), close_fd(fd_to), exit(99);
			}
			total += w;
		}
		r = read(fd_from, buf, BUFSIZE);
		if (r == -1)
		{
			dprintf(STDERR_FILENO, "Error: Can't read from file %s\n", argv[1]);
			free(buf), close_fd(fd_from), close_fd(fd_to), exit(98);
		}
	}

	free(buf);
	close_fd(fd_from);
	close_fd(fd_to);
	return (0);
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <elf.h>
#include <stdint.h>

#define HDR_MAX 64 /* enough for 64-bit ELF header */

/**
 * bswap16 - swap bytes of 16-bit value
 * @x: value
 * Return: swapped value
 */
static uint16_t bswap16(uint16_t x)
{
	uint16_t v;

	v = (uint16_t)((x << 8) | (x >> 8));
	return (v);
}

/**
 * bswap32 - swap bytes of 32-bit value
 * @x: value
 * Return: swapped value
 */
static uint32_t bswap32(uint32_t x)
{
	uint32_t v;

	v = ((x & 0x000000FFU) << 24) |
	    ((x & 0x0000FF00U) << 8) |
	    ((x & 0x00FF0000U) >> 8) |
	    ((x & 0xFF000000U) >> 24);
	return (v);
}

/**
 * bswap64 - swap bytes of 64-bit value
 * @x: value
 * Return: swapped value
 */
static uint64_t bswap64(uint64_t x)
{
	uint64_t v;

	v = ((uint64_t)bswap32((uint32_t)x) << 32) |
	    (uint64_t)bswap32((uint32_t)(x >> 32));
	return (v);
}

/**
 * class_str - string for ELF class
 * @c: EI_CLASS field
 * Return: text
 */
static const char *class_str(unsigned char c)
{
	if (c == ELFCLASS32)
		return ("ELF32");
	if (c == ELFCLASS64)
		return ("ELF64");
	return ("none");
}

/**
 * data_str - string for ELF data encoding
 * @d: EI_DATA field
 * Return: text
 */
static const char *data_str(unsigned char d)
{
	if (d == ELFDATA2LSB)
		return ("2's complement, little endian");
	if (d == ELFDATA2MSB)
		return ("2's complement, big endian");
	return ("none");
}

/**
 * osabi_known_str - string for known OS/ABI values
 * @o: EI_OSABI field
 * Return: text for known values, NULL otherwise
 */
static const char *osabi_known_str(unsigned char o)
{
	switch (o)
	{
	case ELFOSABI_SYSV:
		return ("UNIX - System V");
	case ELFOSABI_HPUX:
		return ("UNIX - HP-UX");
	case ELFOSABI_NETBSD:
		return ("UNIX - NetBSD");
	case ELFOSABI_LINUX:
		return ("UNIX - Linux");
	case ELFOSABI_SOLARIS:
		return ("UNIX - Solaris");
	case ELFOSABI_AIX:
		return ("UNIX - AIX");
	case ELFOSABI_FREEBSD:
		return ("UNIX - FreeBSD");
	case ELFOSABI_OPENBSD:
		return ("UNIX - OpenBSD");
	case ELFOSABI_ARM_AEABI:
		return ("ARM EABI");
	case ELFOSABI_ARM:
		return ("ARM");
	case ELFOSABI_STANDALONE:
		return ("Standalone App");
	default:
		return (NULL);
	}
}

/**
 * print_type_line - print the ELF file Type like readelf
 * @t: e_type (host-endian)
 */
static void print_type_line(uint16_t t)
{
	const char *s;

	s = NULL;
	switch (t)
	{
	case ET_NONE:
		s = "NONE (No file type)";
		break;
	case ET_REL:
		s = "REL (Relocatable file)";
		break;
	case ET_EXEC:
		s = "EXEC (Executable file)";
		break;
	case ET_DYN:
		s = "DYN (Shared object file)";
		break;
	case ET_CORE:
		s = "CORE (Core file)";
		break;
	default:
		s = NULL;
		break;
	}

	if (s != NULL)
	{
		printf("  Type:                              %s\n", s);
		return;
	}

	if (t >= ET_LOOS && t <= ET_HIOS)
	{
		printf("  Type:                              ");
		printf("OS Specific: (0x%x)\n", t);
	}
	else if (t >= ET_LOPROC)
	{
		printf("  Type:                              ");
		printf("Processor Specific: (0x%x)\n", t);
	}
	else
	{
		printf("  Type:                              ");
		printf("UNKNOWN (0x%x)\n", t);
	}
}

/**
 * print_magic - print the Magic bytes
 * @ei: pointer to e_ident
 */
static void print_magic(const unsigned char *ei)
{
	int i;

	printf("ELF Header:\n");
	printf("  Magic:   ");
	for (i = 0; i < EI_NIDENT; i++)
	{
		printf("%02x", ei[i]);
		if (i != EI_NIDENT - 1)
			printf(" ");
	}
	printf("\n");
}

/**
 * print_ident - print Class/Data/Version/OSABI/ABIVersion lines
 * @ei: pointer to e_ident
 */
static void print_ident(const unsigned char *ei)
{
	const char *osabi;

	printf("  Class:                             %s\n",
	       class_str(ei[EI_CLASS]));
	printf("  Data:                              %s\n",
	       data_str(ei[EI_DATA]));
	if (ei[EI_VERSION] == EV_CURRENT)
		printf("  Version:                           1 (current)\n");
	else
		printf("  Version:                           %d\n",
		       ei[EI_VERSION]);

	osabi = osabi_known_str(ei[EI_OSABI]);
	printf("  OS/ABI:                            ");
	if (osabi)
		printf("%s\n", osabi);
	else
		printf("<unknown: %x>\n", ei[EI_OSABI]);

	printf("  ABI Version:                       %d\n",
	       ei[EI_ABIVERSION]);
}

/**
 * print_type_and_entry - print Type and Entry lines
 * @ei: pointer to e_ident (unused, kept for signature stability)
 * @is64: 1 if ELFCLASS64, else 0
 * @is_msb: 1 if big-endian file, else 0
 * @hdr_buf: buffer holding the header bytes
 */
static void print_type_and_entry(unsigned char *ei, int is64, int is_msb,
				 void *hdr_buf)
{
	(void)ei;

	if (is64)
	{
		Elf64_Ehdr *h;
		uint16_t type;
		uint64_t entry;

		h = (Elf64_Ehdr *)hdr_buf;
		type = h->e_type;
		entry = h->e_entry;
		if (is_msb)
		{
			type = bswap16(type);
			entry = bswap64(entry);
		}
		print_type_line(type);
		printf("  Entry point address:               0x%lx\n",
		       (unsigned long)entry);
	}
	else
	{
		Elf32_Ehdr *h2;
		uint16_t type2;
		uint32_t entry2;

		h2 = (Elf32_Ehdr *)hdr_buf;
		type2 = h2->e_type;
		entry2 = h2->e_entry;
		if (is_msb)
		{
			type2 = bswap16(type2);
			entry2 = bswap32(entry2);
		}
		print_type_line(type2);
		printf("  Entry point address:               0x%x\n",
		       entry2);
	}
}

/**
 * print_header - orchestrate printing of the ELF header fields
 * @ei: pointer to e_ident
 * @is64: 1 if ELFCLASS64, else 0
 * @is_msb: 1 if big-endian file, else 0
 * @hdr_buf: buffer holding the header bytes
 */
static void print_header(unsigned char *ei, int is64, int is_msb,
			 void *hdr_buf)
{
	print_magic(ei);
	print_ident(ei);
	print_type_and_entry(ei, is64, is_msb, hdr_buf);
}

/* ---------- small error helpers to keep main under 40 lines ---------- */

/**
 * die_usage - print usage and exit 98
 */
static void die_usage(void)
{
	fprintf(stderr, "Usage: elf_header elf_filename\n");
	exit(98);
}

/**
 * die_open - print open error and exit 98
 * @fname: file name
 */
static void die_open(const char *fname)
{
	fprintf(stderr, "Error: Can't open file %s\n", fname);
	exit(98);
}

/**
 * die_readhdr - print read header error and exit 98
 */
static void die_readhdr(void)
{
	fprintf(stderr, "Error: Can't read ELF header\n");
	exit(98);
}

/**
 * die_notelf - print not-ELF error and exit 98
 */
static void die_notelf(void)
{
	fprintf(stderr, "Error: Not an ELF file\n");
	exit(98);
}

/**
 * die_close - print close error and exit 98
 * @fd: file descriptor
 */
static void die_close(int fd)
{
	fprintf(stderr, "Error: Can't close fd %d\n", fd);
	exit(98);
}

/**
 * main - display information contained in ELF header
 * @argc: argument count
 * @argv: argument vector (argv[1] = elf filename)
 * Return: 0 on success, 98 on any error
 */
int main(int argc, char **argv)
{
	int fd;
	ssize_t r;
	unsigned char buf[HDR_MAX];
	unsigned char *ei;
	int is64;
	int is_msb;

	if (argc != 2)
	{
		die_usage();
	}

	fd = open(argv[1], O_RDONLY);
	if (fd == -1)
	{
		die_open(argv[1]);
	}

	r = read(fd, buf, HDR_MAX);
	if (r < (ssize_t)sizeof(Elf32_Ehdr))
	{
		close(fd);
		die_readhdr();
	}

	ei = ((Elf64_Ehdr *)buf)->e_ident;
	if (!(ei[EI_MAG0] == ELFMAG0 &&
	      ei[EI_MAG1] == ELFMAG1 &&
	      ei[EI_MAG2] == ELFMAG2 &&
	      ei[EI_MAG3] == ELFMAG3))
	{
		close(fd);
		die_notelf();
	}

	is64 = (ei[EI_CLASS] == ELFCLASS64);
	is_msb = (ei[EI_DATA] == ELFDATA2MSB);

	print_header(ei, is64, is_msb, buf);

	if (close(fd) == -1)
	{
		die_close(fd);
	}

	return (0);
}

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
	return (uint16_t)((x << 8) | (x >> 8));
}

/**
 * bswap32 - swap bytes of 32-bit value
 * @x: value
 * Return: swapped value
 */
static uint32_t bswap32(uint32_t x)
{
	return ((x & 0x000000FFU) << 24) |
	       ((x & 0x0000FF00U) << 8) |
	       ((x & 0x00FF0000U) >> 8) |
	       ((x & 0xFF000000U) >> 24);
}

/**
 * bswap64 - swap bytes of 64-bit value
 * @x: value
 * Return: swapped value
 */
static uint64_t bswap64(uint64_t x)
{
	return ((uint64_t)bswap32((uint32_t)x) << 32) |
	       (uint64_t)bswap32((uint32_t)(x >> 32));
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
	case ELFOSABI_SYSV: return ("UNIX - System V");
	case ELFOSABI_HPUX: return ("UNIX - HP-UX");
	case ELFOSABI_NETBSD: return ("UNIX - NetBSD");
	case ELFOSABI_LINUX: return ("UNIX - Linux");
	case ELFOSABI_SOLARIS: return ("UNIX - Solaris");
	case ELFOSABI_AIX: return ("UNIX - AIX");
	case ELFOSABI_FREEBSD: return ("UNIX - FreeBSD");
	case ELFOSABI_OPENBSD: return ("UNIX - OpenBSD");
	case ELFOSABI_ARM_AEABI: return ("ARM EABI");
	case ELFOSABI_ARM: return ("ARM");
	case ELFOSABI_STANDALONE: return ("Standalone App");
	default: return (NULL);
	}
}

/**
 * print_type - print the ELF file type line like readelf
 * @t: e_type (host-endian)
 */
static void print_type(uint16_t t)
{
	const char *s = NULL;

	switch (t)
	{
	case ET_NONE: s = "NONE (No file type)"; break;
	case ET_REL:  s = "REL (Relocatable file)"; break;
	case ET_EXEC: s = "EXEC (Executable file)"; break;
	case ET_DYN:  s = "DYN (Shared object file)"; break;
	case ET_CORE: s = "CORE (Core file)"; break;
	default:
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
		return;
	}

	printf("  Type:                              %s\n", s);
}

/**
 * print_header - print header fields as readelf -h
 * @ei: pointer to e_ident
 * @is64: 1 if ELFCLASS64, else 0
 * @is_msb: 1 if big-endian file, else 0
 * @hdr_buf: buffer holding the header bytes
 */
static void print_header(unsigned char *ei, int is64, int is_msb,
			 void *hdr_buf)
{
	int i;

	printf("ELF Header:\n");

	/* Magic */
	printf("  Magic:   ");
	for (i = 0; i < EI_NIDENT; i++)
	{
		printf("%02x", ei[i]);
		if (i != EI_NIDENT - 1)
			printf(" ");
	}
	printf("\n");

	/* Class, Data, Version, OS/ABI, ABI Version */
	printf("  Class:                             %s\n",
	       class_str(ei[EI_CLASS]));
	printf("  Data:                              %s\n",
	       data_str(ei[EI_DATA]));

	if (ei[EI_VERSION] == EV_CURRENT)
		printf("  Version:                           1 (current)\n");
	else
		printf("  Version:                           %d\n",
		       ei[EI_VERSION]);

	{
		const char *osabi = osabi_known_str(ei[EI_OSABI]);

		printf("  OS/ABI:                            ");
		if (osabi)
			printf("%s\n", osabi);
		else
			printf("<unknown: %x>\n", ei[EI_OSABI]);
	}

	printf("  ABI Version:                       %d\n",
	       ei[EI_ABIVERSION]);

	/* Type + Entry (handle endianness) */
	if (is64)
	{
		Elf64_Ehdr *h = (Elf64_Ehdr *)hdr_buf;
		uint16_t type = h->e_type;
		uint64_t entry = h->e_entry;

		if (is_msb)
		{
			type = bswap16(type);
			entry = bswap64(entry);
		}
		print_type(type);
		printf("  Entry point address:               0x%lx\n",
		       (unsigned long)entry);
	}
	else
	{
		Elf32_Ehdr *h2 = (Elf32_Ehdr *)hdr_buf;
		uint16_t type2 = h2->e_type;
		uint32_t entry2 = h2->e_entry;

		if (is_msb)
		{
			type2 = bswap16(type2);
			entry2 = bswap32(entry2);
		}
		print_type(type2);
		printf("  Entry point address:               0x%x\n", entry2);
	}
}

/**
 * main - display information contained in ELF header
 * @argc: argument count
 * @argv: argument vector (argv[1] = elf filename)
 * Return: 0 on success, 98 on any error
 *
 * Description: On any error (usage, open, read, not ELF, close) prints an
 * explanatory message to stderr and exits with status 98.
 */
int main(int argc, char **argv)
{
	int fd;
	ssize_t r;
	unsigned char buf[HDR_MAX];
	unsigned char *ei;
	int is64, is_msb;

	if (argc != 2)
	{
		fprintf(stderr, "Usage: elf_header elf_filename\n");
		exit(98);
	}

	fd = open(argv[1], O_RDONLY);
	if (fd == -1)
	{
		fprintf(stderr, "Error: Can't open file %s\n", argv[1]);
		exit(98);
	}

	r = read(fd, buf, HDR_MAX);
	if (r < (ssize_t)sizeof(Elf32_Ehdr))
	{
		fprintf(stderr, "Error: Can't read ELF header\n");
		close(fd);
		exit(98);
	}

	ei = ((Elf64_Ehdr *)buf)->e_ident;
	if (!(ei[EI_MAG0] == ELFMAG0 &&
	      ei[EI_MAG1] == ELFMAG1 &&
	      ei[EI_MAG2] == ELFMAG2 &&
	      ei[EI_MAG3] == ELFMAG3))
	{
		fprintf(stderr, "Error: Not an ELF file\n");
		close(fd);
		exit(98);
	}

	is64 = (ei[EI_CLASS] == ELFCLASS64);
	is_msb = (ei[EI_DATA] == ELFDATA2MSB);

	print_header(ei, is64, is_msb, buf);

	if (close(fd) == -1)
	{
		fprintf(stderr, "Error: Can't close fd %d\n", fd);
		exit(98);
	}

	return (0);
}

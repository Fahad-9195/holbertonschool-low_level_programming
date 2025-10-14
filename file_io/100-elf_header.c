#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <elf.h>

/**
 * bswap16 - swap byte order of 16-bit unsigned value
 * @x: value to swap
 *
 * Return: swapped value.
 */
static unsigned short bswap16(unsigned short x)
{
	return ((unsigned short)((x >> 8) | (x << 8)));
}

/**
 * bswap32 - swap byte order of 32-bit unsigned value
 * @x: value to swap
 *
 * Return: swapped value.
 */
static unsigned int bswap32(unsigned int x)
{
	unsigned int y;

	y = ((x >> 24) & 0x000000FFU) |
	    ((x >> 8) & 0x0000FF00U) |
	    ((x << 8) & 0x00FF0000U) |
	    ((x << 24) & 0xFF000000U);
	return (y);
}

/**
 * bswap64_ul - swap byte order of 64-bit value using unsigned long
 * @x: value to swap
 *
 * Description: Uses byte-by-byte shifts to avoid long long in C90.
 * Return: swapped value.
 */
static unsigned long bswap64_ul(unsigned long x)
{
	unsigned long y = 0;
	int i;

	for (i = 0; i < 8; i++)
	{
		y = (y << 8) | (x & 0xFFUL);
		x >>= 8;
	}
	return (y);
}

/**
 * die_usage - print usage error and exit(98)
 *
 * Return: Nothing (exits).
 */
static void die_usage(void)
{
	dprintf(2, "Usage: elf_header elf_filename\n");
	exit(98);
}

/**
 * die_open - print open error and exit(98)
 * @p: path that failed to open
 *
 * Return: Nothing (exits).
 */
static void die_open(const char *p)
{
	dprintf(2, "Error: Can't open file %s\n", p);
	exit(98);
}

/**
 * die_read - print read error and exit(98)
 * @p: path that failed to read
 *
 * Return: Nothing (exits).
 */
static void die_read(const char *p)
{
	dprintf(2, "Error: Can't read ELF header from %s\n", p);
	exit(98);
}

/**
 * die_not_elf - print non-ELF error and exit(98)
 * @p: given path
 *
 * Return: Nothing (exits).
 */
static void die_not_elf(const char *p)
{
	dprintf(2, "Error: Not an ELF file: %s\n", p);
	exit(98);
}

/**
 * read_header - read ELF header with at most two read() calls
 * @path: file path
 * @eh: out buffer for Elf64_Ehdr (also used for 32-bit via cast)
 *
 * Description: Reads EI_NIDENT first, validates magic, then reads
 * the rest of the header. No lseek is used.
 * Return: Nothing. On error, exits with code 98.
 */
static void read_header(const char *path, Elf64_Ehdr *eh)
{
	int fd;
	ssize_t r;
	unsigned char *id;
	size_t need;

	fd = open(path, O_RDONLY);
	if (fd == -1)
		die_open(path);

	r = read(fd, eh->e_ident, EI_NIDENT);
	if (r != (ssize_t)EI_NIDENT)
	{
		(void)close(fd);
		die_not_elf(path);
	}

	id = eh->e_ident;
	if (!(id[EI_MAG0] == ELFMAG0 &&
	      id[EI_MAG1] == ELFMAG1 &&
	      id[EI_MAG2] == ELFMAG2 &&
	      id[EI_MAG3] == ELFMAG3))
	{
		(void)close(fd);
		die_not_elf(path);
	}

	need = sizeof(*eh) - EI_NIDENT;
	r = read(fd, ((char *)eh) + EI_NIDENT, need);
	if (r != (ssize_t)need)
	{
		(void)close(fd);
		die_read(path);
	}
	(void)close(fd);
}

/**
 * normalize_type_entry - normalize e_type and entry to host endianness
 * @e64: pointer to 64-bit header buffer (also used for 32-bit via cast)
 * @cls: EI_CLASS value (ELFCLASS32/ELFCLASS64)
 * @is_msb: 1 if big endian file, else 0
 * @ptype: out normalized e_type
 * @pentry: out normalized entry as unsigned long
 *
 * Return: Nothing.
 */
static void normalize_type_entry(Elf64_Ehdr *e64, int cls, int is_msb,
				 unsigned short *ptype,
				 unsigned long *pentry)
{
	unsigned short et;
	unsigned long en;

	if (cls == ELFCLASS32)
	{
		Elf32_Ehdr *e32 = (Elf32_Ehdr *)e64;

		et = e32->e_type;
		en = (unsigned long)e32->e_entry;
		if (is_msb)
		{
			et = bswap16(et);
			en = (unsigned long)bswap32((unsigned int)en);
		}
	}
	else
	{
		et = e64->e_type;
		en = (unsigned long)e64->e_entry;
		if (is_msb)
		{
			et = bswap16(et);
			en = bswap64_ul(en);
		}
	}
	*ptype = et;
	*pentry = en;
}

/**
 * print_magic - print Magic bytes
 * @id: e_ident array
 *
 * Return: Nothing.
 */
static void print_magic(unsigned char *id)
{
	int i;

	printf("ELF Header:\n");
	printf("  Magic:   ");
	for (i = 0; i < EI_NIDENT; i++)
	{
		printf("%02x", id[i]);
		if (i != EI_NIDENT - 1)
			printf(" ");
	}
	printf("\n");
}

/**
 * print_class - print Class field
 * @cls: EI_CLASS
 *
 * Return: Nothing.
 */
static void print_class(int cls)
{
	printf("  Class:                             ");
	if (cls == ELFCLASS32)
		printf("ELF32\n");
	else if (cls == ELFCLASS64)
		printf("ELF64\n");
	else
		printf("<unknown: %x>\n", cls);
}

/**
 * print_data - print Data (endianness) field
 * @data_enc: EI_DATA
 *
 * Return: Nothing.
 */
static void print_data(int data_enc)
{
	printf("  Data:                              ");
	if (data_enc == ELFDATA2LSB)
	{
		printf("2's complement, ");
		printf("little endian\n");
	}
	else if (data_enc == ELFDATA2MSB)
	{
		printf("2's complement, ");
		printf("big endian\n");
	}
	else
	{
		printf("<unknown: %x>\n", data_enc);
	}
}

/**
 * print_version - print Version field
 * @ver: EI_VERSION
 *
 * Return: Nothing.
 */
static void print_version(int ver)
{
	printf("  Version:                           ");
	if (ver == EV_CURRENT)
		printf("1 (current)\n");
	else
		printf("%d\n", ver);
}

/**
 * print_osabi - print OS/ABI field
 * @osabi: EI_OSABI
 *
 * Return: Nothing.
 */
static void print_osabi(int osabi)
{
	printf("  OS/ABI:                            ");
	switch (osabi)
	{
	case ELFOSABI_SYSV:
		printf("UNIX - System V\n");
		break;
	case ELFOSABI_HPUX:
		printf("UNIX - HP-UX\n");
		break;
	case ELFOSABI_NETBSD:
		printf("UNIX - NetBSD\n");
		break;
	case ELFOSABI_LINUX:
		printf("UNIX - Linux\n");
		break;
	case ELFOSABI_SOLARIS:
		printf("UNIX - Solaris\n");
		break;
	case ELFOSABI_IRIX:
		printf("UNIX - IRIX\n");
		break;
	case ELFOSABI_FREEBSD:
		printf("UNIX - FreeBSD\n");
		break;
	case ELFOSABI_TRU64:
		printf("UNIX - TRU64\n");
		break;
	case ELFOSABI_ARM:
		printf("ARM\n");
		break;
	case ELFOSABI_STANDALONE:
		printf("Standalone App\n");
		break;
	default:
		printf("<unknown: %x>\n", osabi);
		break;
	}
}

/**
 * print_abiver - print ABI Version field
 * @abiv: EI_ABIVERSION
 *
 * Return: Nothing.
 */
static void print_abiver(int abiv)
{
	printf("  ABI Version:                       %d\n", abiv);
}

/**
 * print_type - print Type field
 * @etype: e_type value
 *
 * Return: Nothing.
 */
static void print_type(unsigned short etype)
{
	printf("  Type:                              ");
	switch (etype)
	{
	case ET_NONE:
		printf("NONE (No file type)\n");
		break;
	case ET_REL:
		printf("REL (Relocatable file)\n");
		break;
	case ET_EXEC:
		printf("EXEC (Executable file)\n");
		break;
	case ET_DYN:
		printf("DYN (Shared object file)\n");
		break;
	case ET_CORE:
		printf("CORE (Core file)\n");
		break;
	default:
		printf("<unknown: %x>\n", etype);
		break;
	}
}

/**
 * print_entry - print entry point
 * @entry: normalized entry address (host endianness)
 *
 * Return: Nothing.
 */
static void print_entry(unsigned long entry)
{
	printf("  Entry point address:               ");
	printf("0x%lx\n", entry);
}

/**
 * main - display selected ELF header fields (readelf -h style subset)
 * @ac: argc
 * @av: argv (expects path to ELF file)
 *
 * Return: 0 on success. On error, exits with code 98.
 */
int main(int ac, char **av)
{
	Elf64_Ehdr eh;
	unsigned char *id;
	int cls, is_msb;
	unsigned short etype;
	unsigned long entry;

	if (ac != 2)
		die_usage();

	read_header(av[1], &eh);

	id = eh.e_ident;
	cls = id[EI_CLASS];
	is_msb = (id[EI_DATA] == ELFDATA2MSB);

	normalize_type_entry(&eh, cls, is_msb, &etype, &entry);

	print_magic(id);
	print_class(cls);
	print_data(id[EI_DATA]);
	print_version(id[EI_VERSION]);
	print_osabi(id[EI_OSABI]);
	print_abiver(id[EI_ABIVERSION]);
	print_type(etype);
	print_entry(entry);

	return (0);
}

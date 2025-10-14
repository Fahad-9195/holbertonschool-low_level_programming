#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <elf.h>

/* -------------------- Byte-swap helpers -------------------- */
static unsigned short bswap16(unsigned short x)
{
	return ((unsigned short)((x >> 8) | (x << 8)));
}

static unsigned int bswap32(unsigned int x)
{
	unsigned int y;

	y = ((x >> 24) & 0x000000FFU) |
	    ((x >> 8) & 0x0000FF00U) |
	    ((x << 8) & 0x00FF0000U) |
	    ((x << 24) & 0xFF000000U);
	return (y);
}

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

/* -------------------- Error helpers -------------------- */
static void die_usage(void)
{
	dprintf(2, "Usage: elf_header elf_filename\n");
	exit(98);
}

static void die_open(const char *p)
{
	dprintf(2, "Error: Can't open file %s\n", p);
	exit(98);
}

static void die_read(const char *p)
{
	dprintf(2, "Error: Can't read ELF header from %s\n", p);
	exit(98);
}

static void die_not_elf(const char *p)
{
	dprintf(2, "Error: Not an ELF file: %s\n", p);
	exit(98);
}

/* -------------------- IO (two reads, no lseek) -------------------- */
static void read_header(const char *path, Elf64_Ehdr *eh)
{
	int fd;
	ssize_t r;
	unsigned char *id;
	size_t need;

	fd = open(path, O_RDONLY);
	if (fd == -1)
		die_open(path);

	/* read the identification bytes first (16 bytes) */
	r = read(fd, eh->e_ident, EI_NIDENT);
	if (r != (ssize_t)EI_NIDENT)
	{
		(void)close(fd);
		die_not_elf(path);
	}

	/* validate magic */
	id = eh->e_ident;
	if (!(id[EI_MAG0] == ELFMAG0 &&
	      id[EI_MAG1] == ELFMAG1 &&
	      id[EI_MAG2] == ELFMAG2 &&
	      id[EI_MAG3] == ELFMAG3))
	{
		(void)close(fd);
		die_not_elf(path);
	}

	/* read the rest of the header */
	need = sizeof(*eh) - EI_NIDENT;
	r = read(fd, ((char *)eh) + EI_NIDENT, need);
	if (r != (ssize_t)need)
	{
		(void)close(fd);
		die_read(path);
	}
	(void)close(fd);
}

/* -------------------- Normalize fields -------------------- */
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

/* -------------------- Printers -------------------- */
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

static void print_version(int ver)
{
	printf("  Version:                           ");
	if (ver == EV_CURRENT)
		printf("1 (current)\n");
	else
		printf("%d\n", ver);
}

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

static void print_abiver(int abiv)
{
	printf("  ABI Version:                       %d\n", abiv);
}

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

static void print_entry(unsigned long entry)
{
	printf("  Entry point address:               ");
	printf("0x%lx\n", entry);
}

/* -------------------- main -------------------- */
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

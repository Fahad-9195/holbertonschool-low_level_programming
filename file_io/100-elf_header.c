#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <elf.h>

/* ----- byte-swap helpers (C90-friendly) ----- */
static unsigned short bswap16(unsigned short x)
{
	return (unsigned short)((x >> 8) | (x << 8));
}

static unsigned int bswap32(unsigned int x)
{
	return ((x >> 24) & 0x000000FFU) |
	       ((x >> 8)  & 0x0000FF00U) |
	       ((x << 8)  & 0x00FF0000U) |
	       ((x << 24) & 0xFF000000U);
}

/* swap 64-bit value using only unsigned long and byte shifts */
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

int main(int ac, char **av)
{
	int fd, is_msb, cls, i;
	ssize_t r;
	union {
		Elf64_Ehdr e64;
		Elf32_Ehdr e32;
	} eh;
	unsigned char *id;
	unsigned short etype;
	unsigned long entry;

	if (ac != 2)
	{
		dprintf(2, "Usage: elf_header elf_filename\n");
		exit(98);
	}

	fd = open(av[1], O_RDONLY);
	if (fd == -1)
	{
		dprintf(2, "Error: Can't open file %s\n", av[1]);
		exit(98);
	}

	r = read(fd, &eh.e64, sizeof(eh.e64));
	if (r < (ssize_t)sizeof(Elf32_Ehdr))
	{
		dprintf(2, "Error: Can't read ELF header from %s\n", av[1]);
		close(fd);
		exit(98);
	}
	close(fd);

	id = eh.e64.e_ident;
	if (!(id[EI_MAG0] == ELFMAG0 && id[EI_MAG1] == ELFMAG1 &&
	      id[EI_MAG2] == ELFMAG2 && id[EI_MAG3] == ELFMAG3))
	{
		dprintf(2, "Error: Not an ELF file: %s\n", av[1]);
		exit(98);
	}

	cls = id[EI_CLASS];
	is_msb = (id[EI_DATA] == ELFDATA2MSB);

	/* normalize e_type and entry to host endianness, store entry in unsigned long */
	if (cls == ELFCLASS32)
	{
		etype = eh.e32.e_type;
		entry = (unsigned long)eh.e32.e_entry;
		if (is_msb)
		{
			etype = bswap16(etype);
			entry = (unsigned long)bswap32((unsigned int)entry);
		}
	}
	else /* default to 64 */
	{
		etype = eh.e64.e_type;
		entry = (unsigned long)eh.e64.e_entry;
		if (is_msb)
		{
			etype = bswap16(etype);
			entry = bswap64_ul(entry);
		}
	}

	/* ---- Output (match readelf -h fields requested) ---- */
	printf("ELF Header:\n");

	printf("  Magic:   ");
	for (i = 0; i < EI_NIDENT; i++)
	{
		printf("%02x", id[i]);
		if (i != EI_NIDENT - 1)
			printf(" ");
	}
	printf("\n");

	printf("  Class:                             ");
	if (cls == ELFCLASS32)
		printf("ELF32\n");
	else if (cls == ELFCLASS64)
		printf("ELF64\n");
	else
		printf("<unknown: %x>\n", cls);

	printf("  Data:                              ");
	if (id[EI_DATA] == ELFDATA2LSB)
		printf("2's complement, little endian\n");
	else if (id[EI_DATA] == ELFDATA2MSB)
		printf("2's complement, big endian\n");
	else
		printf("<unknown: %x>\n", id[EI_DATA]);

	printf("  Version:                           ");
	if (id[EI_VERSION] == EV_CURRENT)
		printf("1 (current)\n");
	else
		printf("%d\n", id[EI_VERSION]);

	printf("  OS/ABI:                            ");
	switch (id[EI_OSABI])
	{
	case ELFOSABI_SYSV: printf("UNIX - System V\n"); break;
	case ELFOSABI_HPUX: printf("UNIX - HP-UX\n"); break;
	case ELFOSABI_NETBSD: printf("UNIX - NetBSD\n"); break;
	case ELFOSABI_LINUX: printf("UNIX - Linux\n"); break;
	case ELFOSABI_SOLARIS: printf("UNIX - Solaris\n"); break;
	case ELFOSABI_IRIX: printf("UNIX - IRIX\n"); break;
	case ELFOSABI_FREEBSD: printf("UNIX - FreeBSD\n"); break;
	case ELFOSABI_TRU64: printf("UNIX - TRU64\n"); break;
	case ELFOSABI_ARM: printf("ARM\n"); break;
	case ELFOSABI_STANDALONE: printf("Standalone App\n"); break;
	default: printf("<unknown: %x>\n", id[EI_OSABI]); break;
	}

	printf("  ABI Version:                       %d\n", id[EI_ABIVERSION]);

	printf("  Type:                              ");
	switch (etype)
	{
	case ET_NONE: printf("NONE (No file type)\n"); break;
	case ET_REL:  printf("REL (Relocatable file)\n"); break;
	case ET_EXEC: printf("EXEC (Executable file)\n"); break;
	case ET_DYN:  printf("DYN (Shared object file)\n"); break;
	case ET_CORE: printf("CORE (Core file)\n"); break;
	default:      printf("<unknown: %x>\n", etype); break;
	}

	/* note: %lx works for both 32/64 on typical Linux (LP64 uses 64-bit unsigned long) */
	printf("  Entry point address:               0x%lx\n", entry);

	return (0);
}

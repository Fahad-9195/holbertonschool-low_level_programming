#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <elf.h>
#include <stdint.h>

#define HDR_MAX 64 /* Enough to hold 64-bit ELF header */

/* byte-swap helpers for big-endian files */
static uint16_t bswap16(uint16_t x)
{
	return (uint16_t)((x << 8) | (x >> 8));
}
static uint32_t bswap32(uint32_t x)
{
	return ((x & 0x000000FFU) << 24) |
	       ((x & 0x0000FF00U) << 8)  |
	       ((x & 0x00FF0000U) >> 8)  |
	       ((x & 0xFF000000U) >> 24);
}
static uint64_t bswap64(uint64_t x)
{
	return ((uint64_t)bswap32((uint32_t)x) << 32) |
	       (uint64_t)bswap32((uint32_t)(x >> 32));
}

static const char *class_str(unsigned char c)
{
	if (c == ELFCLASS32) return "ELF32";
	if (c == ELFCLASS64) return "ELF64";
	return "none";
}

static const char *data_str(unsigned char d)
{
	if (d == ELFDATA2LSB) return "2's complement, little endian";
	if (d == ELFDATA2MSB) return "2's complement, big endian";
	return "none";
}

static const char *osabi_str(unsigned char o)
{
	switch (o) {
	case ELFOSABI_SYSV: return "UNIX - System V";
	case ELFOSABI_HPUX: return "UNIX - HP-UX";
	case ELFOSABI_NETBSD: return "UNIX - NetBSD";
	case ELFOSABI_LINUX: return "UNIX - Linux";
	case ELFOSABI_SOLARIS: return "UNIX - Solaris";
	case ELFOSABI_AIX: return "UNIX - AIX";
	case ELFOSABI_FREEBSD: return "UNIX - FreeBSD";
	case ELFOSABI_OPENBSD: return "UNIX - OpenBSD";
	case ELFOSABI_ARM_AEABI: return "ARM EABI";
	case ELFOSABI_ARM: return "ARM";
	case ELFOSABI_STANDALONE: return "Standalone App";
	default: return "UNIX - System V";
	}
}

static void print_type(uint16_t t)
{
	const char *s = NULL;

	switch (t) {
	case ET_NONE: s = "NONE (No file type)"; break;
	case ET_REL:  s = "REL (Relocatable file)"; break;
	case ET_EXEC: s = "EXEC (Executable file)"; break;
	case ET_DYN:  s = "DYN (Shared object file)"; break;
	case ET_CORE: s = "CORE (Core file)"; break;
	default:
		if (t >= ET_LOOS && t <= ET_HIOS)
			printf("  Type:                              OS Specific: (0x%x)\n", t);
		else if (t >= ET_LOPROC) /* upper bound is always true for 16-bit */
			printf("  Type:                              Processor Specific: (0x%x)\n", t);
		else
			printf("  Type:                              UNKNOWN (0x%x)\n", t);
		return;
	}
	printf("  Type:                              %s\n", s);
}

static void print_header(unsigned char *ei, int is64, int is_msb, void *hdr_buf)
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
	printf("  Class:                             %s\n", class_str(ei[EI_CLASS]));
	printf("  Data:                              %s\n", data_str(ei[EI_DATA]));
	printf("  Version:                           %d (current)\n", ei[EI_VERSION]);
	printf("  OS/ABI:                            %s\n", osabi_str(ei[EI_OSABI]));
	printf("  ABI Version:                       %d\n", ei[EI_ABIVERSION]);

	/* Type + Entry: handle endianness if MSB */
	if (is64) {
		Elf64_Ehdr *h = (Elf64_Ehdr *)hdr_buf;
		uint16_t type = h->e_type;
		uint64_t entry = h->e_entry;

		if (is_msb) {
			type = bswap16(type);
			entry = bswap64(entry);
		}
		print_type(type);
		printf("  Entry point address:               0x%lx\n",
		       (unsigned long)entry);
	} else {
		Elf32_Ehdr *h = (Elf32_Ehdr *)hdr_buf;
		uint16_t type = h->e_type;
		uint32_t entry = h->e_entry;

		if (is_msb) {
			type = bswap16(type);
			entry = bswap32(entry);
		}
		print_type(type);
		printf("  Entry point address:               0x%x\n", entry);
	}
}

int main(int argc, char **argv)
{
	int fd;
	ssize_t r;
	unsigned char buf[HDR_MAX];
	unsigned char *ei;
	int is64, is_msb;

	if (argc != 2) {
		dprintf(STDERR_FILENO, "Usage: elf_header elf_filename\n");
		exit(98);
	}

	fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		dprintf(STDERR_FILENO, "Error: Can't open file %s\n", argv[1]);
		exit(98);
	}

	r = read(fd, buf, HDR_MAX);
	if (r < (ssize_t)sizeof(Elf32_Ehdr)) {
		dprintf(STDERR_FILENO, "Error: Can't read ELF header\n");
		close(fd);
		exit(98);
	}

	ei = ((Elf64_Ehdr *)buf)->e_ident;
	if (!(ei[EI_MAG0] == ELFMAG0 && ei[EI_MAG1] == ELFMAG1 &&
	      ei[EI_MAG2] == ELFMAG2 && ei[EI_MAG3] == ELFMAG3)) {
		dprintf(STDERR_FILENO, "Error: Not an ELF file\n");
		close(fd);
		exit(98);
	}

	is64 = (ei[EI_CLASS] == ELFCLASS64);
	is_msb = (ei[EI_DATA] == ELFDATA2MSB);

	print_header(ei, is64, is_msb, buf);

	if (close(fd) == -1) {
		dprintf(STDERR_FILENO, "Error: Can't close fd %d\n", fd);
		exit(98);
	}
	return 0;
}

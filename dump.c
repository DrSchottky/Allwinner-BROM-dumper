#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <stddef.h>

void *memmem(const void *haystack, size_t hlen, const void *needle, size_t nlen);
void get_brom_info(void *brom_mem, int len);

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: sudo %s <BROM start address> <BROM size> <outfile>\n", argv[0]);
        return 0;
    }

    off_t offset = strtoul(argv[1], NULL, 0);
    size_t len = strtoul(argv[2], NULL, 0);

    int brom_fd = open("/dev/mem", O_RDONLY | O_SYNC);
    void *brom_mem = mmap(NULL, len, PROT_READ, MAP_SHARED, brom_fd, offset);
    if (brom_mem == MAP_FAILED) {
        perror("Error: mmap error. Wrong address?");
        return 1;
    }
	
	get_brom_info(brom_mem, len);

	FILE *outfile_fd = fopen(argv[3], "wb");
	if (!outfile_fd)
	{
		perror("Error: can't open outfile");
        return 1;
	}
	
	if (fwrite(brom_mem, 1, len, outfile_fd) != len)
	{
		perror("Error: can't write to outfile");
		fclose(outfile_fd);
        return 1;
	}	
	fclose(outfile_fd);
	
	printf("BROM dumped successfully!\n");

    return 0;
}

void get_brom_info(void *brom_mem, int len)
{
	char *brom_magic = "eGON.BRM";
	void *brom_magic_ptr = memmem(brom_mem, len, brom_magic, strlen(brom_magic));
	if (!brom_magic_ptr)
	{
		perror("Warning: can't find BROM magic\n");
		return;
	}
	printf("BROM magic (eGON.BRM) found!\n");
	ptrdiff_t brom_magic_offset = brom_magic_ptr - brom_mem;
	printf("BROM header starts at 0x%X\n", brom_magic_offset - 8);
	printf("BROM header size is %d\n", ((unsigned char*)brom_mem)[brom_magic_offset + 8]);
	printf("BROM boot version is %.4s\n", &((char*)brom_mem)[brom_magic_offset + 12]);
	printf("BROM eGON is %.4s\n", &((char*)brom_mem)[brom_magic_offset + 16]);
	printf("BROM platform is %.8s\n", &((char*)brom_mem)[brom_magic_offset + 20]);
}

void *memmem(const void *haystack, size_t hlen, const void *needle, size_t nlen)
{
    int needle_first;
    const void *p = haystack;
    size_t plen = hlen;

    if (!nlen)
        return NULL;

    needle_first = *(unsigned char *)needle;

    while (plen >= nlen && (p = memchr(p, needle_first, plen - nlen + 1)))
    {
        if (!memcmp(p, needle, nlen))
            return (void *)p;

        p++;
        plen = hlen - (p - haystack);
    }
	
	return NULL;
}
  

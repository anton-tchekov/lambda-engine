#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "zlib.h"

#define DAT_VERSION_NO 0

void printcomma(char *buf, int n);

int main(int argc, char **argv)
{
	static const unsigned char _dat_identifier[4] = { 'D', 'A', 'T', DAT_VERSION_NO };
	int rv;
	unsigned char *cdata, *udata;
	unsigned long csize, usize;
	FILE *out, *in;
	if(argc != 3)
	{
		fprintf(stderr, "Usage: ./compression output-file input-file\n");
		return 1;
	}

	printf("Loading \"%s\"\n", argv[2]);
	if(!(in = fopen(argv[2], "rb")))
	{
		fprintf(stderr, "Failed to open input file \"%s\"\n", argv[2]);
		return 1;
	}

	fseek(in, 0, SEEK_END);
	usize = ftell(in);
	fclose(in);

	if(!(udata = malloc(usize)))
	{
		fprintf(stderr, "Failed to allocate memory for the uncompressed data\n");
		return 1;
	}

	in = fopen(argv[2], "rb");
	if(fread(udata, 1, usize, in) != usize)
	{
		fprintf(stderr, "Failed to read data \"%s\"\n", argv[2]);
		free(udata);
		return 1;
	}

	fclose(in);

	csize = (11 * usize) / 10 + 12;
	if(!(cdata = malloc(csize)))
	{
		fprintf(stderr, "Failed to allocate memory for the compressed data\n");
		free(udata);
		return 1;
	}

	fprintf(stderr, "Compressing data\n");
	if((rv = compress(cdata, &csize, udata, usize)) != Z_OK)
	{
		fprintf(stderr, "Error while compressing data\n");
		free(udata);
		free(cdata);
		return 1;
	}

	{
		char a[32], b[32];
		printcomma(a, csize);
		printcomma(b, usize);
		printf("Compressed/Uncompressed size ratio (%s/%s) = %.2f%%\n", a, b, (float)csize / (float)usize * 100.0f);
	}

	free(udata);
	fprintf(stderr, "Writing compressed data to \"%s\"\n", argv[1]);
	if(!(out = fopen(argv[1], "wb")))
	{
		fprintf(stderr, "Failed to open file \"%s\" for writing\n", argv[1]);
		free(cdata);
		return 1;
	}

	/* Write DAT identifier */
	if(fwrite(_dat_identifier, 1, 4, out) != 4)
	{
		fprintf(stderr, "Failed to write DAT identifier\n");
		free(cdata);
		return 1;
	}

	/* Write decompressed data size */
	if(fwrite(&usize, 4, 1, out) != 1)
	{
		fprintf(stderr, "Failed to write uncompressed data size\n");
		free(cdata);
		return 1;
	}

	/* Write compressed data size */
	if(fwrite(&csize, 4, 1, out) != 1)
	{
		fprintf(stderr, "Failed to write compressed data size\n");
		free(cdata);
		return 1;
	}

	/* Write compressed data */
	if(fwrite(cdata, 1, csize, out) != csize)
	{
		fprintf(stderr, "Failed to write compressed data\n");
		free(cdata);
		return 1;
	}

	free(cdata);
	fclose(out);
	return 0;
}

void printcomma(char *buf, int n)
{
	char *p;
	int n2, scale;
	n2 = 0;
	scale = 1;
	p = buf;
	if(n < 0)
	{
		*p++ = '-';
		n = -n;
	}

	while(n >= 1000)
	{
		n2 = n2 + scale * (n % 1000);
		n /= 1000;
		scale *= 1000;
	}

	p += sprintf(p, "%d", n);
	while(scale != 1)
	{
		scale /= 1000;
		n = n2 / scale;
		n2 = n2  % scale;
		p += sprintf(p, ",%03d", n);
	}
}


#include <stdio.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "lib/image.h"

#define ACF_VERSION_NO 0

int ilog2(int x)
{
	return __builtin_ctz(x);
}

int main(int argc, char *argv[])
{
	static const unsigned char header[4] = { 'A', 'C', 'F', ACF_VERSION_NO };
	unsigned char *data = NULL;
	int i, j, size, width, height, channels, w, h;
	FILE *fp = NULL;
	if(argc != 8)
	{
		fprintf(stderr, "Usage: ./cubemap_convert \"output-file\" \"right\" \"left\" \"top\" \"bottom\" \"front\" \"back\"\n");
		return 1;
	}

	if(!(fp = fopen(argv[1], "wb")))
	{
		fprintf(stderr, "Failed to open output file \"%s\"\n", argv[1]);
		return 1;
	}

	if(fwrite(header, 1, 4, fp) != 4)
	{
		fprintf(stderr, "Failed to write ACF indentifier\n");
		goto fail;
	}

	stbi_set_flip_vertically_on_load(0);

	if(!(data = stbi_load(argv[2], &width, &height, &channels, 0)))
	{
		fprintf(stderr, "Failed to load cubemap texture \"%s\"\n", argv[2]);
		goto fail;
	}

	if(width < 1 || width != height)
	{
		fprintf(stderr, "Cubemap texture must be square and at least 1x1 pixel in size\n");
		goto fail;
	}

	size = ilog2(width);
	if((1 << size) != width)
	{
		fprintf(stderr, "Cubemap texture size must be a power of two\n");
		goto fail;
	}

	if(fwrite((unsigned char *)&size, 1, 4, fp) != 4)
	{
		fprintf(stderr, "Failed to write ACF size\n");
		goto fail;
	}

	fprintf(stderr, "Loading cubemap texture #%d \"%s\"\n", 1, argv[2]);
	for(j = 0; j < width * width; ++j)
	{
		unsigned char r, g, b, a;
		r = data[channels * j];
		g = data[channels * j + 1];
		b = data[channels * j + 2];
		a = 0xFF;
		fwrite(&b, 1, 1, fp);
		fwrite(&g, 1, 1, fp);
		fwrite(&r, 1, 1, fp);
		fwrite(&a, 1, 1, fp);
	}

	stbi_image_free(data);

	for(i = 3; i < 8; ++i)
	{
		fprintf(stderr, "Loading cubemap texture #%d \"%s\"\n", i - 1, argv[i]);
		if(!(data = stbi_load(argv[i], &w, &h, &channels, 0)))
		{
			fprintf(stderr, "Failed to load cubemap texture \"%s\"\n", argv[i]);
			goto fail;
		}

		if(w != width || h != height)
		{
			fprintf(stderr, "Cubemap texture sizes and formats must be the same\n");
			stbi_image_free(data);
			goto fail;
		}

		for(j = 0; j < width * width; ++j)
		{
			unsigned char r, g, b, a;
			r = data[channels * j];
			g = data[channels * j + 1];
			b = data[channels * j + 2];
			a = 0xFF;
			fwrite(&b, 1, 1, fp);
			fwrite(&g, 1, 1, fp);
			fwrite(&r, 1, 1, fp);
			fwrite(&a, 1, 1, fp);
		}

		stbi_image_free(data);
	}

	fclose(fp);
	return 0;

fail:
	if(data)
	{
		stbi_image_free(data);
	}

	fclose(fp);
	return 1;
}


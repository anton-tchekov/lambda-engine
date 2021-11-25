/* Convert a power of two sized image (.png, .jpg, .bmp etc.)
to the .AIF 8-bit uncompressed bgra image format */
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "lib/image.h"

#define CHANNELS     4
#define SIZE         1024
#define TEXTURE_SIZE (CHANNELS * SIZE * SIZE)

static const unsigned char _aif_header[4] = { 'A', 'I', 'F', 0 };

int _log2(int x)
{
	if(x <= 0)
	{
		return 0;
	}

	return __builtin_ctz(x);
}

unsigned char *load_image(const char *filename)
{
	unsigned char *data;
	int i, width, height, channels;
	if(!(data = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha)))
	{
		fprintf(stderr, "Failed to load texture \"%s\"\n", filename);
		return NULL;
	}

	fprintf(stderr,
		"Image info:\n"
		"Channels: %4d\n"
		"Width:    %4d\n"
		"Height:   %4d\n",
		channels, width, height);

	if(width != SIZE || height != SIZE || channels != CHANNELS)
	{
		int j;
		unsigned char *new;
		fprintf(stderr, "Image is not %dx%d: Extending\n", SIZE, SIZE);
		if(!(new = malloc(TEXTURE_SIZE)))
		{
			fprintf(stderr, "malloc() failed\n");
			return NULL;
		}

		for(i = 0; i < TEXTURE_SIZE; ++i)
		{
			new[i] = 0;
		}

		for(i = 0; i < height; ++i)
		{
			for(j = 0; j < width; ++j)
			{
				new[4 * (i * SIZE + j) + 0] = 255;
				new[4 * (i * SIZE + j) + 1] = 255;
				new[4 * (i * SIZE + j) + 2] = 255;
				new[4 * (i * SIZE + j) + 3] = data[4 * (i * width + j) + 0];
			}
		}

		data = new;
	}
	else
	{
		for(i = 0; i < TEXTURE_SIZE; i += 4)
		{
			unsigned char temp;
			temp = data[i];
			data[i] = data[i + 2];
			data[i + 2] = temp;
		}
	}

	return data;
}

int main(int argc, char *argv[])
{
	int i, count;
	FILE *fp;
	if(argc < 3)
	{
		fprintf(stderr, "Usage: ./img_convert output-file input-file-0 ...\n");
		return 1;
	}

	count = argc - 2;

	if(!(fp = fopen(argv[1], "wb")))
	{
		fprintf(stderr, "Failed to open file \"%s\" for writing\n", argv[1]);
		return 1;
	}

	if(fwrite(_aif_header, 1, 4, fp) != 4)
	{
		fprintf(stderr, "Failed to write AIF header\n");
		return 1;
	}

	if(fwrite(&count, 1, 4, fp) != 4)
	{
		fprintf(stderr, "Failed to write AIF texture count\n");
		return 1;
	}

	for(i = 0; i < count; ++i)
	{
		unsigned char *data = load_image(argv[i + 2]);
		if(!data)
		{
			return 1;
		}

		if(fwrite(data, 1, TEXTURE_SIZE, fp) != TEXTURE_SIZE)
		{
			fprintf(stderr, "Failed to write AIF data\n");
			return 1;
		}
	}

	fclose(fp);
	return 0;
}


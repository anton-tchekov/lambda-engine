#include <stdio.h>
#include <stdint.h>
#include <locale.h>
#include "lib/utf8.h"
#include "lib/ft2build.h"
#include FT_FREETYPE_H

#define TEST

#define ARG_OUTPUT_FILE 1
#define ARG_NUM_FONTS   2

typedef struct FONT_CHAR
{
	unsigned char *Bitmap;
	int32_t Character;
	int Location[2], Size[2], Bearing[2], Advance[2];
} FontChar;

typedef struct FONT_SIZE
{
	FontChar *Characters;
	unsigned int NumCharacters, Size;
} FontSize;

typedef struct FONT
{
	char *Name;
	FontSize *Sizes;
	unsigned int NumSizes;
} Font;

#define arrlen(n) (sizeof(n) / sizeof(*n))

static int _compare_int32(const void *a, const void *b);
static int _sort_unique(int32_t *elements, int count);
static int32_t *str_unique_codepoints(const char *s, int *length);
static int _load_font(FT_Library library, char *font_name, int num_sizes, int *sizes, int32_t **cps, int *lengths, Font *font);
static int _load_size(FT_Face face, int size, int32_t *cps, int length, FontSize *fs);
static int _load_char(FT_Face face, int32_t c, FontChar *fc);
static int aff_write(const char *filename, Font *fonts, unsigned int num_fonts);
static int fc_compare_height(const void *a, const void *b);
static int _blit(unsigned char *dst, unsigned int width,
		unsigned char *src, unsigned int bw, unsigned int bh, unsigned int x, unsigned int y);
static int _pack_sorted_chars(unsigned int size, FontChar **chars, unsigned int num_chars);
static int _bmp_write_bw(const char *filename, const unsigned char *data, unsigned int width, unsigned int height);
static int _write_char(FontChar *fchar, FILE *fp);
static int _log2(int x);

#ifdef TEST
static void _test_print_character(FontChar *fc);
#endif

int main(int argc, char *argv[])
{
	FT_Library library;
	char *font_name;
	int32_t *cps, **cpss;
	int i, j, k, len, num_fonts, num_sizes, size, arg_idx, *sizes, *lengths;
	Font *fonts;

	/* Set locale so that non-ascii characters are displayed correctly */
	setlocale(LC_CTYPE, "");

	/* Check correct usage */
	if(argc < 7)
	{
		fprintf(stderr, "Usage:\n./font_convert output-file num-input-fonts input-font-0 num-font-sizes font-size-0 characters-0 ...\n");
		return 1;
	}

	/* Get how many fonts are used */
	if(!(num_fonts = strtol(argv[ARG_NUM_FONTS], NULL, 10)))
	{
		fprintf(stderr, "Invalid number of fonts\n");
		return 1;
	}

	if(!(fonts = malloc(num_fonts * sizeof(Font))))
	{
		fprintf(stderr, "Failed to allocate memory for fonts struct\n");
		return 1;
	}

	/* Initialize FreeType */
	if(FT_Init_FreeType(&library))
	{
		fprintf(stderr, "FreeType initialization failed\n");
		return 1;
	}

	arg_idx = 2;
	for(i = 0; i < num_fonts; ++i)
	{
		font_name = argv[++arg_idx];
		if(!(num_sizes = strtol(argv[++arg_idx], NULL, 10)))
		{
			fprintf(stderr, "Invalid number of font sizes\n");
			return 1;
		}

		if(!(sizes = malloc(num_sizes * sizeof(int))))
		{
			fprintf(stderr, "Failed to allocate memory for font sizes\n");
			return 1;
		}

		if(!(cpss = malloc(num_sizes * sizeof(int32_t *))))
		{
			fprintf(stderr, "Failed to allocate memory for list of codepoint lists\n");
			return 1;
		}

		if(!(lengths = malloc(num_sizes * sizeof(int32_t *))))
		{
			fprintf(stderr, "Failed to allocate memory for list of codepoint lengths\n");
			return 1;
		}

		for(j = 0; j < num_sizes; ++j)
		{
			if(!(size = strtol(argv[++arg_idx], NULL, 10)))
			{
				fprintf(stderr, "Invalid font size\n");
				return 1;
			}

			cps = str_unique_codepoints(argv[++arg_idx], &len);
			sizes[j] = size;
			cpss[j] = cps;
			lengths[j] = len;
		}

		if(_load_font(library, font_name, num_sizes, sizes, cpss, lengths, fonts + i))
		{
			return 1;
		}

		free(lengths);
		free(sizes);
		for(j = 0; j < num_sizes; ++j)
		{
			free(cpss[j]);
		}

		free(cpss);
	}

	if(aff_write(argv[1], fonts, num_fonts))
	{
		return 1;
	}

	/* Clean up */
	for(i = 0; i < num_fonts; ++i)
	{
		for(j = 0; j < (int)fonts[i].NumSizes; ++j)
		{
			for(k = 0; k < (int)fonts[i].Sizes[j].NumCharacters; ++k)
			{
				free(fonts[i].Sizes[j].Characters[k].Bitmap);
			}

			free(fonts[i].Sizes[j].Characters);
		}

		free(fonts[i].Sizes);
	}

	free(fonts);
	FT_Done_FreeType(library);
	return 0;
}

static int _log2(int x)
{
	if(x <= 0)
	{
		return 0;
	}

	return __builtin_ctz(x);
}

static int fc_compare_height(const void *a, const void *b)
{
	FontChar *f1, *f2;
	f1 = *(FontChar **)a;
	f2 = *(FontChar **)b;
	if(f1->Size[1] < f2->Size[1])
	{
		return 1;
	}

	if(f1->Size[1] == f2->Size[1])
	{
		return 0;
	}

	if(f1->Size[1] > f2->Size[1])
	{
		return -1;
	}

	return 0;
}

/* Generate AFF file */
static int aff_write(const char *filename, Font *fonts, unsigned int num_fonts)
{
	static const unsigned char aff_header[4] = { 'A', 'F', 'F', 0 };
	FILE *fp;
	unsigned char *data;
	unsigned int i, j, k, tex_size, chars_total, area_total;
	char fwext[256];
	Font *font;
	FontSize *size;
	FontChar *fchar;
	FontChar **hsorted;
	chars_total = 0;
	area_total = 0;

	for(i = 0; i < num_fonts; ++i)
	{
		for(j = 0; j < fonts[i].NumSizes; ++j)
		{
			for(k = 0; k < fonts[i].Sizes[j].NumCharacters; ++k)
			{
#ifdef TEST
				_test_print_character(&fonts[i].Sizes[j].Characters[k]);
				fprintf(stderr, "\n\n");
#endif
				++chars_total;
			}
		}
	}

	if(!(hsorted = malloc(chars_total * sizeof(FontChar **))))
	{
		fprintf(stderr, "Failed to allocate memory for pointers to all the characters\n");
		return 1;
	}

	chars_total = 0;
	for(i = 0; i < num_fonts; ++i)
	{
		font = &fonts[i];
		for(j = 0; j < fonts[i].NumSizes; ++j)
		{
			size = &font->Sizes[j];
			for(k = 0; k < size->NumCharacters; ++k)
			{
				fchar = &size->Characters[k];
				area_total += fchar->Size[0] * fchar->Size[1];
				hsorted[chars_total++] = fchar;
			}
		}
	}

	qsort(hsorted, chars_total, sizeof(FontChar *), fc_compare_height);

#ifdef TEST
	printf("Total Area: %d\n", area_total);

	printf("Printing Sorted Heights:\n");
	for(i = 0; i < chars_total; ++i)
	{
		printf("%d\n", hsorted[i]->Size[1]);
	}
#endif

	for(i = 64; i < 1024; i *= 2)
	{
		if(!_pack_sorted_chars(i, hsorted, chars_total))
		{
			break;
		}

#ifdef TEST
		fprintf(stderr, "Size %d is to small, continuing to next power of two\n", i);
#endif
	}

	tex_size = i;

#ifdef TEST
	fprintf(stderr, "Texture size: %d\n", tex_size);
#endif

	if(tex_size > 1024)
	{
		fprintf(stderr, "Characters do not fit into maximum sized texture 1024x1024\n");
		return 1;
	}

	if(!(data = calloc(tex_size * tex_size, 1)))
	{
		fprintf(stderr, "Failed to allocate memory for bitmap data\n");
		return 1;
	}

	for(i = 0; i < num_fonts; ++i)
	{
		font = &fonts[i];
		for(j = 0; j < fonts[i].NumSizes; ++j)
		{
			size = &font->Sizes[j];
			for(k = 0; k < size->NumCharacters; ++k)
			{
				fchar = &size->Characters[k];
				_blit(data, tex_size, fchar->Bitmap, fchar->Location[0], fchar->Location[1], fchar->Size[0], fchar->Size[1]);
			}
		}
	}

	snprintf(fwext, sizeof(fwext), "%s.bmp", filename);
	_bmp_write_bw(fwext, data, tex_size, tex_size);

	snprintf(fwext, sizeof(fwext), "%s.aff", filename);
	if(!(fp = fopen(fwext, "wb")))
	{
		fprintf(stderr, "Failed to open file \"%s\" for writing\n", filename);
		return 1;
	}

	if(fwrite(aff_header, 1, 4, fp) != 4)
	{
		fprintf(stderr, "Failed to write AFF header\n");
		return 1;
	}

	{
		unsigned int p2ts = _log2(tex_size);
		if(fwrite(&p2ts, 1, 4, fp) != 4)
		{
			return 1;
		}
	}

	if(fwrite(&num_fonts, 1, 4, fp) != 4)
	{
		return 1;
	}

	for(i = 0; i < num_fonts; ++i)
	{
		if(fwrite(&fonts[i].NumSizes, 1, 4, fp) != 4)
		{
			return 1;
		}
	}

	for(i = 0; i < num_fonts; ++i)
	{
		for(j = 0; j < fonts[i].NumSizes; ++j)
		{
			if(fwrite(&fonts[i].Sizes[j].Size, 1, 4, fp) != 4)
			{
				return 1;
			}

			if(fwrite(&fonts[i].Sizes[j].NumCharacters, 1, 4, fp) != 4)
			{
				return 1;
			}
		}
	}

	for(i = 0; i < num_fonts; ++i)
	{
		for(j = 0; j < fonts[i].NumSizes; ++j)
		{
			for(k = 0; k < fonts[i].Sizes[j].NumCharacters; ++k)
			{
				if(_write_char(&fonts[i].Sizes[j].Characters[k], fp))
				{
					return 1;
				}
			}
		}
	}

	free(hsorted);
	free(data);
	fclose(fp);
	return 0;
}

static int _write_char(FontChar *fchar, FILE *fp)
{
	if(fwrite(&fchar->Character, 1, 4, fp) != 4) { return 1; }
	if(fwrite(&fchar->Advance[0], 1, 4, fp) != 4) { return 1; }
	if(fwrite(&fchar->Location[0], 1, 4, fp) != 4) { return 1; }
	if(fwrite(&fchar->Location[1], 1, 4, fp) != 4) { return 1; }
	if(fwrite(&fchar->Size[0], 1, 4, fp) != 4) { return 1; }
	if(fwrite(&fchar->Size[1], 1, 4, fp) != 4) { return 1; }
	if(fwrite(&fchar->Bearing[0], 1, 4, fp) != 4) { return 1; }
	if(fwrite(&fchar->Bearing[1], 1, 4, fp) != 4) { return 1; }
	return 0;
}

static int _blit(unsigned char *dst, unsigned int width,
		unsigned char *src, unsigned int x, unsigned int y, unsigned int w, unsigned int h)
{
	unsigned int i, j, k, l;
	for(k = 0, j = y; j < y + h; ++j, ++k)
	{
		for(l = 0, i = x; i < x + w; ++i, ++l)
		{
			dst[j * width + i] = src[k * w + l];
		}
	}

	return 0;
}

static int _pack_sorted_chars(unsigned int size, FontChar **chars, unsigned int num_chars)
{
	unsigned int i, cur_height, cur_x, cur_y;
	cur_height = chars[0]->Size[1];
	cur_x = 0;
	cur_y = 0;
	for(i = 0; i < num_chars; ++i)
	{
		if(cur_x + chars[i]->Size[0] > size)
		{
			cur_x = 0;
			cur_y += cur_height;
			cur_height = chars[i]->Size[1];
		}

		if(cur_y + chars[i]->Size[1] > size)
		{
			return 1;
		}

		chars[i]->Location[0] = cur_x;
		chars[i]->Location[1] = cur_y;
		cur_x += chars[i]->Size[0];
	}

	return 0;
}

#ifdef TEST

/* Print characters to terminal */
static void _test_print_character(FontChar *fc)
{
	int x, y;
	for(y = 0; y < fc->Size[1]; ++y)
	{
		for(x = 0; x < fc->Size[0]; ++x)
		{
			putc(fc->Bitmap[y * fc->Size[0] + x] > 128 ? '#' : ' ', stderr);
		}

		putc('\n', stderr);
	}
}

#endif

/* UTF-8 and string functions */
static int _compare_int32(const void *a, const void *b)
{
	const int32_t *ia = (const int32_t *)a;
	const int32_t *ib = (const int32_t *)b;
	return *ia  - *ib;
}

static int _sort_unique(int32_t *elements, int count)
{
	int i, j;
	qsort(elements, count, sizeof(*elements), _compare_int32);
	for(i = 1, j = 1; i < count; ++i)
	{
		if(elements[i] == elements[i - 1])
		{
			continue;
		}

		elements[j++] = elements[i];
	}

	return j;
}

static int32_t *str_unique_codepoints(const char *s, int *length)
{
	static const char *printable_ascii = " !\"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
	int count;
	char *p;
	int32_t *cp, c;
	if(!strcmp(s, "printable_ascii"))
	{
		s = printable_ascii;
	}

	count = utf8len(s);
	if(!(cp = malloc(count * sizeof(*cp))))
	{
		fprintf(stderr, "Failed to allocate memory for UCS-4 string representation\n");
		return NULL;
	}

	count = 0;
	p = utf8codepoint(s, &c);
	while(c)
	{
		cp[count++] = c;
		p = utf8codepoint(p, &c);
	}

	*length = _sort_unique(cp, count);
	return cp;
}

/* Font loading */
static int _load_font(FT_Library library, char *font_name, int num_sizes, int *sizes, int32_t **cps, int *lengths, Font *font)
{
	int i;
	FT_Face face;

#ifdef TEST
	fprintf(stderr, "Loading font \"%s\"\n", font_name);
#endif

	if(!(font->Sizes = malloc(num_sizes * sizeof(FontSize))))
	{
		fprintf(stderr, "Failed to allocate memory for font sizes struct\n");
		return 1;
	}

	font->NumSizes = num_sizes;
	if(FT_New_Face(library, font_name, 0, &face))
	{
		fprintf(stderr, "Failed to load font \"%s\"\n", font_name);
		free(font->Sizes);
		return 1;
	}

	for(i = 0; i < num_sizes; ++i)
	{
		if(_load_size(face, sizes[i], cps[i], lengths[i], font->Sizes + i))
		{
			fprintf(stderr, "Failed to load font size %d\n", sizes[i]);
			return 1;
		}
	}

	FT_Done_Face(face);
	return 0;
}

static int _load_size(FT_Face face, int size, int32_t *cps, int length, FontSize *fs)
{
	int i;
	fs->Size = size;
	fs->NumCharacters = length;
	if(!(fs->Characters = malloc(length * sizeof(FontChar))))
	{
		fprintf(stderr, "Failed to allocate memory for font characters struct\n");
		return 1;
	}

	FT_Set_Pixel_Sizes(face, 0, size);
	for(i = 0; i < length; ++i)
	{
		if(_load_char(face, cps[i], fs->Characters + i))
		{
			return 1;
		}
	}

	return 0;
}

static int _load_char(FT_Face face, int32_t c, FontChar *fc)
{
#ifdef TEST
	fprintf(stderr, "Loading character \'%lc\'\n", c);
#endif

	if(FT_Load_Char(face, c, FT_LOAD_RENDER))
	{
		fprintf(stderr, "Failed to load character: \'%lc\'", c);
		return 1;
	}

	fc->Character = c;
	fc->Size[0] = face->glyph->bitmap.width;
	fc->Size[1] = face->glyph->bitmap.rows;
	fc->Bearing[0] = face->glyph->bitmap_left;
	fc->Bearing[1] = face->glyph->bitmap_top;
	fc->Advance[0] = face->glyph->advance.x >> 6;
	fc->Advance[1] = face->glyph->advance.y >> 6;
	if(!(fc->Bitmap = malloc(fc->Size[0] * fc->Size[1])))
	{
		fprintf(stderr, "Failed to allocate memory for character bitmap\n");
		return 1;
	}

	memcpy(fc->Bitmap, face->glyph->bitmap.buffer, fc->Size[0] * fc->Size[1]);
	return 0;
}

#define BYTES_PER_PIXEL   3
#define FILE_HEADER_SIZE 14
#define INFO_HEADER_SIZE 40

static int _bmp_write_bw(const char *filename, const unsigned char *data, unsigned int width, unsigned int height)
{
	static unsigned char padding[3];
	unsigned char file_header[FILE_HEADER_SIZE], info_header[INFO_HEADER_SIZE];
	unsigned int i, j, padding_size, file_size;
	FILE *fp;
	if(!(fp = fopen(filename, "wb")))
	{
		fprintf(stderr, "Failed to open BMP file for writing\n");
		return 1;
	}

	padding_size = (4 - (width * BYTES_PER_PIXEL) % 4) % 4;

	file_size = FILE_HEADER_SIZE + INFO_HEADER_SIZE + (BYTES_PER_PIXEL * width + padding_size) * height;
	file_header[0] = 'B';
	file_header[1] = 'M';
	file_header[2] = (unsigned char)file_size;
	file_header[3] = (unsigned char)(file_size >> 8);
	file_header[4] = (unsigned char)(file_size >> 16);
	file_header[5] = (unsigned char)(file_size >> 24);
	file_header[6] = 0;
	file_header[7] = 0;
	file_header[8] = 0;
	file_header[9] = 0;
	file_header[10] = (unsigned char)(FILE_HEADER_SIZE + INFO_HEADER_SIZE);
	file_header[11] = 0;
	file_header[12] = 0;
	file_header[13] = 0;

	info_header[0] = (unsigned char)INFO_HEADER_SIZE;
	info_header[1] = 0;
	info_header[2] = 0;
	info_header[3] = 0;
	info_header[4] = (unsigned char)width;
	info_header[5] = (unsigned char)(width >> 8);
	info_header[6] = (unsigned char)(width >> 16);
	info_header[7] = (unsigned char)(width >> 24);
	info_header[8] = (unsigned char)height;
	info_header[9] = (unsigned char)(height >> 8);
	info_header[10] = (unsigned char)(height >> 16);
	info_header[11] = (unsigned char)(height >> 24);
	info_header[12] = (unsigned char)1;
	info_header[13] = 0;
	info_header[14] = (unsigned char)(BYTES_PER_PIXEL * 8);
	for(i = 15; i < INFO_HEADER_SIZE; ++i)
	{
		info_header[i] = 0;
	}

	fwrite(file_header, 1, FILE_HEADER_SIZE, fp);
	fwrite(info_header, 1, INFO_HEADER_SIZE, fp);
	i = height - 1;
	do
	{
		for(j = 0; j < width; ++j)
		{
			unsigned char v = data[i * width + j];
			unsigned char tw[BYTES_PER_PIXEL] = { v, v, v };
			fwrite(tw, 1, BYTES_PER_PIXEL, fp);
		}

		fwrite(padding, 1, padding_size, fp);
	}
	while(i-- > 0);

	fclose(fp);
	return 0;
}


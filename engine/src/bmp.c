#include "bmp.h"
#include "log.h"
#include <stdio.h>

#define BYTES_PER_PIXEL   3
#define FILE_HEADER_SIZE 14
#define INFO_HEADER_SIZE 40

int bmp_write(const char *filename, const u8 *data, i32 width, i32 height)
{
	static u8 padding[3];
	u8 file_header[FILE_HEADER_SIZE], info_header[INFO_HEADER_SIZE];
	i32 i, j, padding_size, file_size;
	FILE *fp;
	if(!(fp = fopen(filename, "wb")))
	{
		log_error("Failed to open BMP file for writing");
		return 1;
	}

	padding_size = (4 - (width * BYTES_PER_PIXEL) % 4) % 4;

	file_size = FILE_HEADER_SIZE + INFO_HEADER_SIZE + (BYTES_PER_PIXEL * width + padding_size) * height;
	file_header[0] = 'B';
	file_header[1] = 'M';
	file_header[2] = (u8)file_size;
	file_header[3] = (u8)(file_size >> 8);
	file_header[4] = (u8)(file_size >> 16);
	file_header[5] = (u8)(file_size >> 24);
	file_header[6] = 0;
	file_header[7] = 0;
	file_header[8] = 0;
	file_header[9] = 0;
	file_header[10] = (u8)(FILE_HEADER_SIZE + INFO_HEADER_SIZE);
	file_header[11] = 0;
	file_header[12] = 0;
	file_header[13] = 0;

	info_header[0] = (u8)INFO_HEADER_SIZE;
	info_header[1] = 0;
	info_header[2] = 0;
	info_header[3] = 0;
	info_header[4] = (u8)width;
	info_header[5] = (u8)(width >> 8);
	info_header[6] = (u8)(width >> 16);
	info_header[7] = (u8)(width >> 24);
	info_header[8] = (u8)height;
	info_header[9] = (u8)(height >> 8);
	info_header[10] = (u8)(height >> 16);
	info_header[11] = (u8)(height >> 24);
	info_header[12] = (u8)1;
	info_header[13] = 0;
	info_header[14] = (u8)(BYTES_PER_PIXEL * 8);
	for(i = 15; i < INFO_HEADER_SIZE; ++i)
	{
		info_header[i] = 0;
	}

	fwrite(file_header, 1, FILE_HEADER_SIZE, fp);
	fwrite(info_header, 1, INFO_HEADER_SIZE, fp);
	for(i = 0; i < height; ++i)
	{
		for(j = 0; j < width; ++j)
		{
			fwrite(data + 4 * (i * width  + j), 1, BYTES_PER_PIXEL, fp);
		}

		fwrite(padding, 1, (size_t)padding_size, fp);
	}

	fclose(fp);
	return 0;
}


/*
	WAV to AAF converter
	Optionally converts stereo audio to mono by either
	merging the left and right channels or extracting
	just the left or right channel.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"

#define AAF_VERSION_NO          0

#define AAF_SAMPLE_RATE_MASK     ((1UL << 30) - 1)
#define AAF_BITS_PER_SAMPLE_BIT  (1UL << 30)
#define AAF_CHANNELS_BIT         (1UL << 31)

enum FORMAT
{
	LMONO,
	RMONO,
	MMONO,
	STEREO
};

typedef struct WAV
{
	u32 SampleRate, Length;
	u16 NumChannels, BitsPerSample;
	u8 *Data;
} WAV;

typedef struct AAF
{
	u32 Format, Length;
	u8 *Data;
} AAF;

static const u8 *mem_mem
	(const void *a, size_t a_len, const void *b, size_t b_len);

static int wav_load(const char *filename, WAV *wav);
static int wav_convert_to_aaf(WAV *in, AAF *out, int format);
static int aaf_write(const char *filename, AAF *aaf);

int main(int argc, char **argv)
{
	static const char *fmts[4] = { "lmono", "rmono", "mmono", "stereo" };
	int i;
	WAV in;
	AAF out;
	int format;
	char *fmtstr;

	if(argc != 4)
	{
		fprintf(stderr,
				"Not enough arguments\n"
				"Usage: ./audio_convert wav-file-in aaf-file-out format"
				"Format: %s/%s/%s/%s\n",
				fmts[0], fmts[1], fmts[2], fmts[3]);
		return 1;
	}

	if(wav_load(argv[1], &in))
	{
		return 1;
	}

	fmtstr = argv[3];
	for(i = 0; i < 4; ++i)
	{
		if(!strcmp(fmtstr, fmts[i]))
		{
			format = i;
			break;
		}
	}

	if(i == 4)
	{
		fprintf(stderr, "Invalid format\n");
		goto fail;
	}

	if(wav_convert_to_aaf(&in, &out, format))
	{
		goto fail;
	}

	if(aaf_write(argv[2], &out))
	{
		goto fail;
	}

	free(in.Data);
	return 0;

fail:
	free(in.Data);
	return 1;
}

static const u8 *mem_mem
	(const void *a, size_t a_len, const void *b, size_t b_len)
{
	size_t i;
	const u8 *a8, *b8;
	a8 = (u8 *)a;
	b8 = (u8 *)b;
	if(b_len == 0)
	{
		return a;
	}

	if(a_len < b_len)
	{
		return 0;
	}

	for(i = 0; i <= a_len - b_len; ++i)
	{
		if(!memcmp(a8 + i, b8, b_len))
		{
			return a8 + i;
		}
	}

	return 0;
}

static int wav_convert_to_aaf(WAV *in, AAF *out, int format)
{
	fprintf(stderr, "NumChannels = %d\nSampleRate = %d\nBitsPerSample = %d\n", in->NumChannels, in->SampleRate, in->BitsPerSample);

	out->Format = in->SampleRate & AAF_SAMPLE_RATE_MASK;
	if(in->BitsPerSample == 16)
	{
		out->Format |= AAF_BITS_PER_SAMPLE_BIT;
	}

	if(in->NumChannels == 1)
	{
		if(format == LMONO || format == RMONO || format == MMONO)
		{
			out->Length = in->Length;
			out->Data = in->Data;
		}
		else
		{
			fprintf(stderr, "Conversion from mono to stereo audio is not supported\n");
			return 1;
		}
	}
	else
	{
		if(format == LMONO || format == RMONO || format == MMONO)
		{
			u32 stride, stride_new, i, j;
			fprintf(stderr, "Converting stereo audio to mono\n");
			out->Length = in->Length >> 1;
			out->Data = in->Data;
			stride_new = in->BitsPerSample / 8;
			stride = stride_new * 2;
			switch(format)
			{
				case LMONO:
				{
					for(j = 0, i = 0; i < in->Length; i += stride, j += stride_new)
					{
						memmove(out->Data + j, in->Data + i, stride_new);
					}
					break;
				}

				case RMONO:
				{
					for(j = 0, i = 0; i < in->Length; i += stride, j += stride_new)
					{
						memmove(out->Data + j, in->Data + i + stride_new, stride_new);
					}
					break;
				}

				case MMONO:
				{
					if(stride_new == 1)
					{
						for(j = 0, i = 0; i < in->Length; i += 2, ++j)
						{
							out->Data[j] = in->Data[i] / 2 + in->Data[i + 1] / 2;
						}
					}
					else
					{
						for(j = 0, i = 0; i < in->Length; i += 4, j += 2)
						{
							i16 left, right, mix;
							left = in->Data[i] | (in->Data[i + 1] << 8);
							right = in->Data[i + 2] | (in->Data[i + 3] << 8);
							mix = left / 2 + right / 2;
							out->Data[j] = (u8)mix;
							out->Data[j + 1] = (u8)(mix >> 8);
						}
					}
					break;
				}
			}
		}
		else
		{
			out->Format |= AAF_CHANNELS_BIT;
			out->Length = in->Length;
			out->Data = in->Data;
		}
	}

	return 0;
}

static int aaf_write(const char *filename, AAF *aaf)
{
	static const u8 header[4] = { 'A', 'A', 'F', AAF_VERSION_NO };
	FILE *fp;
	if(!(fp = fopen(filename, "wb")))
	{
		fprintf(stderr, "Failed to open \"%s\" for writing AAF\n", filename);
		return 1;
	}

	if(fwrite(header, 1, 4, fp) != 4)
	{
		fprintf(stderr, "Failed to write AAF indentifier\n");
		goto fail;
	}

	if(fwrite((u8 *)&aaf->Format, 1, 4, fp) != 4)
	{
		fprintf(stderr, "Failed to write AAF format\n");
		goto fail;
	}

	if(fwrite((u8 *)&aaf->Length, 1, 4, fp) != 4)
	{
		fprintf(stderr, "Failed to write AAF data length\n");
		goto fail;
	}

	if(fwrite(aaf->Data, 1, aaf->Length, fp) != aaf->Length)
	{
		fprintf(stderr, "Failed to write AAF data\n");
		goto fail;
	}

	fclose(fp);
	return 0;

fail:
	fclose(fp);
	return 1;
}

/*
	WAV file format description:
	http://soundfile.sapp.org/doc/WaveFormat/
*/
static int wav_load(const char *filename, WAV *wav)
{
	u32 data_length, sample_rate;
	FILE *fp;
	u8 buf[512];
	size_t br;
	u16 num_channels, bits_per_sample;
	u8 *data;
	const u8 *data_ptr;
	data = NULL;

	if(!(fp = fopen(filename, "rb")))
	{
		fprintf(stderr, "Failed to open WAV file\n");
		return 0;
	}

	if((br = fread(buf, 1, sizeof(buf), fp)) != sizeof(buf))
	{
		fprintf(stderr, "Failed to read WAV header\n");
		goto error;
	}

	if(memcmp(buf, "RIFF", 4))
	{
		fprintf(stderr, "File is not in RIFF format\n");
		goto error;
	}

	if(memcmp(buf + 8, "WAVEfmt ", 8))
	{
		fprintf(stderr, "File is not a valid WAV file\n");
		goto error;
	}

	/* Subchunk1Size - PCM */
	if(*(u32 *)(buf + 16) != 16)
	{
		fprintf(stderr, "File is not in PCM format\n");
		goto error;
	}

	/* AudioFormat - Linear Quantization */
	if(*(u16 *)(buf + 20) != 1)
	{
		fprintf(stderr, "PCM header is not 0x01\n");
		goto error;
	}

	/* NumChannels */
	num_channels = *(u16 *)(buf + 22);
	if(num_channels != 1 && num_channels != 2)
	{
		fprintf(stderr, "Only mono and stereo sound is supported\n");
		goto error;
	}

	/* SampleRate */
	sample_rate = *(u32 *)(buf + 24);

	/* BitsPerSample */
	bits_per_sample = *(u16 *)(buf + 34);

	if(!(data_ptr = mem_mem(buf + 36, 512 - 36, "data", 4)))
	{
		fprintf(stderr, "Data chunk not found\n");
		goto error;
	}

	data_length = *(u32 *)(data_ptr + 4);
	fseek(fp, data_ptr - buf + 8, SEEK_SET);

	if(!(data = malloc(data_length)))
	{
		fprintf(stderr, "Failed to allocate memory for audio data\n");
		goto error;
	}

	if((fread(data, 1, data_length, fp)) != data_length)
	{
		fprintf(stderr, "Failed to read audio data\n");
		goto error;
	}

	wav->Data = data;
	wav->Length = data_length;
	wav->NumChannels = num_channels;
	wav->SampleRate = sample_rate;
	wav->BitsPerSample = bits_per_sample;
	fclose(fp);
	return 0;

error:
	fclose(fp);
	return 1;
}


#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "readfile.h"

#define arrlen(a) (sizeof(a) / sizeof(*a))

static const char *warn = "/* This is a generated file. DO NOT EDIT */\n";

static int get_c_name(const char *restrict path, char *restrict out, int max_out_len);
static int check_c_name(char *s);
static char *format_glsl(const char *restrict in, u32 length);

/*
	argv[1]: output header file
	argv[2..]: input shader files
*/
int main(int argc, char *argv[])
{
	FILE *fpc, *fph;
	int i, j;
	u32 length;
	char *out, *content, filename[128], f2[128];
	if(argc < 2)
	{
		fprintf(stderr, "No output header and source files specified\n");
		return 1;
	}

	if(argc < 3)
	{
		fprintf(stderr, "No shader files specified\n");
		return 1;
	}

	for(i = 0; argv[1][i]; ++i)
	{
		filename[i] = argv[1][i];
	}

	j = i;
	filename[j++] = '.';
	filename[j++] = 'h';
	filename[j++] = '\0';
	if(!(fph = fopen(filename, "w")))
	{
		fprintf(stderr, "Error opening header file for writing\n");
		return 1;
	}

	j = i;
	filename[j++] = '.';
	filename[j++] = 'c';
	if(!(fpc = fopen(filename, "w")))
	{
		fprintf(stderr, "Error opening source file for writing\n");
		fclose(fph);
		return 1;
	}

	if(get_c_name(filename, f2, sizeof(f2)))
	{
		fprintf(stderr, "Filename contains invalid characters\n");
		goto fail;
	}

	fprintf(fpc, "%s#include \"%s.h\"\n\n", warn, f2);
	for(j = 0; f2[j]; ++j)
	{
		f2[j] = (char)toupper(f2[j]);
	}

	fprintf(fph, "%s"
		"#ifndef __%s_H__\n"
		"#define __%s_H__\n\n", warn, f2, f2);

	for(i = 2; i < argc; ++i)
	{
		if(!(content = readfile(argv[i], &length)))
		{
			fprintf(stderr, "Error reading file \"%s\"\n", argv[i]);
			goto fail;
		}

		if(get_c_name(argv[i], filename, sizeof(filename)))
		{
			fprintf(stderr, "Filename contains invalid characters\n");
			free(content);
			goto fail;
		}

		if(!(out = format_glsl(content, length)))
		{
			free(content);
			goto fail;
		}

		fprintf(fph, "extern const char *%s;\n", filename);
		fprintf(fpc, "const char *%s = \"%s\";\n", filename, out);
		free(out);
		free(content);
	}

	fprintf(fph, "\n#endif\n\n");
	fprintf(fpc, "\n");

	fclose(fph);
	fclose(fpc);
	return 0;

fail:
	fclose(fph);
	fclose(fpc);
	return 1;
}

static char *format_glsl(const char *restrict in, u32 length)
{
	const char *p, *q, *s;
	char *r, *out, c, d;
	int pp, n, addnl;
	if(!(out = malloc(length)))
	{
		return NULL;
	}

	r = out;
	p = in;
	addnl = 0;
	for(;;)
	{
next:
		for(; isspace(*p); ++p)
		{
			if(*p == '\n')
			{
				++p;
				goto next;
			}

			if(!*p)
			{
				goto end;
			}
		}

		pp = *p == '#';
		if(pp)
		{
			if(addnl)
			{
				*r++ = '\\';
				*r++ = 'n';
				addnl = 0;
			}
		}
		else
		{
			addnl = 1;
		}

		for(q = p; *p != '\n'; ++p)
		{
			if(*p == '/' && (c = *(p + 1)) == '*')
			{
				for(; *p != '*' && (c = *(p + 1)) != '/'; ++p)
				{
					if(c == '\0')
					{
						goto end;
					}
				}
			}

			if(!*p)
			{
				goto end;
			}
		}

		for(s = p; isspace(*p); --p) ;

		c = '\0';
		while(q <= p)
		{
			d = *q;
			if(isspace(c) && isspace(d))
			{
				++q;
				continue;
			}

			n = 0;
			if(d == '+' || d == '-' || d == '*' || d == '/' || d == ',' ||
				d == '=' || (!pp && d == '(') || d == ')' || d == '<' || d == '>' ||
				d == '[' || d == ']' || d == ';' || d == '^' || d == '|' ||
				d == '&' || d == '{' || d == '}')
			{
				if(isspace(c))
				{
					--r;
				}

				if(*(q + 1) == ' ')
				{
					n = 1;
				}
			}

			c = d;
			*r++ = d;
			++q;
			if(n)
			{
				++q;
			}
		}

		if(pp)
		{
			*r++ = '\\';
			*r++ = 'n';
		}

		p = s;
	}

end:
	*r = '\0';
	return out;
}

static int get_c_name(const char *restrict path, char *restrict out, int max_out_len)
{
	int name_length, i;
	const char *p, *last_slash;
	for(last_slash = path, p = path; *p; ++p)
	{
		if(*p == '/')
		{
			last_slash = p + 1;
		}
	}

	for(i = 0, name_length = 0; last_slash[name_length] && last_slash[name_length] != '.'; ++name_length)
	{
		out[i++] = last_slash[name_length];
		if(i == max_out_len - 1)
		{
			return 1;
		}
	}

	out[i] = '\0';
	return check_c_name(out);
}

static int check_c_name(char *s)
{
	if(!(isalpha(*s) || *s == '_'))
	{
		return 1;
	}

	for(++s; *s; ++s)
	{
		if(!(isalnum(*s) || *s == '_'))
		{
			return 1;
		}
	}

	return 0;
}


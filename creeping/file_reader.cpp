#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <algorithm>

#include "file_reader.h"

#define True 1
#define False 0

ConfFile::
ConfFile(){
	pClf = new ACREEPING_LINE;
	memset(pClf, 0, sizeof(*pClf));
	loadDefault(pClf);
}

ConfFile::
~ConfFile(){
	delete pClf;
}

const ACREEPING_LINE * ConfFile::
get_conf(){
	return (const ACREEPING_LINE *) pClf;
}

char * ConfFile::
trim_left(char *src)
{
	if (!src)
		return NULL;
	int len = strlen(src);
	int start = 0;

	for (start = 0; start < len; start++)
	{
		if (src[start] == ' ' || src[start] == '\t')
			continue;
		break;
	}
	return strdup(&src[start]);
}

char * ConfFile::
trim_right(char *src)
{
	if (!src)
		return NULL;
	char *ret = strdup(src);
	int len = strlen(ret);
	int start = 0;

	for (start = len - 1; start >= 0; start--)
	{
		char c = ret[start];
		if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
			continue;
		break;
	}
	ret[start + 1] = 0;
	return ret;
}

int ConfFile::
strSplit(const char *src, const char limiter, char *key, char *val)
{
	int len = strlen(src);
	int i = 0, increment = 1, limit = len;

	char *tmp1 = NULL;
	char *tmp2 = NULL;

	while (i != limit)
	{
		if (src[i] == limiter)
		{
			if (key)
			{
				strncpy(key, src, i);
				key[i] = 0;
				tmp1 = trim_right(key);
				tmp2 = trim_left(tmp1);
				strcpy(key, tmp2);
				free(tmp1);
				free(tmp2);
			}
			if (val)
			{
				strncpy(val, &src[i + 1], len - i - 1);
				val[len - i - 1] = 0;
				tmp1 = trim_right(val);
				tmp2 = trim_left(tmp1);
				strcpy(val, tmp2);
				free(tmp1);
				free(tmp2);
			}
			return True;
		}
		i += increment;
	}
	return False;
}

void ConfFile::
loadDefault(ACREEPING_LINE * pClf)
{
	if (pClf->lineText)
		free(pClf->lineText);
	pClf->lineText = strdup("Creeping Line");

	pClf->bRss = 0;
	pClf->bResizing = 1;
	pClf->ScrollingSpeed = 100;

	pClf->font.lfHeight = -40;
	pClf->font.lfWidth = 0;
	pClf->font.lfEscapement = 0;
	pClf->font.lfOrientation = 0;
	pClf->font.lfWeight = 400;
	pClf->font.lfItalic = 0;
	pClf->font.lfUnderline = 0;
	pClf->font.lfStrikeOut = 0;
	pClf->font.lfCharSet = 1;
	pClf->font.lfOutPrecision = 0;
	pClf->font.lfClipPrecision = 0;
	pClf->font.lfQuality = 0;
	pClf->font.lfPitchAndFamily = 0;

	strcpy(pClf->font.lfFaceName, "Arial");

	pClf->window.clForeground = 0;
	pClf->window.clBackground = 0xc8d0d4;
	pClf->window.clTransparentColor = 0xdcdcdc;
	pClf->window.TransparencyFactor = 200;

	pClf->window.rect.left = 0;
	pClf->window.rect.top = 0;
	pClf->window.rect.right = 600;
	pClf->window.rect.bottom = 70;
}

char * ConfFile::
unescape(const char *str)
{
	int len, i, idx;
	char *tmp, *ret;

	idx = 0;
	len = strlen(str);
	tmp = (char *) malloc(len + 1);

	for (i = 0; i < len; i++)
	{
		if ((i == 0 || i == len - 1) && str[i] == '"')
			continue;
		if (str[i] == '\\')
			i++;
		tmp[idx++] = str[i];
	}
	tmp[idx] = 0;
	ret = strdup(tmp);
	free(tmp);
	return ret;
}


int ConfFile::
mygets(FILE * f, char **buf)
{
	char *buffer = (char*)malloc(1024);
	int size = 0;

	while (fread(buffer + size, 1, 1, f) == 1)
	{
		if (*(buffer + size) == '\n')
			break;
		size++;
		if (size % 1024 == 0)
		{
			buffer = (char*)realloc(buffer, size + 1024);
		}
	}
	if (!size)
	{
		free(buffer);
		return 0;
	}

	*(buffer + size) = 0;
	*buf = buffer;
	return size;
}

int ConfFile::
open(const char *filename)
{
	char *line;
	char *key;
	char *val;

	FILE *f;

	if (strcmp(filename, "-") == 0)
	{
		f = stdin;
	}
	else
	{
		f = fopen(filename, "r");
		if (!f)
		{
			fprintf(stderr, "Open configuration file failed: %s\n", filename);
			return errno;
		}
	}

	while (!feof(f))
	{
		// Read the next line
		int len = mygets(f, &line);
		if (!len)
		{
			if (feof(f))
				break;

			fprintf(stderr, "fgets");
			return errno;
		}
		key = (char *) malloc(len);
		val = (char *) malloc(len);

		memset(key, 0, len);
		memset(val, 0, len);

		if (strSplit(line, '=', key, val))
		{
			if (strcmp(key, "General.LineText") == 0)
			{
				if (pClf->lineText)
					free(pClf->lineText);
				pClf->lineText = unescape(val);
			}
			else if (strcmp(key, "General.bUseRSS") == 0)
			{
				pClf->bRss = atoi(val);
			}
			else if (strcmp(key, "General.bResizing") == 0)
			{
				pClf->bResizing = atoi(val);
			}
			else if (strcmp(key, "General.ScrollingSpeed") == 0)
			{
				pClf->ScrollingSpeed = atoi(val);
			}
			else if (strcmp(key, "Font.lfHeight") == 0)
			{
				pClf->font.lfHeight = atoi(val);
			}
			else if (strcmp(key, "Font.lfWidth") == 0)
			{
				pClf->font.lfWidth = atoi(val);
			}
			else if (strcmp(key, "Font.lfEscapement") == 0)
			{
				pClf->font.lfEscapement = atoi(val);
			}
			else if (strcmp(key, "Font.lfOrientation") == 0)
			{
				pClf->font.lfOrientation = atoi(val);
			}
			else if (strcmp(key, "Font.lfWeight") == 0)
			{
				pClf->font.lfWeight = atoi(val);
			}
			else if (strcmp(key, "Font.lfItalic") == 0)
			{
				pClf->font.lfItalic = atoi(val);
			}
			else if (strcmp(key, "Font.lfUnderline") == 0)
			{
				pClf->font.lfUnderline = atoi(val);
			}
			else if (strcmp(key, "Font.lfStrikeOut") == 0)
			{
				pClf->font.lfStrikeOut = atoi(val);
			}
			else if (strcmp(key, "Font.lfCharSet") == 0)
			{
				pClf->font.lfCharSet = atoi(val);
			}
			else if (strcmp(key, "Font.lfOutPrecision") == 0)
			{
				pClf->font.lfOutPrecision = atoi(val);
			}
			else if (strcmp(key, "Font.lfClipPrecision") == 0)
			{
				pClf->font.lfClipPrecision = atoi(val);
			}
			else if (strcmp(key, "Font.lfQuality") == 0)
			{
				pClf->font.lfQuality = atoi(val);
			}
			else if (strcmp(key, "Font.lfPitchAndFamily") == 0)
			{
				pClf->font.lfPitchAndFamily = atoi(val);
			}
			else if (strcmp(key, "Font.lfFaceName") == 0)
			{
				strcpy(pClf->font.lfFaceName, val);
			}
			else if (strcmp(key, "Font.clTextColor") == 0)
			{
				pClf->window.clForeground = strtol(val, NULL, 16);
			}
			else if (strcmp(key, "Font.clTextSize") == 0)
			{
				pClf->window.clTextSize = atoi(val);
			}
			else if (strcmp(key, "Window.clBackground") == 0)
			{
				pClf->window.clBackground = strtol(val, NULL, 16);
			}
			else if (strcmp(key, "Window.clTransparentColor") == 0)
			{
				pClf->window.clTransparentColor = strtol(val, NULL, 16);
			}
			else if (strcmp(key, "Window.TransparencyFactor") == 0)
			{
				pClf->window.TransparencyFactor = atoi(val);
			}
			else if (strcmp(key, "Window.Position") == 0)
			{
				pClf->window.position = atoi(val);
			}
			else if (strcmp(key, "Window.Rect.left") == 0)
			{
				pClf->window.rect.left = atoi(val);
			}
			else if (strcmp(key, "Window.Rect.top") == 0)
			{
				pClf->window.rect.top = atoi(val);
			}
			else if (strcmp(key, "Window.Rect.right") == 0)
			{
				pClf->window.rect.right = atoi(val);
			}
			else if (strcmp(key, "Window.Rect.bottom") == 0)
			{
				pClf->window.rect.bottom = atoi(val);
			}
		}
		free(line);
		free(key);
		free(val);
	}
	fclose(f);
	return 0;
}

void  ConfFile::
wnd_pos_from_conf(int display_width, int display_height, int * x, 
					int * y, int *z, int * width, int * height)
{
	int view_id, view_x, view_y, view_width, view_height;
	view_id = view_x = view_y = view_width = view_height = -1;

	if (getenv("SIGNAGE_VIEW_NUMBER") &&
		getenv("SIGNAGE_VIEW_X") &&
		getenv("SIGNAGE_VIEW_Y") &&
		getenv("SIGNAGE_VIEW_WIDTH") &&
		getenv("SIGNAGE_VIEW_HEIGHT"))
	{
		view_id = atoi(getenv("SIGNAGE_VIEW_NUMBER"));
		view_x = atoi(getenv("SIGNAGE_VIEW_X"));
		view_y = atoi(getenv("SIGNAGE_VIEW_Y"));
		view_width = atoi(getenv("SIGNAGE_VIEW_WIDTH"));
		view_height = atoi(getenv("SIGNAGE_VIEW_HEIGHT"));

		unsetenv("SIGNAGE_VIEW_NUMBER");
		unsetenv("SIGNAGE_VIEW_X");
		unsetenv("SIGNAGE_VIEW_Y");
		unsetenv("SIGNAGE_VIEW_WIDTH");
		unsetenv("SIGNAGE_VIEW_HEIGHT");
	}

	if(view_id)
		*z = view_id;
	
	if (pClf->window.position == WINDOW_POSITION_FIXED)
	{
		*x = pClf->window.rect.left;
		*y = pClf->window.rect.top;
		*width  = pClf->window.rect.bottom - pClf->window.rect.left;
		*height = pClf->window.rect.bottom - pClf->window.rect.top;
		if (view_id != -1) 
		{
			*x += view_x;
			*y += view_y;
			*width  =  std::max(0, std::min(*width,  view_width + view_x  - *x));
			*height =  std::max(0, std::min(*height, view_height + view_y - *y));
		}
	}else{
		*height = pClf->window.clTextSize;

		if (view_id == -1) {
			*width = display_width;
			*x = 0;

			if (pClf->window.position == WINDOW_POSITION_TOP)
			{
				*y = 0;
			} else if (pClf->window.position == WINDOW_POSITION_BOTOM)
			{
				*y = 0;
				*y = display_height - *height;
			}
			else
			{
				fprintf(stderr, "Unknown value of window position \n");
				exit(1);
			}
		} else {
			*width = view_width;
			*x = view_x;
			if (pClf->window.position == WINDOW_POSITION_TOP)
			{
				*y = view_y;
			}
			else if (pClf->window.position == WINDOW_POSITION_BOTOM)
			{
				*y = view_y + view_height -  *height;
			}
			else
			{
				fprintf(stderr, "Unknown value of window position \n");
				exit(1);
			}
		}

	}

	
}

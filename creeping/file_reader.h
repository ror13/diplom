#ifndef FILE_READER_H
#define FILE_READER_H

#define LF_FACESIZE 32

#define WINDOW_POSITION_FIXED 0
#define WINDOW_POSITION_TOP 1
#define WINDOW_POSITION_BOTOM 2

#ifndef _WIN32
typedef struct tagLOGFONT {
  long lfHeight;
  long lfWidth;
  long lfEscapement;
  long lfOrientation;
  long lfWeight;
  long lfItalic;
  long lfUnderline;
  long lfStrikeOut;
  long lfCharSet;
  long lfOutPrecision;
  long lfClipPrecision;
  char lfQuality;
  char lfPitchAndFamily;
  char lfFaceName[LF_FACESIZE];
} LOGFONT, *PLOGFONT;
#else
#include <windows.h>
#endif
typedef struct _ARECT
{
	int left;
	int top;
	int right;
	int bottom;
} ARECT;

typedef struct _AWINDOW
{
	unsigned int clForeground;
	unsigned int clBackground;
	int clTextSize;
	unsigned int clTransparentColor;
	int TransparencyFactor;
	unsigned int position;

	ARECT rect;
} AWINDOW;

typedef struct _ACREEPING_LINE
{
	char *lineText;
	int bResizing;
	int ScrollingSpeed;
	int bRss;

	LOGFONT	font;
	AWINDOW	window;
} ACREEPING_LINE;


class ConfFile
{
	public:
		ConfFile();
		~ConfFile();
		int open(const char *filename);
		int open(const char *data, int data_size);
		void wnd_pos_from_conf(int display_width, int display_height, int * x, 
								int * y, int *z, int * width, int * height);
		const ACREEPING_LINE * get_conf();
	private:
		int mygets(FILE * f, char **buf);
		char * unescape(const char *str);
		void loadDefault(ACREEPING_LINE * pClf);
		int strSplit(const char *src, const char limiter, char *key, char *val);
		char * trim_right(char *src);
		char * trim_left(char *src);
		void parse_line(char * line, int len);
		ACREEPING_LINE * pClf;
};
#endif //FILE_READER_H

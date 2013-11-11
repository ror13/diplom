#ifndef FONT_RENDER_H
#define FONT_RENDER_H

#include <pango/pangoft2.h>

#include "file_reader.h"

#define FW_DONTCARE  0
#define FW_THIN  100
#define FW_EXTRALIGHT  200
#define FW_ULTRALIGHT  200
#define FW_LIGHT  300
#define FW_NORMAL  400
#define FW_REGULAR  400
#define FW_MEDIUM  500
#define FW_SEMIBOLD  600
#define FW_DEMIBOLD  600
#define FW_BOLD  700
#define FW_EXTRABOLD  800
#define FW_ULTRABOLD  800
#define FW_HEAVY  900
#define FW_BLACK  90

class FontRender
{
	public:
		FontRender(const ACREEPING_LINE * conf);
		~FontRender();
		const void * rendering(const char * txt, int * width, int * height );
	private:
		PangoFontDescription * font_description_from_logfont(const LOGFONT * lfp);
		
		bool underline;
		bool strikeout;
		PangoLayout *g_layout;
		PangoContext *pango_context;
		PangoFontDescription *pango_font;
		PangoFontMap * pango_fontmap;
};
#endif //FONT_RENDER_H

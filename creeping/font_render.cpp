#include "font_render.h"
#include "debug_utils.h"
//font map has bugs when unref
//becouse i using this bad code
//if you now haw fix it fix this static member
static PangoFontMap * global_pango_fontmap = NULL;
PangoFontMap * get_global_pango_fontmap()
{
	if(global_pango_fontmap == NULL)
		global_pango_fontmap = pango_ft2_font_map_new ();
	return global_pango_fontmap;
}
//end bad code
FontRender::
FontRender(const ACREEPING_LINE * conf)
{
	DEBUG_PRINT_LINE;
	pango_font = font_description_from_logfont(&conf->font);
	pango_font_description_set_size(pango_font, conf->window.clTextSize * PANGO_SCALE);
	PangoFontMap * pango_fontmap = get_global_pango_fontmap();
	pango_context =  pango_font_map_create_context(pango_fontmap);
	pango_context_set_font_description(pango_context, pango_font);
	
	g_layout = pango_layout_new(pango_context);
	pango_layout_set_font_description(g_layout, pango_font);
	pango_layout_set_width(g_layout, -1);
	pango_layout_set_alignment(g_layout, PANGO_ALIGN_LEFT);
	
	underline = conf->font.lfUnderline ? true : false;
	strikeout = conf->font.lfStrikeOut ? true : false;
	DEBUG_PRINT_LINE;
}

FontRender::
~FontRender()
{	
	DEBUG_PRINT_LINE;
	g_object_unref(g_layout);	
	g_object_unref(pango_context);	
	pango_font_description_free(pango_font);
	DEBUG_PRINT_LINE;
}

const void * FontRender::
rendering(const char * text, int * width, int * height)
{
	DEBUG_PRINT_LINE;
	gchar* res_text = NULL;
	const gchar* underline_txt = "";
	const gchar* strikeout_txt = "";
	gchar *escaped_text = g_strdup(text);

	DEBUG_PRINT_LINE;if (underline) {
		underline_txt = " underline=\"single\"";
	}

	if (strikeout) {
		strikeout_txt = " strikethrough=\"true\"";
	}
	res_text = g_strconcat("<span ", underline_txt, strikeout_txt, ">", escaped_text, "</span>", NULL);
	pango_layout_set_markup(g_layout, res_text, -1);
	g_free(escaped_text);
	g_free(res_text);
	pango_layout_get_pixel_size(g_layout,width,height);
	DEBUG_PRINT_LINE;
	return (const void *) g_layout;
}



PangoFontDescription * FontRender::
font_description_from_logfont(const LOGFONT * lfp)
{
	DEBUG_PRINT_LINE;
	PangoFontDescription *description;
	gchar *family;
	PangoStyle style;
	PangoVariant variant;
	PangoWeight weight;
	PangoStretch stretch;

	family = g_strdup(lfp->lfFaceName);

	if (!lfp->lfItalic)
		style = PANGO_STYLE_NORMAL;
	else
		style = PANGO_STYLE_ITALIC;

	variant = PANGO_VARIANT_NORMAL;

	/* The PangoWeight values PANGO_WEIGHT_* map exactly do Windows FW_*
	 * values.  Is this on purpose? Quantize the weight to exact
	 * PANGO_WEIGHT_* values. Is this a good idea?
	 */
	if (lfp->lfWeight == FW_DONTCARE)
		weight = PANGO_WEIGHT_NORMAL;
	else if (lfp->lfWeight <= (FW_ULTRALIGHT + FW_LIGHT) / 2)
		weight = PANGO_WEIGHT_ULTRALIGHT;
	else if (lfp->lfWeight <= (FW_LIGHT + FW_NORMAL) / 2)
		weight = PANGO_WEIGHT_LIGHT;
	else if (lfp->lfWeight <= (FW_NORMAL + FW_BOLD) / 2)
		weight = PANGO_WEIGHT_NORMAL;
	else if (lfp->lfWeight <= (FW_BOLD + FW_ULTRABOLD) / 2)
		weight = PANGO_WEIGHT_BOLD;
	else if (lfp->lfWeight <= (FW_ULTRABOLD + FW_HEAVY) / 2)
		weight = PANGO_WEIGHT_ULTRABOLD;
	else
		weight = PANGO_WEIGHT_HEAVY;

	/* XXX No idea how to figure out the stretch */
	stretch = PANGO_STRETCH_NORMAL;

	description = pango_font_description_new();
	pango_font_description_set_family(description, family);
	pango_font_description_set_style(description, style);
	pango_font_description_set_weight(description, weight);
	pango_font_description_set_stretch(description, stretch);
	pango_font_description_set_variant(description, variant);

	DEBUG_PRINT_LINE;
	
	return description;
}

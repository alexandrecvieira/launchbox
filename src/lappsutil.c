/**
 *
 * Copyright (c) 2019 Alexandre C Vieira
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#include "lappsutil.h"

int icon_size, font_size, app_label_width, app_label_height, apps_table_width, apps_table_height;
int indicator_font_size, indicator_width, indicator_height;
int s_height, s_width, grid[2], main_vbox_border_width;
double screen_size_relation;
char *recent_label_font_size;
cairo_surface_t* surface;

#define ARRAY_LENGTH(a) (sizeof (a) / sizeof (a)[0])

gboolean blur_background(const char *image_path, const char *bg_image_path)
{
    MagickWand *inWand = NULL;
    MagickBooleanType status;

    MagickWandGenesis();
    inWand = NewMagickWand();
    status = MagickReadImage(inWand, image_path);

    if (status == MagickFalse)
    {
	inWand = DestroyMagickWand(inWand);
	MagickWandTerminus();
	return FALSE;
    }

    MagickBlurImage(inWand, 0, 5);
 
    MagickAdaptiveResizeImage(inWand, s_width, s_height);

    MagickWriteImage(inWand, bg_image_path);

    inWand = DestroyMagickWand(inWand);
 
    MagickWandTerminus();

    return TRUE;
}

GdkPixbuf *ximage_to_pixbuf(XImage *image)
{
    GdkPixbuf *bg_target_pix = NULL;
    MagickWand *inWand = NULL;
       
    PixelIterator *pitr = NULL;
    PixelWand *pwand = NULL;
    PixelWand **wand_pixels = NULL;
    unsigned long pixel;
    int x, y;
    char hex[128];

    int rowstride, row;
    guchar *pixels = NULL;

    if(image == NULL){
	return NULL;
    }
    else
    {
	MagickWandGenesis();
	inWand = NewMagickWand();
	pwand = NewPixelWand();
	PixelSetColor(pwand, "none"); // Set default;
	MagickNewImage(inWand, s_width, s_height, pwand);
	pitr = NewPixelIterator(inWand);

	unsigned long nwands; // May also be size_t

	for (y=0; y < image->height; y++) {
	    wand_pixels=PixelGetNextIteratorRow(pitr,&nwands);
            for ( x=0; x < image->width; x++) {
                pixel = XGetPixel(image,x,y);
                sprintf(hex, "#%02x%02x%02x",
			pixel>>16,           // Red
			(pixel&0x00ff00)>>8, // Green
			pixel&0x0000ff       // Blue
		    );
                PixelSetColor(wand_pixels[x],hex);
            }
            (void) PixelSyncIterator(pitr);
	}

	// MagickBlurImage(inWand, 0, 5);

	// MagickSetImageOpacity(inWand, 0.5);

	bg_target_pix = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, s_width, s_height);

	pixels = gdk_pixbuf_get_pixels(bg_target_pix);
	rowstride = gdk_pixbuf_get_rowstride(bg_target_pix);
  
	for (row = 0; row < s_height; row++)
	{
	    guchar *data = pixels + row * rowstride;
	    MagickExportImagePixels(inWand, 0, row, s_width, 1, "RGBA", CharPixel, data);
	}

	pitr = DestroyPixelIterator(pitr);
	inWand = DestroyMagickWand(inWand);
	pwand = DestroyPixelWand(pwand);
	
	MagickWandTerminus();

	return bg_target_pix;
    }
}

GdkPixbuf *create_app_name(const char *app_name, double font_size)
{
    GdkPixbuf *bg_target_pix = NULL;
    MagickWand *magick_wand = NULL;
    MagickWand *c_wand = NULL;
    DrawingWand *d_wand = NULL;
    PixelWand *p_wand = NULL;
    size_t width, height;
    int rowstride, row;
    guchar *pixels = NULL;
    char *target_name = NULL;

    char **app_name_splited = g_strsplit(app_name, " ", -1);
    char **ptr = NULL;
    int i = 1;
    int lines = 1;
    for (ptr = app_name_splited; *ptr; ptr++)
    {
	if (i == 1)
	    target_name = g_strconcat(*ptr, " ", NULL);
	if (i > 1 && (i % 2) == 0) // i == 2 or i == 4 or ...
	{
	    target_name = g_strconcat(target_name, *ptr, "\n ", NULL);
	    lines++;
	}
	if (i > 1 && (i % 2) > 0) // i == 3 or i == 5 or ...
	    target_name = g_strconcat(target_name, *ptr, " ", NULL);
	i++;
    }

    g_strfreev(app_name_splited);

    MagickWandGenesis();
    magick_wand = NewMagickWand();
    d_wand = NewDrawingWand();
    p_wand = NewPixelWand();
    PixelSetColor(p_wand, "none");

    // Create a new transparent image
    MagickNewImage(magick_wand, 640, 480, p_wand);

    // Set up a 16 point white font
    PixelSetColor(p_wand, "white");
    DrawSetFillColor(d_wand, p_wand);
    DrawSetFont(d_wand, "Sans Serif");

    if(lines > 3)
    {
	DrawSetFontSize(d_wand, font_size - 1);
    }
    else
    {
	DrawSetFontSize(d_wand, font_size);
    }
    
    // Turn antialias on - not sure this makes a difference
    DrawSetTextAntialias(d_wand, MagickTrue);

    // Now draw the text
    DrawAnnotation(d_wand, 0, font_size, target_name);

    g_free(target_name);

    // Draw the image on to the magick_wand
    MagickDrawImage(magick_wand, d_wand);

    // Trim the image down to include only the text
    MagickTrimImage(magick_wand, 0);

    // equivalent to the command line +repage
    MagickResetImagePage(magick_wand, "");

    // Make a copy of the text image
    c_wand = CloneMagickWand(magick_wand);

    // Set the background colour to gray for the shadow
    PixelSetColor(p_wand, "black");
    MagickSetImageBackgroundColor(magick_wand, p_wand);

    // Opacity is a real number indicating (apparently) percentage
    MagickShadowImage(magick_wand, 100, 3, 0, 0);

    // Composite the text on top of the shadow
    MagickCompositeImage(magick_wand, c_wand, OverCompositeOp, 4, 4);

    width = MagickGetImageWidth(magick_wand);
    height = MagickGetImageHeight(magick_wand);

    bg_target_pix = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, width, height);

    pixels = gdk_pixbuf_get_pixels(bg_target_pix);
    rowstride = gdk_pixbuf_get_rowstride(bg_target_pix);
    MagickSetImageDepth(magick_wand, 32);

    for (row = 0; row < height; row++)
    {
	guchar *data = pixels + row * rowstride;
	MagickExportImagePixels(magick_wand, 0, row, width, 1, "RGBA", CharPixel, data);
    }

    /* Clean up */
    magick_wand = DestroyMagickWand(magick_wand);
    c_wand = DestroyMagickWand(c_wand);
    d_wand = DestroyDrawingWand(d_wand);
    p_wand = DestroyPixelWand(p_wand);

    MagickWandTerminus();

    return bg_target_pix;
}

gboolean shadow_icon(const char *path)
{
    MagickWand *src_wand = NULL;
    MagickWand *dst_wand = NULL;
    PixelWand *shadow_color = NULL;
    MagickBooleanType status;
  
    MagickWandGenesis();

    src_wand = NewMagickWand();
    status = MagickReadImage(src_wand, path);

    if (status == MagickFalse)
    {
	src_wand = DestroyMagickWand(src_wand);
	MagickWandTerminus();
	return FALSE;
    }

    shadow_color = NewPixelWand();
   
    dst_wand = CloneMagickWand(src_wand);

    PixelSetColor(shadow_color, "black");
    MagickSetImageBackgroundColor(dst_wand, shadow_color);

    // MagickBooleanType MagickShadowImage(MagickWand *wand,
    // const double alpha, const double sigma,const ssize_t x,const ssize_t y)
    MagickShadowImage(dst_wand, 100, 2, 0, 0);

    MagickCompositeImage(dst_wand, src_wand, OverCompositeOp, 2, 2);

    MagickAdaptiveResizeImage(dst_wand, icon_size, icon_size);

    MagickWriteImage(dst_wand, path);

    src_wand = DestroyMagickWand(src_wand);
    dst_wand = DestroyMagickWand(dst_wand);
    shadow_color = DestroyPixelWand(shadow_color);

    MagickWandTerminus();

    return TRUE;
}

GdkPixbuf *shadow_indicator(GdkPixbuf *src_pix)
{
    GdkPixbuf *bg_target_pix = NULL;
    size_t width, height;
    int rowstride, row;
    guchar *pixels = NULL;
    char *buffer = NULL;
    gsize buffer_size;
    MagickWand *src_wand = NULL;
    MagickWand *dest_wand = NULL;

    MagickWandGenesis();

    gdk_pixbuf_save_to_buffer(src_pix, &buffer, &buffer_size, "png", NULL);
        
    src_wand = NewMagickWand();
    MagickBooleanType read = MagickReadImageBlob(src_wand, buffer, buffer_size);
    dest_wand = CloneMagickWand(src_wand);
    PixelWand *shadow_color = NewPixelWand();
    PixelSetColor(shadow_color, "black");
    MagickWand *shadow = CloneMagickWand(dest_wand);
    MagickSetImageBackgroundColor(dest_wand, shadow_color);
    MagickShadowImage(dest_wand, 100, 2, 0, 0);
    MagickCompositeImage(dest_wand, shadow, OverCompositeOp, 3, 3);
    
    width = MagickGetImageWidth(dest_wand);
    height = MagickGetImageHeight(dest_wand);
    
    bg_target_pix = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, width, height);

    pixels = gdk_pixbuf_get_pixels(bg_target_pix);
    rowstride = gdk_pixbuf_get_rowstride(bg_target_pix);
    MagickSetImageDepth(dest_wand, 32);
    
    for (row = 0; row < height; row++)
    {
	guchar *data = pixels + row * rowstride;
	MagickExportImagePixels(dest_wand, 0, row, width, 1, "RGBA", CharPixel, data);
    }

    shadow = DestroyMagickWand(shadow);
    src_wand = DestroyMagickWand(src_wand);
    dest_wand = DestroyMagickWand(dest_wand);
    shadow_color = DestroyPixelWand(shadow_color);

    MagickWandTerminus();

    return bg_target_pix;
}

void set_icons_fonts_sizes()
{
    GdkScreen *screen = gdk_screen_get_default();
    s_height = gdk_screen_get_height(screen);
    s_width = gdk_screen_get_width(screen);
    screen_size_relation = (pow(s_width * s_height, ((double) (1.0 / 3.0))) / 1.6);

    /*openlog("Launchbox", LOG_PID | LOG_CONS, LOG_USER);
      syslog(LOG_INFO, "Screen relation: w-%d x h-%d -> %f", s_width, s_height, screen_size_relation);
      closelog();*/

    // common screen size resolution = suggested_size (s_width / s_height)
    // 1024x768 = 57.689983 (~1.33) | 1280x800 = 62.996052 (1.6)   | 1280x1024 = 68.399038 (~1.24) | 1366x768  = 63.506375 (~1.77)
    // 1440x900 = 68.142022 (1.6)   | 1600x900 = 70.577702 (~1.77) | 1680x1050 = 75.517258 (1.6)   | 1920x1080 = 79.699393 (~1.77)
    if (screen_size_relation < 68)
    {
	icon_size = 48;
	font_size = 12;
	recent_label_font_size = g_strdup("x-small");
    }
    else if (screen_size_relation >= 68 && screen_size_relation < 79)
    {
	icon_size = 48;
	font_size = 14;
	recent_label_font_size = g_strdup("small");
    }
    else if (screen_size_relation >= 79)
    {
	icon_size = 64;
	font_size = 16;
	recent_label_font_size = g_strdup("medium");
    }

    if (screen_size_relation < 75)
	app_label_height = 58;
    else
	app_label_height = 68;

    if (screen_size_relation < 62)
	app_label_width = 120;
    else if (screen_size_relation >= 62 && screen_size_relation < 79)
	app_label_width = 160;
    else if (screen_size_relation >= 79)
	app_label_width = 250;

    indicator_font_size = font_size * 2;
    indicator_width = font_size * 2;
    indicator_height = (font_size * 2) + (font_size / 2);
    main_vbox_border_width = screen_size_relation / 2;
    apps_table_height = s_height - (screen_size_relation * 2);
    apps_table_width = s_width - screen_size_relation;

    /* grid[0] = rows | grid[1] = columns */
    if (s_width > s_height)
    { // normal landscape orientation
	grid[0] = 4;
	grid[1] = 5;
    }
    else
    { // most likely a portrait orientation
	grid[0] = 5;
	grid[1] = 4;
    }
}

int app_name_comparator(GAppInfo *item1, GAppInfo *item2)
{
    return g_ascii_strcasecmp(g_app_info_get_name(item1), g_app_info_get_name(item2));
}

Pixmap get_root_pixmap(Display* display, Window *root)
{
    Pixmap currentRootPixmap;
    Atom act_type;
    int act_format;
    unsigned long nitems, bytes_after;
    unsigned char *data = NULL;
    Atom _XROOTPMAP_ID;

    _XROOTPMAP_ID = XInternAtom(display, "_XROOTPMAP_ID", False);

    if (XGetWindowProperty(display, *root, _XROOTPMAP_ID, 0, 1, False,
                XA_PIXMAP, &act_type, &act_format, &nitems, &bytes_after,
                &data) == Success) {

        if (data) {
            currentRootPixmap = *((Pixmap *) data);
            XFree(data);
        }
    }

    return currentRootPixmap;
}

void blur_image_surface (int radius)
{
    cairo_surface_t *tmp;
    int width, height;
    int src_stride, dst_stride;
    int x, y, z, w;
    uint8_t *src, *dst;
    uint32_t *s, *d, a, p;
    int i, j, k;
    uint8_t kernel[17];
    const int size = ARRAY_LENGTH (kernel);
    const int half = size / 2;

    if (cairo_surface_status (surface))
	return;

    width = cairo_image_surface_get_width (surface);
    height = cairo_image_surface_get_height (surface);

    switch (cairo_image_surface_get_format (surface)) {
    case CAIRO_FORMAT_A1:
    default:
	/* Don't even think about it! */
	return;

    case CAIRO_FORMAT_A8:
	/* Handle a8 surfaces by effectively unrolling the loops by a
	 * factor of 4 - this is safe since we know that stride has to be a
	 * multiple of uint32_t. */
	width /= 4;
	break;

    case CAIRO_FORMAT_RGB24:
    case CAIRO_FORMAT_ARGB32:
	break;
    }

    tmp = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, width, height);
    if (cairo_surface_status (tmp))
	return;

    src = cairo_image_surface_get_data (surface);
    src_stride = cairo_image_surface_get_stride (surface);

    dst = cairo_image_surface_get_data (tmp);
    dst_stride = cairo_image_surface_get_stride (tmp);

    a = 0;
    for (i = 0; i < size; i++) {
	double f = i - half;
	a += kernel[i] = exp (- f * f / 30.0) * 80;
    }

    /* Horizontally blur from surface -> tmp */
    for (i = 0; i < height; i++) {
	s = (uint32_t *) (src + i * src_stride);
	d = (uint32_t *) (dst + i * dst_stride);
	for (j = 0; j < width; j++) {
	    if (radius < j && j < width - radius) {
		d[j] = s[j];
		continue;
	    }

	    x = y = z = w = 0;
	    for (k = 0; k < size; k++) {
		if (j - half + k < 0 || j - half + k >= width)
		    continue;

		p = s[j - half + k];

		x += ((p >> 24) & 0xff) * kernel[k];
		y += ((p >> 16) & 0xff) * kernel[k];
		z += ((p >>  8) & 0xff) * kernel[k];
		w += ((p >>  0) & 0xff) * kernel[k];
	    }
	    d[j] = (x / a << 24) | (y / a << 16) | (z / a << 8) | w / a;
	}
    }

    /* Then vertically blur from tmp -> surface */
    for (i = 0; i < height; i++) {
	s = (uint32_t *) (dst + i * dst_stride);
	d = (uint32_t *) (src + i * src_stride);
	for (j = 0; j < width; j++) {
	    if (radius <= i && i < height - radius) {
		d[j] = s[j];
		continue;
	    }

	    x = y = z = w = 0;
	    for (k = 0; k < size; k++) {
		if (i - half + k < 0 || i - half + k >= height)
		    continue;

		s = (uint32_t *) (dst + (i - half + k) * dst_stride);
		p = s[j];

		x += ((p >> 24) & 0xff) * kernel[k];
		y += ((p >> 16) & 0xff) * kernel[k];
		z += ((p >>  8) & 0xff) * kernel[k];
		w += ((p >>  0) & 0xff) * kernel[k];
	    }
	    d[j] = (x / a << 24) | (y / a << 16) | (z / a << 8) | w / a;
	}
    }

    cairo_surface_destroy (tmp);
    cairo_surface_mark_dirty (surface);
}

int expose_count = 0;
gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, 
                  gpointer user_data)
{
    cr = gdk_cairo_create(gtk_widget_get_window(widget));
    
    if(expose_count == 0)
	blur_image_surface(s_height);
    
    cairo_set_source_surface( cr, surface, 0, 0 );
    cairo_paint(cr);
    cairo_set_source_rgba (cr, 0, 0, 0, 0.9);
    cairo_fill (cr);
    cairo_paint_with_alpha(cr, 0.2);
    
    cairo_destroy( cr );

    expose_count++;

    return FALSE;
}

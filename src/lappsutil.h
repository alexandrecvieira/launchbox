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

#ifndef LAPPSUTIL_H
#define LAPPSUTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <time.h> 

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <gio/gio.h>
#include <glib.h>
#include <glib-object.h>
#include <glib/gstdio.h>

#include <syslog.h>

#include <wand/MagickWand.h>

extern int icon_size, font_size, app_label_width, app_label_height, apps_table_width, apps_table_height;
extern int indicator_font_size, indicator_width, indicator_height;
extern int s_height, s_width, grid[2], main_vbox_border_width;
extern double screen_size_relation;
extern char *recent_label_font_size;
extern cairo_surface_t* surface;

void set_icons_fonts_sizes();
gboolean blur_background(const char *image_path, const char *bg_image_path);
GdkPixbuf *ximage_to_pixbuf(XImage *image);
GdkPixbuf *create_app_name(const char *app_name, double font_size);
gboolean shadow_icon(const char *path);
GdkPixbuf *shadow_indicator(GdkPixbuf *src_pix);
int app_name_comparator(GAppInfo *item1, GAppInfo *item2);
Pixmap get_root_pixmap(Display* display, Window *root);
gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data);

#endif /* LAPPSUTIL_H */

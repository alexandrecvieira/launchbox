# Sources for launchapps
bin_PROGRAMS = launchbox
launchbox_SOURCES = launchapps.c lappsutil.c

# localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

# Linker options
launchbox_LDFLAGS = $(LTLIBINTL) $(MAGICKWAND_LIBS) $(GLIB2_LIBS) $(GTK2_LIBS) $(GIO2_LIBS) -lX11 -lm

# Compiler options. Here we are adding the include directory
# to be searched for headers included in the source code.
launchbox_CPPFLAGS = -I$(top_srcdir)/include $(GTK2_CFLAGS) $(MAGICKWAND_CFLAGS)

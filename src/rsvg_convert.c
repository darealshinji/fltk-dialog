/*
   Based on librsvg's rsvg-convert.c

   Copyright (C) 2005 Red Hat, Inc.
   Copyright (C) 2005 Dom Lachowicz <cinamod@hotmail.com>
   Copyright (C) 2005 Caleb Moore <c.moore@student.unsw.edu.au>
   Copyright (C) 2017-2018 djcj <djcj@gmx.de>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this program; if not, write to the
   Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Authors: Carl Worth <cworth@cworth.org>, 
            Caleb Moore <c.moore@student.unsw.edu.au>,
            Dom Lachowicz <cinamod@hotmail.com>
*/

/*
 ./configure --disable-shared --disable-introspection --disable-pixbuf-loader --with-pic

 gcc -shared -Wall -Wno-deprecated-declarations -O2 -fPIC -I. -I.. \
   -o ../rsvg_convert.so ../src/rsvg_convert.c \
   -s -Wl,-z,defs -Wl,--as-needed .libs/librsvg-2.a -lm \
   $(pkg-config --static --cflags --libs glib-2.0 gio-2.0 gdk-pixbuf-2.0 \
     cairo pangocairo libxml-2.0 libcroco-0.6)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cairo/cairo.h>
#include <gio/gio.h>
#include <glib-object.h>
#include <glib.h>

#include "rsvg.h"

static void callback (int *width, int *height, gpointer data) {
  RsvgDimensionData *dimensions = data;
  *width = dimensions->width;
  *height = dimensions->height;
}

int rsvg_to_png (const char *input_file, const char *output_file)
{
  GFile *file = NULL;
  GInputStream *stream = NULL;
  GError *error = NULL;
  GFileInfo *file_info = NULL;
  gboolean compressed = FALSE;
  RsvgHandle *rsvg = NULL;
  RsvgDimensionData dimensions;
  cairo_surface_t *surface = NULL;
  cairo_t *cr = NULL;

  g_type_init ();
  rsvg_set_default_dpi (90.0);

  file = g_file_new_for_commandline_arg (input_file);
  stream = (GInputStream *) g_file_read (file, NULL, &error);
  file_info = g_file_query_info (file, G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE, G_FILE_QUERY_INFO_NONE, NULL, NULL);

  if (file_info) {
    const char *content_type;
    char *gz_content_type;
    content_type = g_file_info_get_content_type (file_info);
    gz_content_type = g_content_type_from_mime_type ("application/x-gzip");
    compressed = (content_type && g_content_type_is_a (content_type, gz_content_type));
    g_free (gz_content_type);
    g_object_unref (file_info);
  }

  if (compressed) {
    GZlibDecompressor *decompressor;
    GInputStream *converter_stream;
    decompressor = g_zlib_decompressor_new (G_ZLIB_COMPRESSOR_FORMAT_GZIP);
    converter_stream = g_converter_input_stream_new (stream, G_CONVERTER (decompressor));
    g_object_unref (stream);
    stream = converter_stream;
  }

  if (stream) {
    rsvg = rsvg_handle_new_from_stream_sync (stream, file, RSVG_HANDLE_FLAGS_NONE, NULL, &error);
  }

  g_clear_object (&stream);
  g_clear_object (&file);

  if (error) {
    return 1;
  }

  rsvg_handle_set_size_callback (rsvg, callback, &dimensions, NULL);
  dimensions.width = dimensions.height = 64;
  surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, dimensions.width, dimensions.height);
  cr = cairo_create (surface);

  rsvg_handle_render_cairo (rsvg, cr);
  cairo_surface_write_to_png (surface, output_file);

  g_object_unref (rsvg);
  cairo_destroy (cr);
  cairo_surface_destroy (surface);
  rsvg_cleanup ();

  return 0;
}


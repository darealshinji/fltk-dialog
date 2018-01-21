/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018, djcj <djcj@gmx.de>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <string>
#include <cairo/cairo.h>
#include <gio/gio.h>
#include <glib-object.h>
#include <glib.h>

#include "rsvg.h"

static std::string png_data;

static void callback(int *w, int *h, gpointer data) {
  RsvgDimensionData *d = (RsvgDimensionData *)data;
  *w = d->width;
  *h = d->height;
}

static cairo_status_t write_func(void *v, const unsigned char *data, unsigned int length) {
  (void)v;
  png_data.append((char *)data, length);
  return CAIRO_STATUS_SUCCESS;
}

/* use this after rsvg_to_png_convert() */
extern "C" unsigned char *rsvg_to_png_get_data(void) {
  return png_data.empty() ? NULL : (unsigned char *)png_data.c_str();
}

extern "C" int rsvg_to_png_convert(const char *input_file)
{
  GFile *file;
  GInputStream *stream;
  GError *error = NULL;
  GFileInfo *info;
  bool gzip = false;
  RsvgHandle *handle;
  RsvgDimensionData dim;
  cairo_surface_t *surf;
  cairo_t *cr;

  g_type_init();

  file = g_file_new_for_commandline_arg(input_file);
  stream = (GInputStream *)g_file_read(file, NULL, &error);

  if (error) {
    g_clear_object(&stream);
    g_clear_object(&file);
    return 0;
  }

  info = g_file_query_info(file, G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE, G_FILE_QUERY_INFO_NONE, NULL, NULL);

  if (info) {
    const char *type = g_file_info_get_content_type(info);
    char *mime = g_content_type_from_mime_type("application/x-gzip");
    if (type && g_content_type_is_a(type, mime)) {
      gzip = true;
    }
    g_free(mime);
    g_object_unref(info);
  }

  if (gzip) {
    GZlibDecompressor *decomp = g_zlib_decompressor_new(G_ZLIB_COMPRESSOR_FORMAT_GZIP);
    GInputStream *new_stream = g_converter_input_stream_new(stream, G_CONVERTER(decomp));
    g_object_unref(stream);
    stream = new_stream;
  }

  if (!stream) {
    g_clear_object(&stream);
    g_clear_object(&file);
    return 0;
  }

  rsvg_set_default_dpi(90.0);
  handle = rsvg_handle_new_from_stream_sync(stream, file, RSVG_HANDLE_FLAGS_NONE, NULL, &error);
  g_clear_object(&stream);
  g_clear_object(&file);

  if (error) {
    return 0;
  }

  rsvg_handle_set_size_callback(handle, callback, &dim, NULL);
  dim.width = dim.height = 64;
  surf = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, dim.width, dim.height);
  cr = cairo_create(surf);

  rsvg_handle_render_cairo(handle, cr);
  cairo_surface_write_to_png_stream(surf, write_func, NULL);

  g_object_unref(handle);
  cairo_destroy(cr);
  cairo_surface_destroy(surf);
  rsvg_cleanup();

  return png_data.size();
}


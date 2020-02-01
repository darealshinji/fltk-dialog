/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2020, djcj <djcj@gmx.de>
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

#include <iostream>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
//extern "C" {
//#include <icns.h>
//}

#include "fltk-dialog.hpp"
#include "icns_image.hpp"


static const icns_type_t sourceTypes[]
{
  /* we want 64x64, so check 128x128 and higher first */
  ICNS_128X128_32BIT_DATA,
  ICNS_256x256_32BIT_ARGB_DATA,
  ICNS_512x512_32BIT_ARGB_DATA,
  ICNS_1024x1024_32BIT_ARGB_DATA,
/*
  ICNS_1024x1024_32BIT_ARGB_DATA,
  ICNS_512x512_32BIT_ARGB_DATA,
  ICNS_256x256_32BIT_ARGB_DATA,
  ICNS_128X128_32BIT_DATA,
 */

  ICNS_48x48_32BIT_DATA,
  ICNS_32x32_32BIT_DATA,
  ICNS_16x16_32BIT_DATA,

  ICNS_48x48_8BIT_DATA,
  ICNS_32x32_8BIT_DATA,
  ICNS_16x16_8BIT_DATA,
  ICNS_16x12_8BIT_DATA,

  ICNS_48x48_4BIT_DATA,
  ICNS_32x32_4BIT_DATA,
  ICNS_16x16_4BIT_DATA,
  ICNS_16x12_4BIT_DATA,

  ICNS_48x48_1BIT_DATA,
  ICNS_32x32_1BIT_DATA,
  ICNS_32x32_1BIT_ICON,
  ICNS_16x16_1BIT_DATA,
  ICNS_16x12_1BIT_DATA
};


icns_image::icns_image(const char *filename) : Fl_RGB_Image(0,0,0)
{
  FILE *fp = NULL;
  icns_family_t *iconFamily = NULL;
  icns_image_t iconImage = { 0,0,0,0,0,0 };
  int rv;

  w(0); h(0); d(0); ld(0);
  alloc_array = 0;
  array = NULL;

  /* dlopen() libicns */

  void *handle = dlopen("libicns.so.1", RTLD_LAZY);
  char *error = dlerror();

  if (!handle) {
    std::cerr << error << std::endl;
    ld(ERR_FILE_ACCESS);
    return;
  }

  dlerror();

# define LOAD_SYMBOL(type,func,param) \
  GETPROCADDRESS(handle,type,func,param) \
  error = dlerror(); \
  if (error) { \
    std::cerr << "error: " << error << std::endl; \
    ld(ERR_FILE_ACCESS); \
    dlclose(handle); \
    return; \
  }

  LOAD_SYMBOL( int, icns_read_family_from_file, (FILE*, icns_family_t**) )
  LOAD_SYMBOL( int, icns_get_image32_with_mask_from_family, (icns_family_t*, icns_type_t, icns_image_t*) )

  /* load icon file */

  if ((fp = fopen(filename, "r")) == NULL) {
    ld(ERR_FILE_ACCESS);
    dlclose(handle);
    return;
  }

  rv = icns_read_family_from_file(fp, &iconFamily);
  fclose(fp);

  if (rv != 0) {
    ld(ERR_FORMAT);
    dlclose(handle);
    return;
  }
  rv = 1;

  /* iterate through the source types until we find something */
  for (size_t i = 0; i < sizeof(sourceTypes)/sizeof(*sourceTypes); i++) {
    if ((rv = icns_get_image32_with_mask_from_family(iconFamily, sourceTypes[i], &iconImage)) == 0) {
      break;
    }
  }

/*
  if (type == ANY) {
    for (size_t i = 0; i < sizeof(sourceTypes)/sizeof(*sourceTypes); i++) {
      if ((rv = icns_get_image32_with_mask_from_family(iconFamily, sourceTypes[i], &iconImage)) == 0) {
        break;
      }
    }
  } else {
    icns_type_t check[5] = {
      ICNS_NULL_DATA, ICNS_NULL_DATA,
      ICNS_NULL_DATA, ICNS_NULL_DATA,
      ICNS_NULL_DATA
    };

    switch (type) {
      case ICNS_1024:
        check[0] = ICNS_1024x1024_32BIT_ARGB_DATA;
        break;
      case ICNS_512:
        check[0] = ICNS_512x512_32BIT_ARGB_DATA;
        break;
      case ICNS_256:
        check[0] = ICNS_256x256_32BIT_ARGB_DATA;
        break;
      case ICNS_128:
        check[0] = ICNS_128X128_32BIT_DATA;
        break;

      case ICNS_48:
        check[0] = ICNS_48x48_32BIT_DATA;
        check[1] = ICNS_48x48_8BIT_DATA;
        check[2] = ICNS_48x48_4BIT_DATA;
        check[3] = ICNS_48x48_1BIT_DATA;
        break;
      case ICNS_48_32BIT:
        check[0] = ICNS_48x48_32BIT_DATA;
        break;
      case ICNS_48_8BIT:
        check[0] = ICNS_48x48_8BIT_DATA;
        break;
      case ICNS_48_4BIT:
        check[0] = ICNS_48x48_4BIT_DATA;
        break;
      case ICNS_48_1BIT:
        check[0] = ICNS_48x48_1BIT_DATA;
        break;

      case ICNS_32:
        check[0] = ICNS_32x32_32BIT_DATA;
        check[1] = ICNS_32x32_8BIT_DATA;
        check[2] = ICNS_32x32_4BIT_DATA;
        check[3] = ICNS_32x32_1BIT_DATA;
        check[4] = ICNS_32x32_1BIT_ICON;
        break;
      case ICNS_32_32BIT:
        check[0] = ICNS_32x32_32BIT_DATA;
        break;
      case ICNS_32_8BIT:
        check[0] = ICNS_32x32_8BIT_DATA;
        break;
      case ICNS_32_4BIT:
        check[0] = ICNS_32x32_4BIT_DATA;
        break;
      case ICNS_32_1BIT:
        check[0] = ICNS_32x32_1BIT_DATA;
        check[1] = ICNS_32x32_1BIT_ICON;
        break;

      case ICNS_16:
        check[0] = ICNS_16x16_32BIT_DATA;
        check[1] = ICNS_16x16_8BIT_DATA;
        check[2] = ICNS_16x16_4BIT_DATA;
        check[3] = ICNS_16x16_1BIT_DATA;
        break;
      case ICNS_16_32BIT:
        check[0] = ICNS_16x16_32BIT_DATA;
        break;
      case ICNS_16_8BIT:
        check[0] = ICNS_16x16_8BIT_DATA;
        break;
      case ICNS_16_4BIT:
        check[0] = ICNS_16x16_4BIT_DATA;
        break;
      case ICNS_16_1BIT:
        check[0] = ICNS_16x16_1BIT_DATA;
        break;

      case ICNS_16x12:
        check[0] = ICNS_16x12_8BIT_DATA;
        check[1] = ICNS_16x12_4BIT_DATA;
        check[2] = ICNS_16x12_1BIT_DATA;
        break;
      case ICNS_16x12_8BIT:
        check[0] = ICNS_16x12_8BIT_DATA;
        break;
      case ICNS_16x12_4BIT:
        check[0] = ICNS_16x12_4BIT_DATA;
        break;
      case ICNS_16x12_1BIT:
        check[0] = ICNS_16x12_1BIT_DATA;
        break;

      default:
        ld(ERR_FORMAT);
        free(iconFamily);
        dlclose(handle);
        return;
    }

    for (size_t i = 0; i < sizeof(check)/sizeof(*check); i++) {
      if (check[i] == ICNS_NULL_DATA) {
        break;
      }
      if ((rv = icns_get_image32_with_mask_from_family(iconFamily, check[i], &iconImage)) == 0) {
        break;
      }
    }
  }
  */

  dlclose(handle);

  if (rv != 0) {
    ld(ERR_FORMAT);
    free(iconFamily);
    return;
  }

  w(iconImage.imageWidth);
  h(iconImage.imageHeight);
  d(iconImage.imageChannels);

  /* take over pointer to image data (no need to call icns_free_image()) */
  array = iconImage.imageData;
  alloc_array = 1;
  iconImage.imageData = NULL;

  free(iconFamily);
}


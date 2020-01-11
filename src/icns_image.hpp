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

#include <FL/Fl.H>
#include <FL/Fl_RGB_Image.H>

#include <stdint.h>

#define ICNS_1024x1024_32BIT_ARGB_DATA 0x69633130
#define ICNS_512x512_32BIT_ARGB_DATA 0x69633039
#define ICNS_256x256_32BIT_ARGB_DATA 0x69633038
#define ICNS_128X128_32BIT_DATA 0x69743332
#define ICNS_48x48_1BIT_DATA 0x69636823
#define ICNS_48x48_4BIT_DATA 0x69636834
#define ICNS_48x48_8BIT_DATA 0x69636838
#define ICNS_48x48_32BIT_DATA 0x69683332
#define ICNS_32x32_1BIT_DATA 0x49434E23
#define ICNS_32x32_4BIT_DATA 0x69636C34
#define ICNS_32x32_8BIT_DATA 0x69636C38
#define ICNS_32x32_32BIT_DATA 0x696C3332
#define ICNS_16x16_1BIT_DATA 0x69637323
#define ICNS_16x16_4BIT_DATA 0x69637334
#define ICNS_16x16_8BIT_DATA 0x69637338
#define ICNS_16x16_32BIT_DATA 0x69733332
#define ICNS_16x12_1BIT_DATA 0x69636D23
#define ICNS_16x12_4BIT_DATA 0x69636D34
#define ICNS_16x12_8BIT_DATA 0x69636D38
#define ICNS_32x32_1BIT_ICON 0x49434F4E
//#define ICNS_NULL_DATA 0x00000000

typedef uint8_t icns_uint8_t;
typedef uint16_t icns_uint16_t;
typedef uint32_t icns_uint32_t;
typedef uint64_t icns_uint64_t;
typedef uint8_t icns_byte_t;
typedef uint32_t icns_type_t;
typedef int32_t icns_size_t;

typedef struct icns_element_t {
  icns_type_t elementType;
  icns_size_t elementSize;
  icns_byte_t elementData[1];
} icns_element_t;

typedef struct icns_family_t {
  icns_type_t resourceType;
  icns_size_t resourceSize;
  icns_element_t elements[1];
} icns_family_t;

typedef struct icns_image_t {
  icns_uint32_t imageWidth;
  icns_uint32_t imageHeight;
  icns_uint8_t imageChannels;
  icns_uint16_t imagePixelDepth;
  icns_uint64_t imageDataSize;
  icns_byte_t *imageData;
} icns_image_t;


class icns_image : public Fl_RGB_Image
{
public:
/*
  enum {
    ANY, ICNS_1024, ICNS_512, ICNS_256, ICNS_128,
    ICNS_48, ICNS_48_1BIT, ICNS_48_4BIT, ICNS_48_8BIT, ICNS_48_32BIT,
    ICNS_32, ICNS_32_1BIT, ICNS_32_4BIT, ICNS_32_8BIT, ICNS_32_32BIT,
    ICNS_16, ICNS_16_1BIT, ICNS_16_4BIT, ICNS_16_8BIT, ICNS_16_32BIT,
    ICNS_16x12, ICNS_16x12_1BIT, ICNS_16x12_4BIT, ICNS_16x12_8BIT
  };
  */

  /* Loads an Apple icon file (.icns) */
  icns_image(const char *filename);
  //icns_image(const char *filename, int type = ANY);
};


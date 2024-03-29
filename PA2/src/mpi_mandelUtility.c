/******************************************************************************
* FILE: mpi_mandelUtility.c
* DESCRIPTION:
*   Implementations of utility functions for mandelbrot
* AUTHOR: Fred Harris (original code)
*         Evan Su (modified for c usage)
* LAST REVISED: 03/13/17
******************************************************************************/
//libraries and complier directives
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi_mandelUtility.h"
#ifdef _WIN32
  #define WRITE_FLAGS "wb"
#else
  #define WRITE_FLAGS "w"
#endif


//this function calculates the mandelbrot for a single pixel.
int calc_Pixel(struct complex c)
{
    int count, max_iter;
    struct complex z;
    float temp, lengthsq = 0;
    max_iter = 256;
    z.real = 0;
    z.imag = 0;
    count = 0;

    do
    {
        temp = z.real * z.real - z.imag * z.imag + c.real;
        z.imag = 2 * z.real * z.imag + c.imag;
        z.real = temp;
        lengthsq = z.real *z.real + z.imag * z.imag;
        count++;
    }while((lengthsq < 4.00)&& (count < max_iter));

    return count;

}
//writes the image to black and white 1d array
int pim_write_black_and_white(const char * const fileName,
                               const int width,
                               const int height,
                               const unsigned char * pixels)
{
  
  const int true = 1;
  const int false = 0;

  FILE * fp = fopen(fileName, WRITE_FLAGS);

  if (!fp) return false;
  fprintf(fp, "P5\n%i %i 255\n", width, height);
  fwrite(pixels, width * height, 1, fp);
  fclose(fp);

  return true;
}

//writes the image to black and white 2d array
int pim_write_black_and_white1(const char * const fileName,
                               const int width,
                               const int height,
                               const unsigned char ** pixels)
{
  int i;
  int ret;
  unsigned char * t = (unsigned char*) malloc(width * height * sizeof(char));
  
  for (i = 0; i < height; ++i) memcpy(t + width * i, pixels[i], width);
  ret = pim_write_black_and_white(fileName, width, height, t);
  free(t);
  return ret;
}

//writes the image to color 1d array
int pim_write_color(const char * const fileName,
                     const int width,
                     const int height,
                     const unsigned char * pixels)
{
  const int true = 1;
  const int false = 0;
  FILE * fp = fopen(fileName, WRITE_FLAGS);

  if (!fp) return false;
  fprintf(fp, "P6\n%i %i 255\n", width, height);
  fwrite(pixels, width * height * 3, 1, fp);
  fclose(fp);

  return true;
}
//writes the image to color 2d array
int pim_write_color1(const char * const fileName,
                     const int width,
                     const int height,
                     const unsigned char  ** pixels)
{
  int i;
  int ret;
  unsigned char * t = (unsigned char*) malloc(width * height * sizeof(char) * 3);;

  for (i = 0; i < height; ++i) memcpy(t + width * i * 3, pixels[i], width * 3);
  ret = pim_write_color(fileName, width, height, t);
  free(t);
  return ret;
  
}
//writes the image to color 1d array using rgb
int pim_write_color2(const char * const fileName,
                     const int width,
                     const int height,
                     const unsigned char * red,
                     const unsigned char * green,
                     const unsigned char * blue)
{
  int i, j;
  int ret;
  unsigned char * p, * t = (unsigned char*) malloc(width * height * sizeof(char)* 3);

  p = t;
  for (i = 0; i < height; ++i)
  {
    for (j = 0; j < width; ++j)
    {
      *(p++) = *(red++);
      *(p++) = *(green++);
      *(p++) = *(blue++);
    }
  }
  ret = pim_write_color(fileName, width, height, t);
  free(t);
  return ret;
}
//writes the image to color 2d array using rgb
int pim_write_color3(const char * const fileName,
                     const int width,
                     const int height,
                     const unsigned char ** red,
                     const unsigned char ** green,
                     const unsigned char ** blue)
{
  int i, j;
  int ret;
  const unsigned char * r, * g, * b;
  unsigned char * p, * t = (unsigned char*) malloc(width * height * sizeof(char)* 3);;

  p = t;
  for (i = 0; i < height; ++i)
  {
    r = red[i];
    g = green[i];
    b = blue[i];
    for (j = 0; j < width; ++j)
    {
      *(p++) = *(r++);
      *(p++) = *(g++);
      *(p++) = *(b++);
    }
  }
  ret = pim_write_color(fileName, width, height, t);
  free(t);
  return ret;
}

//writes the image to color 2d array using rgb
//Red values are ignored.
int pim_write_color4(const char * const fileName,
                     const int width,
                     const int height,
                     const unsigned char ** red,
                     const unsigned char ** green,
                     const unsigned char ** blue)
{
  int i, j;
  int ret;
  const unsigned char * g, * b;
  unsigned char * p, * t = (unsigned char*) malloc(width * height * sizeof(char)* 3);;

  p = t;
  for (i = 0; i < height; ++i)
  {
    g = green[i];
    b = blue[i];
    for (j = 0; j < width; ++j)
    {
      *(p++) = 0;
      *(p++) = (*(g++) * 5) % 256;
      *(p++) = (*(b++) * 5) % 256;
    }
  }
  ret = pim_write_color(fileName, width, height, t);
  free(t);
  return ret;
}

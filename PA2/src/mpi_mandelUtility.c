#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
  #define WRITE_FLAGS "wb"
#else
  #define WRITE_FLAGS "w"
#endif

struct complex
{
    float real;
    float imag;
};

int calc_Pixel(struct complex c)
{
    int count, max_iter;
    struct complex z;
    float temp, lengthsq;
    max_iter = 256;
    z.real = 0;
    z.imag = 0;
    count = 0;

    do
    {
        temp = z.real * z.real - z.imag * z.imag + c.real;
        z.imag = 2 * z.real * z.real + c.imag;
        z.real = temp;
        count++;
    }while((lengthsq < 4.00)&& (count < max_iter));

    return count;

}

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
int pim_write_color1(const char * const fileName,
                     const int width,
                     const int height,
                     const unsigned char * const * pixels)
{
  int i;
  int ret;
  unsigned char * t = (unsigned char*) malloc(width * height * sizeof(char) * 3);;

  for (i = 0; i < height; ++i) memcpy(t + width * i * 3, pixels[i], width * 3);
  ret = pim_write_color(fileName, width, height, t);
  free(t);
  return ret;
  
}
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

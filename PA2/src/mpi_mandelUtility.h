#ifndef __PIMFUNCS_H__
#define __PIMFUNCS_H__
typedef struct complex;

int calc_Pixel(struct complex);



int pim_write_black_and_white(const char * const fileName,
                               const int width,
                               const int height,
                               const unsigned char * pixels);
int pim_write_black_and_white1(const char * const fileName,
                               const int width,
                               const int height,
                               const unsigned char ** pixels);
int pim_write_color(const char * const fileName,
                     const int width,
                     const int height,
                     const unsigned char * pixels);
int pim_write_color1(const char * const fileName,
                     const int width,
                     const int height,
                     const unsigned char ** pixels);
int pim_write_color2(const char * const fileName,
                     const int width,
                     const int height,
                     const unsigned char * red,
                     const unsigned char * green,
                     const unsigned char * blue);
int pim_write_color3(const char * const fileName,
                     const int width,
                     const int height,
                     const unsigned char ** red,
                     const unsigned char ** green,
                     const unsigned char ** blue);

#endif

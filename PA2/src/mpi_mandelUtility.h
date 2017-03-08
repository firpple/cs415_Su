#ifndef __PIMFUNCS_H__
#define __PIMFUNCS_H__

struct complex
{
    float real;
    float imag;
};

int calc_Pixel(struct complex);



bool pim_write_black_and_white(const char * const fileName,
                               const int width,
                               const int height,
                               const unsigned char * pixels);
bool pim_write_black_and_white(const char * const fileName,
                               const int width,
                               const int height,
                               const unsigned char ** pixels);
bool pim_write_color(const char * const fileName,
                     const int width,
                     const int height,
                     const unsigned char * pixels);
bool pim_write_color(const char * const fileName,
                     const int width,
                     const int height,
                     const unsigned char ** pixels);
bool pim_write_color(const char * const fileName,
                     const int width,
                     const int height,
                     const unsigned char * red,
                     const unsigned char * green,
                     const unsigned char * blue);
bool pim_write_color(const char * const fileName,
                     const int width,
                     const int height,
                     const unsigned char ** red,
                     const unsigned char ** green,
                     const unsigned char ** blue);

#endif

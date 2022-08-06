#ifndef IMAGE_READER
    #define IMAGE_READER
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdbool.h>
    #include "color.h"

    #define buildInt(array,index)  (array[index+3]<<24) + (array[index+2]<<16) + (array[index+1]<<8) +(array[index])
    #define buildShort(array,index)  (array[index+1]<<8) +(array[index])
    #define int2byte(array,index,number) array[index] = (number & 255); array[index + 1] = ((number >> 8)& 255); array[index + 2] = ((number >> 16)& 255); array[index + 3] = ((number >> 24)& 255); 
    #define short2byte(array,index,number) array[index] = number & 255; array[index + 1] = (number >> 8)& 255;
    #define getImageIndex(i, j,k, width, height) (i*3 + j*(width*3) + (2-k))
    #define getDataIndex(i, j,k, width, height, padding) (i*3 + (height - j - 1)*(width*3 + padding) + (2-k))
    
    #ifndef LOGGIN_ENABLED
        #define LOGGIN_ENABLED false
    #endif
    #define LOG(s,...)   if(LOGGIN_ENABLED){ printf(s, __VA_ARGS__);}

    typedef struct image image;
    struct image{
        size_t width;
        size_t height;
        unsigned char* data;
    };

    bool write_bmp(char const * const  name, image image);
    image new_image(size_t width, size_t height);
    void free_image(image screen);
    void put_pixel(image image,size_t y, size_t x, pixel_color color);
    pixel_color get_pixel(image image,size_t y, size_t x);
#endif

#ifndef _COLOR_
    #define _COLOR_
    #include <tgmath.h>

    #define divide_color(color, k)  scale_color(color, (1.0/(double)k))
    #define new_color_RGB(r,g,b) (color_RGB) {.red = r, .green = g, .blue = b}
    #define COLOR_ERROR 0.0001

    typedef struct pixel_color pixel_color;
    struct pixel_color
    {
        unsigned char red;
        unsigned char green;
        unsigned char blue;
    };

    typedef struct color_RGB color_RGB;
    struct color_RGB
    {
        double red;
        double green;
        double blue;  
    };

    color_RGB add_color(color_RGB a, color_RGB b);
    color_RGB mix_color(color_RGB a, color_RGB b);
    color_RGB normalize_color(color_RGB a);
    color_RGB scale_color(color_RGB a, double k);
    pixel_color to_pixel_color(color_RGB a);
    

    
#endif
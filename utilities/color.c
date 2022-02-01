#include "color.h"

color_RGB add_color(color_RGB a, color_RGB b){
    color_RGB c = {};
    c.red = a.red + b.red;
    c.green = a.green + b.green;
    c.blue = a.blue + b.blue;
    return c;
}

color_RGB mix_color(color_RGB a, color_RGB b){
    color_RGB c = {};
    c.red = a.red * b.red;
    c.red = isnan(c.red) ? COLOR_ERROR : c.red;
    c.green = a.green * b.green;
    c.green = isnan(c.green ) ? COLOR_ERROR : c.green;
    c.blue = a.blue * b.blue;
    c.blue = isnan(c.blue) ? COLOR_ERROR : c.blue;
    return c;
}

color_RGB normalize_color(color_RGB a){
    double norm = sqrt((a.red*a.red) + (a.blue*a.blue) + (a.green*a.green));
    color_RGB c = {};
    c.red = a.red/norm;
    c.green = a.green/norm;
    c.blue = a.blue/norm;
    return c;
}

color_RGB scale_color(color_RGB a, double k){
    color_RGB c = {};
    c.red = a.red * k;
    c.red = isnan(c.red) ? COLOR_ERROR : c.red;
    c.green = a.green * k;
    c.green = isnan(c.green ) ? COLOR_ERROR : c.green;
    c.blue = a.blue * k;
    c.blue = isnan(c.blue) ? COLOR_ERROR : c.blue;
    
    return c;
}


pixel_color to_pixel_color(color_RGB normal){
    double red = normal.red > (1.0 - COLOR_ERROR) ? 1.0 : normal.red;
    double green = normal.green > (1.0 - COLOR_ERROR)? 1.0 : normal.green;
    double blue = normal.blue > (1.0 - COLOR_ERROR)? 1.0 : normal.blue;

    // Gamma correction
    int r = (int) (0.5 + pow(red,1.0/2.2)*255.0);
    int g = (int) (0.5 + pow(green,1.0/2.2)*255.0);
    int b = (int) (0.5 + pow(blue,1.0/2.2)*255.0);
    
    pixel_color pixel = {
        .red = (unsigned char) r,
        .blue = (unsigned char) b,
        .green = (unsigned char) g
    };
    return pixel;
}
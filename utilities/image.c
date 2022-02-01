#include "image.h"

/*
* Writes a very simple bmp file
*/
bool write_bmp(char const * const name, image image){
    size_t width = image.width;
    size_t height = image.height;
    unsigned char* screen = image.data;
    size_t padding = (width%4) == 0 ?  0 : 4 - (width%4);
    size_t dataSize = height*(width*3 + padding);
    unsigned char* data = calloc(dataSize, sizeof(char));    
    for(size_t i = 0; i < width; i++){
        for(size_t j = 0; j < height; j++){
            for(size_t k=0; k < 3; k++){
                data[getDataIndex(i,j,k,width,height,padding)] = screen[getImageIndex(i,j,k, width, height)];
            }
        }
    }

    size_t offset = 54;
    size_t fileSize = dataSize + offset; //using default header size
    FILE* file = fopen(name, "wb+");
    unsigned char* header = calloc(14, sizeof(unsigned char));
    header[0] = 'B';
    header[1] = 'M';
    int2byte(header, 2, fileSize);
    int2byte(header, 10, offset);

    unsigned char* headerInfo = calloc(40, sizeof(unsigned char)); 

    int2byte(headerInfo, 0, 40);
    int2byte(headerInfo, 4, width);
    int2byte(headerInfo, 8, height);
    short2byte(headerInfo, 12, 1);
    short2byte(headerInfo, 14, 24);
    int2byte(headerInfo, 16, 0);
    int2byte(headerInfo, 20, dataSize);
    int2byte(headerInfo, 24, 0);
    int2byte(headerInfo, 28, 0);
    int2byte(headerInfo, 32, 0);
    int2byte(headerInfo, 36, 0);

    fwrite(header, sizeof(unsigned char), 14, file);
    fflush(file);
    fwrite(headerInfo, sizeof(unsigned char), 40 , file);
    fflush(file); 
    fwrite(data, sizeof(unsigned char), dataSize, file);

    fflush(file);
    fclose(file);
    free(data);
    free(header);
    free(headerInfo);
    return true;
} 

image new_image(size_t width, size_t height){
    unsigned char* data = calloc(width*height*3, sizeof(unsigned char));
    image screen = {
        width,
        height,
        data
    };
    return screen;
}

void free_image(image screen){
    free(screen.data);
}

void put_pixel(image image,size_t y, size_t x, pixel_color color){
    image.data[getImageIndex(y, x, 0, image.width, image.height)] = color.red;
    image.data[getImageIndex(y, x, 1, image.width, image.height)] = color.green;
    image.data[getImageIndex(y, x, 2, image.width, image.height)] = color.blue;
}

pixel_color get_pixel(image image,size_t y, size_t x){
    pixel_color pixel = {
        .red = image.data[getImageIndex(y, x, 0, image.width, image.height)],
        .green = image.data[getImageIndex(y, x, 1, image.width, image.height)],
        .blue = image.data[getImageIndex(y, x, 2, image.width, image.height)]
    };
    return pixel;
}
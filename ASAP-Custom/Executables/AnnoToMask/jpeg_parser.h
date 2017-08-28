#ifndef JPEG_PARSET_H_
#define JPEG_PARSET_H_

#include <string>
#include "libjpeg/jpeglib.h"

class JpegParser
{
public:
    static void write_to_jpeg(std::string& file, char* img_buf , int width , int height)
    {
        FILE* outfile = fopen(file.c_str(), "wb");

        struct jpeg_compress_struct cinfo;
        struct jpeg_error_mgr       jerr;

        cinfo.err = jpeg_std_error(&jerr);
        jpeg_create_compress(&cinfo);
        jpeg_stdio_dest(&cinfo, outfile);

        cinfo.image_width = width;
        cinfo.image_height = height;
        cinfo.input_components = 3;
        cinfo.in_color_space = JCS_RGB;

        jpeg_set_defaults(&cinfo);
        /*set the quality [0..100]  */
        jpeg_set_quality(&cinfo, 75, true);

        jpeg_start_compress(&cinfo, true);

        JSAMPROW row_pointer;          /* pointer to a single row */

        while (cinfo.next_scanline < cinfo.image_height) {
            row_pointer = (JSAMPROW)&img_buf[cinfo.next_scanline*3*width];
            jpeg_write_scanlines(&cinfo, &row_pointer, 1);
        }

        jpeg_finish_compress(&cinfo);
        jpeg_destroy_compress(&cinfo);
        fclose(outfile);
    }

protected:
private:
};


#endif

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "libtiff/tiffio.h"
#include "libjpeg/jpeglib.h"

#include <stdio.h>

#include <iostream>
#include <string>
#include <fstream>

#include <time.h>

// ��Щ�ʹ������йأ����ù�
struct my_error_mgr {
    struct jpeg_error_mgr pub;  /* "public" fields */

    jmp_buf setjmp_buffer;  /* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

void my_error_exit(j_common_ptr cinfo)
{
    my_error_ptr myerr = (my_error_ptr)cinfo->err;
    (*cinfo->err->output_message) (cinfo);
    longjmp(myerr->setjmp_buffer, 1);
}


int JPEGLoader(int argc, char* argv[])
{
    char* filename = "E:/data/Pathology/1.jpg";
    struct jpeg_decompress_struct cinfo;
    struct my_error_mgr jerr;
    FILE * infile;
    JSAMPARRAY buffer;
    int row_stride;

    if ((infile = fopen(filename, "rb")) == NULL) {
        fprintf(stderr, "can't open %s\n", filename);
        return 0;
    }
    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;
    if (setjmp(jerr.setjmp_buffer)) {
        jpeg_destroy_decompress(&cinfo);
        fclose(infile);
        return 0;
    }
    jpeg_create_decompress(&cinfo);

    jpeg_stdio_src(&cinfo, infile);

    // ���������ȡ�˶�ȡͼƬ����Ϣ������ͼƬ�ĸߺͿ�
    (void)jpeg_read_header(&cinfo, TRUE);


    // ������������Լ��Ĵ��룬��ȡ���û���ͼ����Ϣ

    unsigned char* img_buf = new unsigned char[cinfo.image_width*cinfo.image_height * 3];

    (void)jpeg_start_decompress(&cinfo);

    row_stride = cinfo.output_width * cinfo.output_components;
    buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);


    int curHeight = 0;
    while (cinfo.output_scanline < cinfo.output_height) {
        // һ��һ�еĶ�ȡ
        (void)jpeg_read_scanlines(&cinfo, buffer, 1);

        // ��������Ӵ����ȡ��ͼƬ����������
        // buffer�����˶�ȡ�ĵ�ǰ�е����ݣ�����˳����RGB
        // output_scanline���Ѿ���ȡ��������

        memcpy(img_buf + curHeight * 3 * cinfo.image_width, buffer[0], 3 * cinfo.image_width);
        /*for (int i = 0; i < cinfo.image_width; ++i) {
        img_buf[i * 3 + 2] = buffer[0][i * 3 + 0];
        img_buf[i * 3 + 1] = buffer[0][i * 3 + 1];
        img_buf[i * 3 + 0] = buffer[0][i * 3 + 2];
        }*/

        curHeight++;
    }

    //std::ofstream out(std::string(filename) + std::string(".rgb"), std::ios::out | std::ios::binary);
    //if (out.is_open()) {
    //    out.write((char*)img_buf, cinfo.image_width*cinfo.image_height * 3);
    //    out.close();
    //}

    delete[] img_buf;

    (void)jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    return 1;

    return 0;
}
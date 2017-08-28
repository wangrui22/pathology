#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "libtiff/tiffio.h"
#include "libjpeg/jpeglib.h"

#include <stdio.h>

#include <iostream>
#include <string>
#include <fstream>
#include <memory>

#include <time.h>

#include "io/multiresolutionimageinterface/MultiResolutionImageWriter.h"
#include "core/PathologyEnums.h"


//////////////////////////////////////////////////////////////////////////
//jpeg
// 这些和错误处理有关，不用管
struct my_error_mgr2 {
    struct jpeg_error_mgr pub;  /* "public" fields */

    jmp_buf setjmp_buffer;  /* for return to caller */
};

typedef struct my_error_mgr2 * my_error_ptr2;

void my_error_exit2(j_common_ptr cinfo)
{
    my_error_ptr2 myerr = (my_error_ptr2)cinfo->err;
    (*cinfo->err->output_message) (cinfo);
    longjmp(myerr->setjmp_buffer, 1);
}

//////////////////////////////////////////////////////////////////////////



typedef struct IMAGE_INFO_STRUCT
{
    LPARAM DataFilePTR;
}ImageInfoStruct;

typedef bool (WINAPI *InitImageFileFunc)(ImageInfoStruct& sImageInfo,
    const char* Path);
InitImageFileFunc my_InitImageFileFunc;

typedef bool (WINAPI *UnInitImageFileFunc)(ImageInfoStruct& sImageInfo);
UnInitImageFileFunc my_UnInitImageFileFunc;

typedef char* (WINAPI *GetImageStreamFunc)(ImageInfoStruct& sImageInfo,
    float fScale, int nImagePosX, int nImagePosY, int& nDataLength,
    unsigned char** ImageStream);
GetImageStreamFunc my_GetImageStreamFunc;

typedef bool (WINAPI *DeleteImageDataFunc)(LPVOID pImageData);
DeleteImageDataFunc my_DeleteImageDataFunc;

typedef bool (WINAPI *GetThumnailImagePathFunc)(const char* szFilePath,
    unsigned char** ImageData, int& nDataLength, int& nThumWidth, int& nThumHeght);
GetThumnailImagePathFunc my_GetThumnailImagePathFunc;

typedef bool (WINAPI *GetPriviewInfoPathFunc)(const char* szFilePath, unsigned char** ImageData,
    int& nDataLength, int& nPriviewWidth, int& nPriviewHeight);
GetPriviewInfoPathFunc my_GetPriviewInfoPathFunc;

typedef bool (WINAPI *GetLableInfoPathFunc)(const char* szFilePath, unsigned char** ImageData,
    int& nDataLength, int& nLabelWidth, int& nLabelHeight);
GetLableInfoPathFunc my_GetLableInfoPathFunc;

typedef bool (WINAPI *GetHeaderInfoFunc)(ImageInfoStruct sImageInfo, int&khiImageHeight,
    int &khiImageWidth, int	&khiScanScale, float &khiSpendTime, double &khiScanTime,
    float &khiImageCapRes, int& khiImageBlockSize);
GetHeaderInfoFunc my_GetHeaderInfoFunc;

typedef bool (WINAPI *GetImageDataRoiFunc)(ImageInfoStruct sImageInfo, float fScale,
    int sp_x, int sp_y, int nWidth, int nHeight, BYTE** pBuffer, int &DataLength, bool flag);
GetImageDataRoiFunc my_GetImageDataRoiFunc;


int KFBToTIFF(int argc, char* argv[])
{

    HINSTANCE hdll;
#ifdef WIN64
    hdll = LoadLibrary((LPCSTR)("D:/program/MyCode/Pathology_custom_format/Test/dll/x64/ImageOperationLib.dll"));
#else
    hdll = LoadLibrary((LPCSTR)("D:/program/MyCode/Pathology_custom_format/Test/dll/x86/ImageOperationLib.dll"));
#endif // WIN64
    if (hdll == NULL)
    {
        printf("Error to LoadLibrary!\n");
        return 0;
    }
    my_InitImageFileFunc = (InitImageFileFunc)GetProcAddress(hdll, "InitImageFileFunc");
    my_GetHeaderInfoFunc = (GetHeaderInfoFunc)GetProcAddress(hdll, "GetHeaderInfoFunc");
    my_GetImageStreamFunc = (GetImageStreamFunc)GetProcAddress(hdll, "GetImageStreamFunc");
    my_DeleteImageDataFunc = (DeleteImageDataFunc)GetProcAddress(hdll, "DeleteImageDataFunc");
    my_UnInitImageFileFunc = (UnInitImageFileFunc)GetProcAddress(hdll, "UnInitImageFileFunc");
    my_GetThumnailImagePathFunc = (GetThumnailImagePathFunc)GetProcAddress(hdll, "GetThumnailImagePathFunc");
    my_GetPriviewInfoPathFunc = (GetPriviewInfoPathFunc)GetProcAddress(hdll, "GetPriviewInfoPathFunc");
    my_GetLableInfoPathFunc = (GetLableInfoPathFunc)GetProcAddress(hdll, "GetLableInfoPathFunc");

    const char* filename = "D:/program/MyCode/Pathology_custom_format/Test/data/kfbio_test_2.kfb"; //kfbio_test_1.kfb,  kfbio_test_2.kfb
    //const char* filename = "D:/program/MyCode/Pathology_custom_format/Test/data/test.kfb"; //kfbio_test_1.kfb,  kfbio_test_2.kfb
    ImageInfoStruct sImageInfo;

    int     kImageHeight = 0;
    int     kImageWidth = 0;
    int     kScanScale = 0;
    float   kSpendTime = 0.0f;
    double  kScanTime = 0.0;
    float   kImageCapRes = 0.0f;
    int kImageBlockSize = 0;

    int kDataLength = 0;
    unsigned char* pImageDataPtr = nullptr;

    int iThumImageLength = 0;   int iThumImageWidth = 0;     int iThumImageHeigth = 0;
    unsigned char* pThumImageDataPtr = nullptr;

    int iPreviewImageLength = 0;    int iPreviewImageWidth = 0;     int iPreviewImageHeigth = 0;
    unsigned char* pPreviewImageDataPtr = nullptr;

    int iLabelImageLength = 0;      int iLabelImageWidth = 0;     int iLabelImageHeigth = 0;
    unsigned char* pLabelImageDataPter = nullptr;
    FILE *fp = nullptr;
    std::string temp_filename;
    char buffer[20];
    if (my_InitImageFileFunc == NULL)
    {
        return -1;
    }

    my_InitImageFileFunc(sImageInfo, filename);
    my_GetHeaderInfoFunc(sImageInfo, kImageHeight, kImageWidth, kScanScale,
        kSpendTime, kScanTime, kImageCapRes, kImageBlockSize);
    my_GetThumnailImagePathFunc(filename, &pThumImageDataPtr, iThumImageLength, iThumImageWidth, iThumImageHeigth);
    fp = fopen("thumbnail.jpg", "wb");
    fwrite((void*)(pThumImageDataPtr), 1, iThumImageLength, fp);
    fclose(fp); fp = nullptr;
    my_GetPriviewInfoPathFunc(filename, &pPreviewImageDataPtr, iPreviewImageLength, iPreviewImageWidth, iPreviewImageHeigth);
    fp = fopen("preview.jpg", "wb");
    fwrite((void*)(pPreviewImageDataPtr), 1, iPreviewImageLength, fp);
    fclose(fp); fp = nullptr;
    my_GetLableInfoPathFunc(filename, &pLabelImageDataPter, iLabelImageLength, iLabelImageWidth, iLabelImageHeigth);
    fp = fopen("labelinfo.jpg", "wb");
    fwrite((void*)(pLabelImageDataPter), 1, iLabelImageLength, fp);
    fclose(fp); fp = nullptr;
    my_GetImageStreamFunc(sImageInfo, kScanScale, kImageWidth / 2, kImageHeight / 2, kDataLength, &pImageDataPtr);

    float current_scale = (float)kScanScale;
    bool empty_img = false;
    int curlevel = 0;
    int tile_count = 0;

    //////////////////////////////////////////////////////////////////////////
    //Multi-res-writer 
    //MultiResolutionImageWriter multi_res_writer;
    //multi_res_writer.openFile("E:/data/Pathology/kfb_multi_res.tiff");
    ////multi_res_writer.openFile("E:/data/Pathology/test.tiff");
    //multi_res_writer.setColorType(pathology::RGB);
    //multi_res_writer.setCompression(pathology::JPEG);
    //multi_res_writer.setTileSize(256);
    //multi_res_writer.setDataType(pathology::DataType::UChar);
    //multi_res_writer.setInterpolation(pathology::Linear);
    ////multi_res_writer.setOverrideSpacing();
    //multi_res_writer.writeImageInformation(kImageWidth, kImageHeight);
    //////////////////////////////////////////////////////////////////////////


    TIFF *image = TIFFOpen("E:/data/Pathology/kfb_test_multi_res.tiff", "w");

    std::cout << "create directory : " << TIFFCreateDirectory(image) << std::endl;

    //TIFFSetField(image, TIFFTAG_TILEWIDTH, 256);
    //TIFFSetField(image, TIFFTAG_TILELENGTH, 256);
    //TIFFSetField(image, TIFFTAG_IMAGEWIDTH, kImageWidth);
    //TIFFSetField(image, TIFFTAG_IMAGELENGTH, kImageHeight);
    //TIFFSetField(image, TIFFTAG_COMPRESSION, COMPRESSION_JPEG);
    //TIFFSetField(image, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
    //TIFFSetField(image, TIFFTAG_BITSPERSAMPLE, 8);
    //TIFFSetField(image, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_UINT);
    //TIFFSetField(image, TIFFTAG_SAMPLESPERPIXEL, 3);
    //TIFFSetField(image, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    //TIFFSetField(image, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
    //TIFFSetField(image, TIFFTAG_XRESOLUTION, 10.0);
    //TIFFSetField(image, TIFFTAG_YRESOLUTION, 10.0);
    //TIFFSetField(image, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);

    
    //////////////////////////////////////////////////////////////////////////
    //Jpeg decoder
    struct jpeg_decompress_struct cinfo;
    struct my_error_mgr2 jerr;
    int row_stride;
    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit2;

    //////////////////////////////////////////////////////////////////////////

    long long tile_sum = 0;
    long long tile_error = 0;

    while (!empty_img)
    {

        TIFFSetDirectory(image, curlevel);

        TIFFSetField(image, TIFFTAG_TILEWIDTH, 256);
        TIFFSetField(image, TIFFTAG_TILELENGTH, 256);
        TIFFSetField(image, TIFFTAG_IMAGEWIDTH, kImageWidth);
        TIFFSetField(image, TIFFTAG_IMAGELENGTH, kImageHeight);

        TIFFSetField(image, TIFFTAG_COMPRESSION, COMPRESSION_JPEG);
        TIFFSetField(image, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_YCBCR);
        TIFFSetField(image, TIFFTAG_YCBCRSUBSAMPLING, 2, 1);

        TIFFSetField(image, TIFFTAG_BITSPERSAMPLE, 8);
        TIFFSetField(image, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_UINT);
        TIFFSetField(image, TIFFTAG_SAMPLESPERPIXEL, 3);
        TIFFSetField(image, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
        TIFFSetField(image, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);

        //TIFFSetField(image, TIFFTAG_XRESOLUTION, 10.0);
        //TIFFSetField(image, TIFFTAG_YRESOLUTION, 10.0);
        //TIFFSetField(image, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);

        if (curlevel != 0 )
        {
            TIFFSetField(image, TIFFTAG_SUBFILETYPE, FILETYPE_REDUCEDIMAGE);
        }

        std::cout << "\ncurrent level : " << curlevel << std::endl;
        std::cout << "current scale : " << current_scale << std::endl;

        tile_count = 0;

        /*if (curlevel == 9)
        {
            std::cout << "EE\n";
        }
        if (curlevel == 8)
        {
            std::cout << "EE\n";
        }
        if (curlevel == 7)
        {
            std::cout << "EE\n";
        }
        if (curlevel == 6)
        {
            std::cout << "EE\n";
        }*/

        std::cout << "image width : " << kImageWidth << std::endl;
        std::cout << "image height : " << kImageHeight << std::endl;

        for (int width = 0; width < kImageWidth; width += 256)
        {
            for (int height = 0; height < kImageHeight; height += 256)
            {
                my_GetImageStreamFunc(sImageInfo, current_scale, width, height, kDataLength, &pImageDataPtr);

                if (nullptr == pImageDataPtr)
                {
                    empty_img = true;
                    goto EMPTY_IMG_BUF;
                }

                tile_sum++;

                //temp_filename = "E:/data/Pathology/temp/tempres_";
                //itoa(current_scale, buffer, 10);    temp_filename += std::string(buffer) + "_";
                //itoa(width, buffer, 10);        temp_filename += std::string(buffer) + "_";
                //itoa(height, buffer, 10);         temp_filename += std::string(buffer) + ".jpg";
                //fp = fopen(temp_filename.c_str(), "wb");
                //fwrite((void*)(pImageDataPtr), 1, kDataLength, fp);
                //fclose(fp); fp = nullptr;


                //////////////////////////////////////////////////////////////////////////
                //jpeg decode
                std::unique_ptr<unsigned char[]> img_data(new unsigned char[kDataLength]);
                memcpy(img_data.get(), pImageDataPtr, kDataLength);
                jpeg_create_decompress(&cinfo);
                jpeg_mem_src(&cinfo, img_data.get(), kDataLength);
                (void)jpeg_read_header(&cinfo, TRUE);
                //if (cinfo.image_width != 256 || cinfo.image_height != 256)
                //{
                //    tile_error++;
                //    //std::cout << "Error\n";
                //
                //    //temp_filename = "E:/data/Pathology/temp/tempres_";
                //    //itoa(current_scale, buffer, 10);    temp_filename += std::string(buffer) + "_";
                //    //itoa(width, buffer, 10);        temp_filename += std::string(buffer) + "_";
                //    //itoa(height, buffer, 10);         temp_filename += std::string(buffer) + ".jpg";
                //    //fp = fopen(temp_filename.c_str(), "wb");
                //    //fwrite((void*)(pImageDataPtr), 1, kDataLength, fp);
                //    //fclose(fp); fp = nullptr;
                //}

                //row_stride = cinfo.output_width * cinfo.output_components;
                //unsigned char* img_buf = new unsigned char[cinfo.image_width*cinfo.image_height * 3];

                //JSAMPARRAY raw_buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);

                //int curHeight = 0;
                //while (cinfo.output_scanline < cinfo.output_height) {
                //    // 一行一行的读取
                //    (void)jpeg_read_scanlines(&cinfo, raw_buffer, 1);
                //    memcpy(img_buf + curHeight * 3 * cinfo.image_width, raw_buffer[0], 3 * cinfo.image_width);

                //    curHeight++;
                //}

                ////std::ofstream out(std::string(temp_filename) + std::string(".raw"), std::ios::out | std::ios::binary);
                ////if (out.is_open()) {
                ////    out.write((char*)img_buf, cinfo.image_width*cinfo.image_height * 3);
                ////    out.close();
                ////}

                //(void)jpeg_finish_decompress(&cinfo);
                //jpeg_destroy_decompress(&cinfo);

                //////////////////////////////////////////////////////////////////////////

                

                int tileid = TIFFComputeTile(image, width, height, 0, 0);
                //std::cout << "tile id : " << tileid << std::endl;
                TIFFWriteRawTile(image, tileid, pImageDataPtr, kDataLength);


                //delete[] pImageDataPtr;
                ++tile_count;
            }
        }

    EMPTY_IMG_BUF:
        std::cout << "write directory : " << TIFFWriteDirectory(image) << std::endl;
        std::cout << "flush : " << TIFFFlush(image) << std::endl;
        /*std::cout << "current done  " << tile_count << std::endl;*/

        current_scale *= 0.5f;
        ++curlevel;
        kImageWidth = int((kImageWidth)*0.5);
        kImageHeight = int((kImageHeight)*0.5);
        if (kImageWidth <128 && kImageHeight < 128)
        {
            break;
        }
        if (tile_count < 2)
        {
            break;
        }
    }


    std::cout << "tile sum : " << tile_sum << std::endl;
    std::cout << "tile error : " << tile_error << std::endl;

    TIFFClose(image);
    //TIFFClose(multi_res_writer._tiff);
    //multi_res_writer.finishImage();


    /*delete pImageDataPtr;
    delete pThumImageDataPtr;
    delete pPreviewImageDataPtr;
    delete pLabelImageDataPter;*/


    FreeLibrary(hdll);

    return 0;
}
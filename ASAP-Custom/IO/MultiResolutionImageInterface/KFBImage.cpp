#ifdef WIN32

#include "KFBImage.h"

#include <stdio.h>
#include <stdlib.h>
#include <fstream>

#include "boost/thread.hpp"
#include "libjpeg/jpeglib.h"

using namespace pathology;


KFBImage::KFBImage()
{
    _imageHeight = 0;
    _imageWidth = 0;
    _scanScale = 0;
    _spendTime = 0.0f;
    _scanTime = 0.0;
    _imageCapRes = 0.0f;
    _imageBlockSize = 0;
}

KFBImage::~KFBImage()
{
    boost::unique_lock<boost::shared_mutex> l(*_openCloseMutex);
    cleanup();
    MultiResolutionImage::cleanup();
}

bool KFBImage::initializeType(const std::string& imagePath)
{
    boost::unique_lock<boost::shared_mutex> l(*_openCloseMutex);

    //initialize KFB
    KFBParser::instance()->init_image_file(_sImageInfo, imagePath.c_str());
    //get KFB header  //TODO check header valid
    int originScanScale = 0;
    KFBParser::instance()->get_header_info(_sImageInfo, _imageHeight, _imageWidth, originScanScale,
        _spendTime, _scanTime, _imageCapRes, _imageBlockSize);
    _scanScale = static_cast<float>(originScanScale);

    //initialize base class parameter
    _dataType = UChar;
    _samplesPerPixel = 3;
    _colorType = RGB;
    _fileType = "KFBImage";
    _spacing.clear();
    _spacing.resize(2);
    _spacing[0] = _imageCapRes;
    _spacing[1] = _imageCapRes;


    //Calculate number of levels
    int curLevel = 0;
    int curX = _imageWidth;
    int curY = _imageHeight;

    _levelDimensions.clear();
    std::vector<unsigned long long> curDims(2);
    curDims[0]= curX;
    curDims[1]= curY;
    _levelDimensions.push_back(curDims);

    float curScale = _scanScale;
    _levelScales.clear();
    _levelScales.push_back(curScale);

    _levelDownsample.clear();
    _levelDownsample.push_back(1.0);

    bool endTag = (curX < 256 && curY < 256);
    while (!endTag)
    {
        ++curLevel;

        curScale *= 0.5f;
        curX = static_cast<int>(curX*0.5f + 0.5);
        curY = static_cast<int>(curY*0.5f + 0.5);

        if (curX < 256 && curY < 256)
        {
            endTag = true;
        }

        curDims[0] = curX;
        curDims[1] = curY;
        _levelDimensions.push_back(curDims);

        _levelScales.push_back(curScale);

        _levelDownsample.push_back(_levelDownsample[_levelDownsample.size()-1]*2.0);

    } 

    _numberOfLevels = curLevel+1;

    _isValid = true;

    return _isValid;
}

double KFBImage::getMinValue(int channel /*= -1*/)
{
    return 0;
}

double KFBImage::getMaxValue(int channel /*= -1*/)
{
    return 255;
}

void KFBImage::cleanup()
{
    KFBParser::instance()->finalize_image_file(_sImageInfo);
    MultiResolutionImage::cleanup();
}

struct my_error_mgr
{
    struct jpeg_error_mgr pub;  /* "public" fields */
    jmp_buf setjmp_buffer;  /* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

void my_error_exit2(j_common_ptr cinfo)
{
    my_error_ptr myerr = (my_error_ptr)cinfo->err;
    (*cinfo->err->output_message) (cinfo);
    longjmp(myerr->setjmp_buffer, 1);
}

void* KFBImage::readDataFromImage(const long long& startX, const long long& startY, 
    const unsigned long long& width, const unsigned long long& height, const unsigned int& level)
{
    boost::shared_lock<boost::shared_mutex> l(*_openCloseMutex);

    unsigned char* img_buf = new unsigned char[width*height * 3];
    memset(img_buf, 252, width*height * 3);

    if (level > _levelDimensions.size() - 1)
    {
        return img_buf;
    }

    const float scale = _levelScales[level];

    unsigned char* buffer= NULL;
    int len = 0;

    int x = int(startX / (_levelDownsample[level]));
    int y = int(startY / (_levelDownsample[level]));

    int imgWidth = _levelDimensions[level][0];
    int imgHeight = _levelDimensions[level][1];

    if (x >= imgWidth-1 || y >= imgHeight-1)
    {
        return img_buf;
    }

    int realWidth = width;
    int realHeight = height;
    if (imgWidth - (x + (int)width)  < 0)
    {
        realWidth = imgWidth - x;
        //return img_buf;
    }
    if (imgHeight - (y + (int)height) < 0)
    {
        realHeight = imgHeight - y;
        //return img_buf;
    }

    if ((realWidth<= 0 || realHeight<= 0) || 
        !KFBParser::instance()->get_image_roi_stream(_sImageInfo, scale, x, y, realWidth, realHeight, len, &buffer))
    {
        std::cout << "get buffer failed!\n";
        return img_buf;
    }

    jpeg_decompress_struct cinfo;
    my_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit2;

    jpeg_create_decompress(&cinfo);
    jpeg_mem_src(&cinfo, buffer, len);
    jpeg_read_header(&cinfo, TRUE);

    jpeg_start_decompress(&cinfo);

    if (cinfo.output_components != 3)
    {
        return img_buf;
    }

    int row_stride = cinfo.output_width * 3;

    JSAMPARRAY raw_buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);

    int curHeight = 0;
    while (cinfo.output_scanline < cinfo.output_height)
    {
        // 一行一行的读取
        jpeg_read_scanlines(&cinfo, raw_buffer, 1);
        memcpy(img_buf + curHeight * 3 * width, raw_buffer[0], 3 * cinfo.image_width);

        curHeight++;
    }

    //////////////////////////////////////////////////////////////////////////
    //For testing
    //std::ofstream out("D:/roi.raw", std::ios::out | std::ios::binary);
    //if (out.is_open()) 
    //{
    //    out.write((char*)img_buf, cinfo.image_width*cinfo.image_height * 3);
    //    out.close();
    //}
    //////////////////////////////////////////////////////////////////////////

    return img_buf;

}


#endif
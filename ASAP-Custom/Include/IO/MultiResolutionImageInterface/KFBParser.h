#ifndef INNER_KFB_PARSER_H_
#define INNER_KFB_PARSER_H_

#include "boost/thread/mutex.hpp"

#ifdef  WIN32

#include <Windows.h>

typedef struct IMAGE_INFO_STRUCT
{
    LPARAM DataFilePTR;
}ImageInfoStruct;

//typedef bool (WINAPI *GetThumnailImagePathFunc)(const char* szFilePath,
//    unsigned char** ImageData, int& nDataLength, int& nThumWidth, int& nThumHeght);
//GetThumnailImagePathFunc _GetThumnailImagePathFunc;
//
//typedef bool (WINAPI *GetPriviewInfoPathFunc)(const char* szFilePath, unsigned char** ImageData,
//    int& nDataLength, int& nPriviewWidth, int& nPriviewHeight);
//GetPriviewInfoPathFunc _GetPriviewInfoPathFunc;
//
//typedef bool (WINAPI *GetLableInfoPathFunc)(const char* szFilePath, unsigned char** ImageData,
//    int& nDataLength, int& nLabelWidth, int& nLabelHeight);
//GetLableInfoPathFunc _GetLableInfoPathFunc;


class KFBParser
{
    typedef bool (WINAPI *InitImageFileFunc)(ImageInfoStruct& sImageInfo,
        const char* Path);
    InitImageFileFunc _InitImageFileFunc;

    typedef bool (WINAPI *UnInitImageFileFunc)(ImageInfoStruct& sImageInfo);
    UnInitImageFileFunc _UnInitImageFileFunc;

    typedef bool (WINAPI *DeleteImageDataFunc)(LPVOID pImageData);
    DeleteImageDataFunc _DeleteImageDataFunc;

    typedef bool (WINAPI *GetHeaderInfoFunc)(ImageInfoStruct sImageInfo, int&khiImageHeight,
        int &khiImageWidth, int	&khiScanScale, float &khiSpendTime, double &khiScanTime,
        float &khiImageCapRes, int& khiImageBlockSize);
    GetHeaderInfoFunc _GetHeaderInfoFunc;

    typedef char* (WINAPI *GetImageStreamFunc)(ImageInfoStruct& sImageInfo,
        float fScale, int nImagePosX, int nImagePosY, int& nDataLength,
        unsigned char** ImageStream);
    GetImageStreamFunc _GetImageStreamFunc;

    typedef bool (WINAPI *GetImageDataRoiFunc)(ImageInfoStruct sImageInfo, float fScale,
        int sp_x, int sp_y, int nWidth, int nHeight, BYTE** pBuffer, int &DataLength, bool flag);
    GetImageDataRoiFunc _GetImageDataRoiFunc;

public:
    static KFBParser* instance()
    {
        if (nullptr == _instance){
            boost::unique_lock<boost::mutex> locker(_mutex);
            if (nullptr == _instance){
                _instance = new KFBParser();
            }
        }

        return _instance;
    }

    ~KFBParser()
    {
        if (_hdll){
            FreeLibrary(_hdll);
        }
    }

    bool init_image_file(ImageInfoStruct& image_info , const char* path)
    {
        if (_InitImageFileFunc){
            return _InitImageFileFunc(image_info, path);
        }
        else{
            return false;
        }
    }

    bool finalize_image_file(ImageInfoStruct& image_info)
    {
        if (_UnInitImageFileFunc) {
            return _UnInitImageFileFunc(image_info);
        }
        else {
            return false;
        }
    }

    bool get_header_info(ImageInfoStruct image_info, int& image_height,
        int &image_width, int	&scan_scale, float &spend_time, double &scan_time,
        float &capture_resolution, int& image_tile_size)
    {
        if (_GetHeaderInfoFunc) {
            return _GetHeaderInfoFunc(image_info , image_height , image_width , scan_scale ,
                spend_time , scan_time , capture_resolution , image_tile_size);
        }
        else {
            return false;
        }
    }

    bool delete_image_data(void* img_data)
    {
        if (_DeleteImageDataFunc) {
            return _DeleteImageDataFunc(img_data);
        }
        else {
            return false;
        }
    }

    bool get_image_stream(ImageInfoStruct& image_info,
        float scale, int start_pos_x, int start_pos_y, int& data_length,
        unsigned char** img_jpeg_stream)
    {
        if (_GetImageStreamFunc) {
            _GetImageStreamFunc(image_info , scale , start_pos_x, start_pos_y, data_length , img_jpeg_stream);
            return data_length != 0 && img_jpeg_stream != nullptr;
        }
        else {
            return false;
        }
    }

    bool get_image_roi_stream(ImageInfoStruct image_info, float scale,
        int start_pos_x, int start_pos_y, int width, int height, int &data_length, unsigned char** img_jpeg_stream)
    {
        if (_GetImageStreamFunc) {
            return _GetImageDataRoiFunc(image_info, scale, start_pos_x, start_pos_y, 
                width, height, img_jpeg_stream, data_length, true);
        }
        else {
            return false;
        }
    }


protected:
    KFBParser()
    {
        _hdll = LoadLibrary((LPCSTR)("./formats/ImageOperationLib.dll"));

        _InitImageFileFunc = (InitImageFileFunc)GetProcAddress(_hdll, "InitImageFileFunc");
        _UnInitImageFileFunc = (UnInitImageFileFunc)GetProcAddress(_hdll, "UnInitImageFileFunc");
        _GetHeaderInfoFunc = (GetHeaderInfoFunc)GetProcAddress(_hdll, "GetHeaderInfoFunc");
        _DeleteImageDataFunc = (DeleteImageDataFunc)GetProcAddress(_hdll, "DeleteImageDataFunc");
        _GetImageStreamFunc = (GetImageStreamFunc)GetProcAddress(_hdll, "GetImageStreamFunc");
        _GetImageDataRoiFunc = (GetImageDataRoiFunc)GetProcAddress(_hdll, "GetImageDataRoiFunc");

        /*_GetThumnailImagePathFunc = (GetThumnailImagePathFunc)GetProcAddress(_hdll, "GetThumnailImagePathFunc");
        _GetPriviewInfoPathFunc = (GetPriviewInfoPathFunc)GetProcAddress(_hdll, "GetPriviewInfoPathFunc");
        _GetLableInfoPathFunc = (GetLableInfoPathFunc)GetProcAddress(_hdll, "GetLableInfoPathFunc");*/
    }
private:
    static KFBParser* _instance;
    static boost::mutex _mutex;

    HINSTANCE _hdll;
};

#endif //  WIN32

#endif // !INNER_KFB_PARSER_H_

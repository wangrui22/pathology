#ifdef WIN32

#ifndef _KFB_IMAGE_H_
#define _KFB_IMAGE_H_

#include "MultiResolutionImage.h"
#include "multiresolutionimageinterface_export.h"
#include "KFBParser.h"

class MULTIRESOLUTIONIMAGEINTERFACE_EXPORT KFBImage : public MultiResolutionImage
{
public:
    KFBImage();
    ~KFBImage();

    virtual bool initializeType(const std::string& imagePath);

    virtual double getMinValue(int channel = -1);
    virtual double getMaxValue(int channel = -1);

protected:
    virtual void cleanup();

    virtual void* readDataFromImage(const long long& startX, const long long& startY, const unsigned long long& width,
        const unsigned long long& height, const unsigned int& level);

private:
    ImageInfoStruct _sImageInfo;

    int     _imageHeight;
    int     _imageWidth;
    float     _scanScale;
    float   _spendTime;
    double  _scanTime;
    float   _imageCapRes;
    int _imageBlockSize;

    std::vector<float> _levelScales;
    std::vector<float> _levelDownsample;
};

#endif 

#endif // WIN32

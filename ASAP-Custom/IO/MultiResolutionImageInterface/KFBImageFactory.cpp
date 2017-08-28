#include "KFBImageFactory.h"
#include "KFBImage.h"
#include "tiffio.h"

const KFBImageFactory KFBImageFactory::registerThis;

KFBImageFactory::KFBImageFactory() : MultiResolutionImageFactory("kfb") {
}

MultiResolutionImage* KFBImageFactory::readImage(const std::string& fileName) const {
    KFBImage* img = new KFBImage();
    img->initialize(fileName);
    if (img->valid()) {
        return img;
    }
    else {
        delete img;
        return NULL;
    }
}

bool KFBImageFactory::canReadImage(const std::string& fileName) const {
    KFBImage* img = new KFBImage();
    bool canOpen = img->initialize(fileName);
    delete img;
    return canOpen;
}
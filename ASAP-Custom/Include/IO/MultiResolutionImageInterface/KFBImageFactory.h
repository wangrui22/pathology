#ifndef _KFBMAGEFACTORY
#define _KFBMAGEFACTORY

#include "multiresolutionimageinterface_export.h"
#include "MultiResolutionImageFactory.h"

class MULTIRESOLUTIONIMAGEINTERFACE_EXPORT KFBImageFactory : public MultiResolutionImageFactory {
public:
    KFBImageFactory();

private:
    static const KFBImageFactory registerThis;
    MultiResolutionImage* readImage(const std::string& fileName) const;
    bool canReadImage(const std::string& fileName) const;
};

#endif
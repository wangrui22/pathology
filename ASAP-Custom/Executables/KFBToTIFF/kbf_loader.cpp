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

#include "io/multiresolutionimageinterface/KFBImage.h"
#include "core/PathologyEnums.h"

int KFBLoader(int argc, char* argv[])
{
    KFBImage* img = new KFBImage();
    img->initializeType("D:/program/MyCode/Pathology_custom_format/Test/data/test.kfb");

    unsigned char* img_buffer= nullptr;
    img->getRawRegion<unsigned char>(26 , 29, 256, 256, 1, img_buffer);
    
    return 0;
}
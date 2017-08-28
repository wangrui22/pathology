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

#include "io/multiresolutionimageinterface/MultiResolutionImageWriter.h"
#include "core/PathologyEnums.h"

#define GET_FIELD_OR_FAIL(tiff, tag, type, result)			\
  do {									\
    type tmp;								\
    if (!TIFFGetField(tiff, tag, &tmp)) {				\
      printf("Cannot get required TIFF tag: %d", tag);		\
      return false;							\
    }									\
    result = tmp;							\
  } while (0)


struct _openslide_tiff_level {
    tdir_t dir;
    int64_t image_w;
    int64_t image_h;
    int64_t tile_w;
    int64_t tile_h;
    int64_t tiles_across;
    int64_t tiles_down;

    bool tile_read_direct;
    uint16_t photometric;
};

int TIFFLoader(int argc, char* argv[])
{
    //TIFF *tiff = TIFFOpen("E:/data/Pathology/CMU-1.tiff", "r");
    TIFF *tiff = TIFFOpen("E:/data/Pathology/kfb_multi_res.tiff", "r");
    int value = 0;
    double dvalue = 0.0;
    long lvalue;
    TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &value);
    std::cout << "width : " << value << std::endl;

    TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &value);
    std::cout << "height : " << value << std::endl;

    TIFFGetField(tiff, TIFFTAG_IMAGEDEPTH, &value);
    std::cout << "depth : " << value << std::endl;

    TIFFGetField(tiff, TIFFTAG_BITSPERSAMPLE, &value);
    std::cout << "bits per sample : " << value << std::endl;

    TIFFGetField(tiff, TIFFTAG_SAMPLESPERPIXEL, &value);
    std::cout << "samples per pixel : " << value << std::endl;

    TIFFGetField(tiff, TIFFTAG_COMPRESSION, &value);
    std::cout << "compress  : " << value << std::endl;

    TIFFGetField(tiff, TIFFTAG_PHOTOMETRIC, &value);
    std::cout << "photometric  : " << value << std::endl;

    TIFFGetField(tiff, TIFFTAG_YCBCRSUBSAMPLING, &value);
    std::cout << "ycbcr subsampling  : " << value << std::endl;

    TIFFGetField(tiff, TIFFTAG_FILLORDER, &value);
    std::cout << "fillorder  : " << value << std::endl;

    TIFFGetField(tiff, TIFFTAG_PLANARCONFIG, &value);
    std::cout << "planarconfig  : " << value << std::endl;

    TIFFGetField(tiff, TIFFTAG_XRESOLUTION, &dvalue);
    std::cout << "x res  : " << dvalue << std::endl;

    TIFFGetField(tiff, TIFFTAG_YRESOLUTION, &dvalue);
    std::cout << "y res : " << dvalue << std::endl;

    TIFFGetField(tiff, TIFFTAG_RESOLUTIONUNIT, &dvalue);
    std::cout << "res unit : " << dvalue << std::endl;

    uint16 count;
    char data[512];
    TIFFGetField(tiff, TIFFTAG_SOFTWARE, &count, data);
    std::cout << "software : " << count << std::endl;

    TIFFGetField(tiff, TIFFTAG_TILEWIDTH, &value);
    std::cout << "tile width : " << value << std::endl;

    TIFFGetField(tiff, TIFFTAG_TILELENGTH, &value);
    std::cout << "tile height  : " << value << std::endl;

    TIFFGetField(tiff, TIFFTAG_TILEDEPTH, &value);
    std::cout << "tile depth  : " << value << std::endl;

    TIFFGetField(tiff, TIFFTAG_TILEOFFSETS, &lvalue);
    std::cout << "tile offset  : " << lvalue << std::endl;

    TIFFGetField(tiff, TIFFTAG_TILEBYTECOUNTS, &value);
    std::cout << "byte counts for tiles  : " << value << std::endl;

    std::cout << "Done\n";

    std::cout << "directions : " << TIFFNumberOfDirectories(tiff) << std::endl;

    int idir = 0;

    do 
    {
        

        if (!TIFFIsTiled(tiff)) {
            continue;
        }

        // confirm it is either the first image, or reduced-resolution
        int cur_dir = TIFFCurrentDirectory(tiff);
        std::cout << "cur dir : " << cur_dir << std::endl;

        if (cur_dir != 0) {
            uint32_t subfiletype;
            if (!TIFFGetField(tiff, TIFFTAG_SUBFILETYPE, &subfiletype)) {
                continue;
            }

            std::cout << "sub file type : " << subfiletype << std::endl;

            if (!(subfiletype & FILETYPE_REDUCEDIMAGE)) {
                continue;
            }
        }
        else
        {
            uint32_t subfiletype;
            TIFFGetField(tiff, TIFFTAG_SUBFILETYPE, &subfiletype);
            std::cout << "sub file type : " << subfiletype << std::endl;
        }
        uint16_t compression;
        if (!TIFFGetField(tiff, TIFFTAG_COMPRESSION, &compression)) {
            continue;
        };
        if (!TIFFIsCODECConfigured(compression)) {
            continue;
        }


        // set the directory
        if (idir == TIFFCurrentDirectory(tiff)) {
            // avoid libtiff unnecessarily rereading directory contents
            
        }
        else if(!TIFFSetDirectory(tiff, idir)) {
            printf("Cannot set TIFF directory %d", idir);
            break;
        }

        // figure out tile size
        int64_t tw, th;
        GET_FIELD_OR_FAIL(tiff, TIFFTAG_TILEWIDTH, uint32_t, tw);
        GET_FIELD_OR_FAIL(tiff, TIFFTAG_TILELENGTH, uint32_t, th);

        std::cout << "tile width : " << tw << std::endl;
        std::cout << "tile height : " << th << std::endl;

        // get image size
        int64_t iw, ih;
        GET_FIELD_OR_FAIL(tiff, TIFFTAG_IMAGEWIDTH, uint32_t, iw);
        GET_FIELD_OR_FAIL(tiff, TIFFTAG_IMAGELENGTH, uint32_t, ih);

        std::cout << "image width : " << iw << std::endl;
        std::cout << "image height : " << ih << std::endl;

        // decide whether we can bypass libtiff when reading tiles
        uint16_t  planar_config, photometric;
        uint16_t bits_per_sample, samples_per_pixel;
        GET_FIELD_OR_FAIL(tiff, TIFFTAG_COMPRESSION, uint16_t, compression);
        GET_FIELD_OR_FAIL(tiff, TIFFTAG_PLANARCONFIG, uint16_t, planar_config);
        GET_FIELD_OR_FAIL(tiff, TIFFTAG_PHOTOMETRIC, uint16_t, photometric);
        GET_FIELD_OR_FAIL(tiff, TIFFTAG_BITSPERSAMPLE, uint16_t, bits_per_sample);
        GET_FIELD_OR_FAIL(tiff, TIFFTAG_SAMPLESPERPIXEL, uint16_t, samples_per_pixel);
        bool read_direct =
            compression == COMPRESSION_JPEG &&
            planar_config == PLANARCONFIG_CONTIG &&
            (photometric == PHOTOMETRIC_RGB || photometric == PHOTOMETRIC_YCBCR) &&
            bits_per_sample == 8 &&
            samples_per_pixel == 3;

        idir++;
    }
    while (TIFFReadDirectory(tiff));

    std::cout << "tiff is tiled : " << TIFFIsTiled(tiff) << std::endl;

    std::cout << "Read directiory : " << idir << std::endl;


    /*tsize_t size = 256 * 256 * 3;
    std::cout << TIFFTileSize(image) << std::endl;;
    tdata_t buf = new char[TIFFTileSize(image)];
    TIFFReadRawTile(image, 1, buf, TIFFTileSize(image));
    std::ofstream out("test_ex.jpg", std::ios::out | std::ios::binary);
    if (out.is_open()) {
        out.write((char*)buf, TIFFTileSize(image));
        out.close();
    }*/

    TIFFClose(tiff);


    return 0;
}
#IO MultiResolutionImageInterface makefile

#version (default debug)
ver = debug

#compiler
CC = g++ -Wall -std=c++11

#file direction
MODULE_NAME = multiresolutionimageinterface
ROOT = ../..
INCLUDE_ROOT_DIR = $(ROOT)/include
INCLUDE_DIR = $(INCLUDE_ROOT_DIR)/io
BIN_DIR = $(ROOT)/bin

VPATH = $(INCLUDE_ROOT_DIR) : $(INCLUDE_DIR) : $(INCLUDE_DIR)/$(MODULE_NAME)
#vpath %.h $(INCLUDE_DIR):$(INCLUDE_DIR)/$(MODULE_NAME)

#debug/release cc
ifeq ($(ver) , debug)
CFLAGS += -c -g -fPIC
else
CFLAGS += -c -fPIC
endif

#macro
# DEFS = OPENSLIDEFILEFORMAT_STATIC_DEFINE 
# CFLAGS += -D $(DEFS)

#target and obj
TARGET = $(BIN_DIR)/lib$(MODULE_NAME).so
OBJS = JPEG2000Codec.o TileCache.o AperioSVSWriter.o \
MultiResolutionImage.o MultiResolutionImageFactory.o MultiResolutionImageReader.o MultiResolutionImageWriter.o \
LIFImage.o LIFImageFactory.o \
TIFFImage.o TIFFImageFactory.o \
VSIImage.o VSIImageFactory.o

ADDITINAL_INCLUDE_DIR = -I$(INCLUDE_ROOT_DIR) -I$(INCLUDE_DIR) -I$(INCLUDE_DIR)/$(MODULE_NAME)
ADDITIANL_DEPENDENCIES = -L$(BIN_DIR) -lcore -lijg8 -ltiff -ljasper -ljpeg -lz -lpugixml -lboost_system -lboost_filesystem \
-lboost_regex -lboost_thread -lboost_chrono -lboost_date_time -lboost_atomic -lmbedcrypto -lmbedtls -lmbedx509 -ldl

EXPORT_HEADER = multiresolutionimageinterface_export.h

$(TARGET): $(OBJS) $(EXPORT_HEADER)
	$(CC) -shared -o $(TARGET) $(OBJS) $(ADDITIANL_DEPENDENCIES)

MultiResolutionImageReader.o : $(EXPORT_HEADER) MultiResolutionImageReader.cpp MultiResolutionImageReader.h MultiResolutionImageFactory.o
	$(CC) $(CFLAGS) MultiResolutionImageReader.cpp  $(ADDITINAL_INCLUDE_DIR)

MultiResolutionImageFactory.o : $(EXPORT_HEADER) MultiResolutionImageFactory.cpp MultiResolutionImageFactory.h VSIImage.o MultiResolutionImage.o 
	$(CC) $(CFLAGS) MultiResolutionImageFactory.cpp $(ADDITINAL_INCLUDE_DIR)

MultiResolutionImageWriter.o : $(EXPORT_HEADER) MultiResolutionImageWriter.cpp MultiResolutionImageWriter.h MultiResolutionImage.o JPEG2000Codec.o
	$(CC) $(CFLAGS) MultiResolutionImageWriter.cpp $(ADDITINAL_INCLUDE_DIR)	

VSIImageFactory.o : $(EXPORT_HEADER) VSIImageFactory.cpp VSIImageFactory.h VSIImage.o 
	$(CC) $(CFLAGS) VSIImageFactory.cpp $(ADDITINAL_INCLUDE_DIR)

VSIImage.o : $(EXPORT_HEADER) VSIImage.cpp VSIImage.h MultiResolutionImage.o
	$(CC) $(CFLAGS) VSIImage.cpp $(ADDITINAL_INCLUDE_DIR)

LIFImageFactory.o : $(EXPORT_HEADER) LIFImageFactory.cpp LIFImageFactory.h LIFImage.o 
	$(CC) $(CFLAGS) LIFImageFactory.cpp $(ADDITINAL_INCLUDE_DIR)

LIFImage.o : $(EXPORT_HEADER) LIFImage.cpp LIFImage.h MultiResolutionImage.o
	$(CC) $(CFLAGS) LIFImage.cpp -I$(INCLUDE_DIR)  $(ADDITINAL_INCLUDE_DIR)

TIFFImage.o : $(EXPORT_HEADER) TIFFImage.cpp TIFFImage.h MultiResolutionImage.o
	$(CC) $(CFLAGS) TIFFImage.cpp $(ADDITINAL_INCLUDE_DIR)

TIFFImageFactory.o : $(EXPORT_HEADER) TIFFImageFactory.cpp TIFFImageFactory.h TIFFImage.o 
	$(CC) $(CFLAGS) TIFFImageFactory.cpp $(ADDITINAL_INCLUDE_DIR)

MultiResolutionImage.o : $(EXPORT_HEADER) MultiResolutionImage.cpp MultiResolutionImage.h #$(BIN_DIR)/Core.so
	$(CC) $(CFLAGS) MultiResolutionImage.cpp $(ADDITINAL_INCLUDE_DIR)

AperioSVSWriter.o : $(EXPORT_HEADER) AperioSVSWriter.cpp AperioSVSWriter.h JPEG2000Codec.o
	$(CC) $(CFLAGS) AperioSVSWriter.cpp $(ADDITINAL_INCLUDE_DIR)

TileCache.o : $(EXPORT_HEADER) TileCache.cpp TileCache.h
	$(CC) $(CFLAGS) TileCache.cpp $(ADDITINAL_INCLUDE_DIR)

JPEG2000Codec.o : $(EXPORT_HEADER) JPEG2000Codec.cpp JPEG2000Codec.h
	$(CC) $(CFLAGS) JPEG2000Codec.cpp $(ADDITINAL_INCLUDE_DIR)

.PHONY : clean
clean:
	-rm $(OBJS) $(TARGET)
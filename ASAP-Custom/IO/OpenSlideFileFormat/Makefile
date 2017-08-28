#DBImporter makefile

#version (default debug)
ver = debug

#compiler
CC = g++ -Wall -std=c++11

ifeq (ver , debug)
CC += -g -fPIC
else
CC += -fPIC
endif

MODULE_NAME = openslidefileformat
ROOT = ../..
INCLUDE_ROOT_DIR = $(ROOT)/include
INCLUDE_DIR = $(INCLUDE_ROOT_DIR)/io
BIN_DIR = $(ROOT)/bin

VPATH = $(INCLUDE_ROOT_DIR):$(INCLUDE_DIR):$(INCLUDE_DIR)/$(MODULE_NAME)

TARGET = $(BIN_DIR)/formats/$(MODULE_NAME).so
OBJS = OpenSlideImage.o OpenSlideImageFactory.o

ADDITINAL_INCLUDE_DIR = -I$(INCLUDE_ROOT_DIR) -I$(INCLUDE_DIR) -I$(INCLUDE_DIR)/$(MODULE_NAME) \
-I$(INCLUDE_DIR)/multiresolutionimageinterface -I/usr/include/openslide
ADDITIONAL_DEPENDENCIES = -L$(BIN_DIR) -lopenslide -lboost_system -lboost_thread \
-lboost_chrono -lboost_date_time -lboost_atomic -lcore -lmultiresolutionimageinterface

$(TARGET) : $(OBJS)
	$(CC) -shared -o $(TARGET) $(OBJS) $(ADDITIONAL_DEPENDENCIES)

OpenSlideImageFactory.o : openslidefileformat_export.h OpenSlideImageFactory.h OpenSlideImage.o
	$(CC) -c $(BIN_DIR) $(ADDITINAL_INCLUDE_DIR) OpenSlideImageFactory.cpp

OpenSlideImage.o : openslidefileformat_export.h OpenSlideImage.h
	$(CC) -c $(BIN_DIR) $(ADDITINAL_INCLUDE_DIR) OpenSlideImage.cpp

.PHONY : clean
clean :
	rm $(OBJS) $(TARGET)
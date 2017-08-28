#Annotation makefile

#version (default debug)
ver = debug

#compiler
CC = g++ -std=c++11

#file direction
MODULE_NAME = annotation
ROOT = ..
INCLUDE_ROOT_DIR = $(ROOT)/include
INCLUDE_DIR = $(INCLUDE_ROOT_DIR)/$(MODULE_NAME)
BIN_DIR = $(ROOT)/bin

VPATH = $(INCLUDE_ROOT_DIR) : $(INCLUDE_DIR)

#debug/release cc
ifeq ($(ver) , debug)
CFLAGS += -c -g -fPIC
else
CFLAGS += -c -fPIC
endif

#macro
# DEFS = CORE_STATIC_DEFINE 
# CFLAGS += -D $(DEFS)

#target and obj
TARGET = $(BIN_DIR)/lib$(MODULE_NAME).so
OBJS = Annotation.o AnnotationBase.o AnnotationGroup.o AnnotationList.o Repository.o\
AnnotationToMask.o BinaryRepository.o ImageScopeRepository.o NDPARepository.o XmlRepository.o AnnotationService.o

EXPORT_HEADER = annotation_export.h
ADDITINAL_INCLUDE_DIR = -I$(INCLUDE_ROOT_DIR) -I$(INCLUDE_DIR) -I$(INCLUDE_ROOT_DIR)/io \
-I$(INCLUDE_ROOT_DIR)/io/multiresolutionimageinterface -I$(INCLUDE_ROOT_DIR)/io/openslidefileformat 

ADDITIANL_DEPENDENCIES = -L$(BIN_DIR) -lcore -lmultiresolutionimageinterface -lpugixml -lmbedcrypto -lmbedtls -lmbedx509  

$(TARGET): $(OBJS) $(EXPORT_HEADER)
	$(CC) -shared -o $(TARGET) $(OBJS) $(ADDITIANL_DEPENDENCIES)

filetools.o : $(EXPORT_HEADER) filetools.cpp filetools.h stringconversion.o
	$(CC) $(CFLAGS) filetools.cpp $(ADDITINAL_INCLUDE_DIR)

AnnotationService.o : $(EXPORT_HEADER) AnnotationService.cpp AnnotationService.h \
AnnotationToMask.o BinaryRepository.o ImageScopeRepository.o NDPARepository.o XmlRepository.o
	$(CC) $(CFLAGS) AnnotationService.cpp $(ADDITINAL_INCLUDE_DIR)
	
AnnotationToMask.o : $(EXPORT_HEADER) AnnotationToMask.cpp AnnotationToMask.h Repository.o
	$(CC) $(CFLAGS) AnnotationToMask.cpp $(ADDITINAL_INCLUDE_DIR)

BinaryRepository.o : $(EXPORT_HEADER) BinaryRepository.cpp BinaryRepository.h Repository.o
	$(CC) $(CFLAGS) BinaryRepository.cpp $(ADDITINAL_INCLUDE_DIR)

ImageScopeRepository.o : $(EXPORT_HEADER) ImageScopeRepository.cpp ImageScopeRepository.h Repository.o
	$(CC) $(CFLAGS) ImageScopeRepository.cpp $(ADDITINAL_INCLUDE_DIR)

NDPARepository.o : $(EXPORT_HEADER) NDPARepository.cpp NDPARepository.h Repository.o
	$(CC) $(CFLAGS) NDPARepository.cpp $(ADDITINAL_INCLUDE_DIR)

XmlRepository.o : $(EXPORT_HEADER) XmlRepository.cpp XmlRepository.h Repository.o
	$(CC) $(CFLAGS) XmlRepository.cpp $(ADDITINAL_INCLUDE_DIR)

Repository.o : $(EXPORT_HEADER) Repository.cpp Repository.h AnnotationList.o
	$(CC) $(CFLAGS) Repository.cpp $(ADDITINAL_INCLUDE_DIR)

AnnotationList.o : $(EXPORT_HEADER) AnnotationList.cpp AnnotationList.h AnnotationGroup.o Annotation.o
	$(CC) $(CFLAGS) AnnotationList.cpp $(ADDITINAL_INCLUDE_DIR)

AnnotationGroup.o : $(EXPORT_HEADER) AnnotationGroup.cpp AnnotationGroup.h AnnotationBase.o 
	$(CC) $(CFLAGS) AnnotationGroup.cpp $(ADDITINAL_INCLUDE_DIR)

Annotation.o : $(EXPORT_HEADER) Annotation.cpp Annotation.h Annotation_define.h AnnotationBase.o 
	$(CC) $(CFLAGS) Annotation.cpp $(ADDITINAL_INCLUDE_DIR)

AnnotationBase.o : $(EXPORT_HEADER) AnnotationBase.cpp AnnotationBase.h psimpl.h
	$(CC) $(CFLAGS) AnnotationBase.cpp $(ADDITINAL_INCLUDE_DIR)


.PHONY : clean
clean:
	-rm $(OBJS) $(TARGET)





	




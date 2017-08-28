#Core makefile

#version (default debug)
ver = debug

#compiler
CC = g++ -Wall -std=c++11

#file direction
MODULE_NAME = core
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
OBJS = filetools.o CmdLineProgressMonitor.o ImageSource.o stringconversion.o ProgressMonitor.o Box.o Point.o PathologyEnums.o 

EXPORT_HEADER = core_export.h
ADDITINAL_INCLUDE_DIR = -I$(INCLUDE_ROOT_DIR) -I$(INCLUDE_DIR)
ADDITIANL_DEPENDENCIES = -lboost_system -lboost_filesystem -lboost_regex

$(TARGET): $(OBJS) $(EXPORT_HEADER)
	$(CC) -shared -o $(TARGET) $(OBJS) $(ADDITIANL_DEPENDENCIES)

filetools.o : $(EXPORT_HEADER) filetools.cpp filetools.h stringconversion.o
	$(CC) $(CFLAGS) filetools.cpp $(ADDITINAL_INCLUDE_DIR)

CmdLineProgressMonitor.o : $(EXPORT_HEADER) CmdLineProgressMonitor.cpp CmdLineProgressMonitor.h ProgressMonitor.o
	$(CC) $(CFLAGS) CmdLineProgressMonitor.cpp $(ADDITINAL_INCLUDE_DIR)
	
ImageSource.o : $(EXPORT_HEADER) ImageSource.cpp ImageSource.h PathologyEnums.o
	$(CC) $(CFLAGS) ImageSource.cpp $(ADDITINAL_INCLUDE_DIR)

stringconversion.o : $(EXPORT_HEADER) stringconversion.cpp stringconversion.h 
	$(CC) $(CFLAGS) stringconversion.cpp $(ADDITINAL_INCLUDE_DIR)

ProgressMonitor.o : $(EXPORT_HEADER) ProgressMonitor.cpp ProgressMonitor.h
	$(CC) $(CFLAGS) ProgressMonitor.cpp $(ADDITINAL_INCLUDE_DIR)

Box.o : $(EXPORT_HEADER) Box.cpp Box.h
	$(CC) $(CFLAGS) Box.cpp $(ADDITINAL_INCLUDE_DIR)

Point.o : $(EXPORT_HEADER) Point.cpp Point.h
	$(CC) $(CFLAGS) Point.cpp $(ADDITINAL_INCLUDE_DIR)

PathologyEnums.o : $(EXPORT_HEADER) PathologyEnums.cpp PathologyEnums.h
	$(CC) $(CFLAGS) PathologyEnums.cpp $(ADDITINAL_INCLUDE_DIR)


.PHONY : clean
clean:
	-rm $(OBJS) $(TARGET)





	




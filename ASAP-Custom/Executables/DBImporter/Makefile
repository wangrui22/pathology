#DBImporter makefile

#version (default debug)
ver = debug

#compiler
CC = g++ -Wall -std=c++11

#file direction
MODULE_NAME = dbimporter
ROOT = ../..
INCLUDE_ROOT_DIR = $(ROOT)/include
BIN_DIR = $(ROOT)/bin

VPATH = $(INCLUDE_ROOT_DIR)

#debug/release cc
ifeq ($(ver) , debug)
CFLAGS += -g
endif

#macro
# DEFS =  
# CFLAGS += -D $(DEFS)

#target and obj
TARGET = $(BIN_DIR)/$(MODULE_NAME).out
OBJS = anno_server.o

ADDITINAL_INCLUDE_DIR = -I$(INCLUDE_ROOT_DIR)
ADDITIANL_DEPENDENCIES = -L$(BIN_DIR) -lcore -lmultiresolutionimageinterface -lmysqlcppconn -lmbedcrypto -lmbedtls -lmbedx509 -lboost_system -lboost_filesystem -ldl

$(TARGET):db_importer.cpp
	$(CC) $(CFLAGS) $(ADDITINAL_INCLUDE_DIR) -o $(TARGET) db_importer.cpp $(ADDITIANL_DEPENDENCIES)

.PHONY : clean
clean:
	-rm $(OBJS) $(TARGET)





	




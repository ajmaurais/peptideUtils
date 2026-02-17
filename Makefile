
CXX = g++
OUT_FILE_NAME = libpeptideUtils.a

PROG_DIR:=$(shell pwd)
SHARE_DIR:=$(PROG_DIR)/share

CXXFLAGS= -fPIC -c -g -Wall -std=c++11 -DENABLE_ZLIB -DSHARE_DIR="\"${SHARE_DIR}\""

HEADER_DIR=./include

SRC_DIR=./src

OBJ_DIR=./obj

OUT_DIR=./lib

SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
OBJS := $(subst $(SRC_DIR)/,$(OBJ_DIR)/,$(SRCS:.cpp=.o))
CXXFLAGS += -I$(HEADER_DIR)

.PHONY: doc

$(OUT_FILE_NAME): $(OBJS)
	mkdir -p $(OUT_DIR)
	ar rvs $(OUT_DIR)/$@ $^

#Compiling every *.cpp to *.o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $< -o $@

doc:
	doxygen doc/doxygen/Doxyfile

clean:
	rm -rf $(OBJ_DIR) $(OUT_DIR)/$(OUT_FILE_NAME) Makefile.bak

rebuild: clean build


CXX = g++
OUT_FILE_NAME = utils.a

CXXFLAGS= -fPIC -c -g -Wall -std=c++11

HEADER_DIR=./include

SRC_DIR=./src

OBJ_DIR=./obj

OUT_DIR=./lib

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(subst $(SRC_DIR)/,$(OBJ_DIR)/,$(SRCS:.cpp=.o))
CXXFLAGS += -I$(HEADER_DIR)

.PHONY: doc

$(OUT_FILE_NAME): $(OBJS)
	mkdir -p $(OUT_DIR)
	ar rvs $(OUT_DIR)/$@ $^

#Compiling every *.cpp to *.o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADER_DIR)/%.hpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@

doc:
	doxygen doc/doxygen/Doxyfile

clean:
	rm -f $(OBJ_DIR)/*.o $(OUT_DIR)/$(OUT_FILE_NAME) Makefile.bak

rebuild: clean build

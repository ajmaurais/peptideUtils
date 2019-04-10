#  Makefile template for Static library. 
# 1. Compile every *.cpp in the folder 
# 2. All obj files under obj folder
# 3. static library .a at lib folder
# 4. run 'make dirmake' before calling 'make'


CC = g++
OUT_FILE_NAME = utils.a

CXXFLAGS= -fPIC -c -g -Wall -std=c++11

HEADER_DIR=./include

SRC_DIR=./src

OBJ_DIR=./obj

OUT_DIR=./lib

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(subst $(SRC_DIR)/,$(OBJ_DIR)/,$(SRCS:.cpp=.o))
CXXFLAGS += -I$(HEADER_DIR)

#$(OUT_FILE_NAME): $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(wildcard *.cpp))

$(OUT_FILE_NAME): $(OBJS)
	mkdir -p $(OUT_DIR)
	ar rvs $(OUT_DIR)/$@ $^

#Compiling every *.cpp to *.o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADER_DIR)/%.hpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@
	
clean:
	rm -f $(OBJ_DIR)/*.o $(OUT_DIR)/$(OUT_FILE_NAME) Makefile.bak

rebuild: clean build

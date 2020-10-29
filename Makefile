BIN_DIR = bin
BUILD_DIRS = sadmd sadcd sadfsd comm example $(BIN_DIR)
BUILD = build
BIN = $(BUILD)/$(BIN_DIR)
SRC = src
INC = include
LIB = boost_program_options
LIB_DIRS =

# detect OS
OS ?= $(shell uname -s)

# customize variables for the OS we're running on
ifeq "$(OS)" "OpenBSD"
	CXX = clang++
	INC += /usr/local/include
	LIB_DIRS += /usr/local/lib
else
	CXX = g++
endif

CXX_FLAGS = -c -std=c++17 -ggdb
# do not optimize if requested
ifneq "$(OPT)" "no"
	CXX_FLAGS += -O
endif
CXX_INC = $(INC:%=-I%)       # add '-I' prefix to each value in INC
CXX_LIB = $(LIB:%=-l%)       # add '-l' prefix to each value in LIB
CXX_LIB += $(LIB_DIRS:%=-L%) # add '-L' prefix to each value in LIB_DIRS

# build source file list
COMMON_SRC = $(wildcard $(SRC)/comm/*.cpp)

SADMD_SRC = $(filter-out \
	    $(SRC)/sadmd/sadmd-bootstrap.cpp, \
	    $(wildcard $(SRC)/sadmd/*.cpp))

SADCD_SRC = $(filter-out \
	    $(SRC)/sadcd/sadcd-bootstrap.cpp, \
	    $(wildcard $(SRC)/sadcd/*.cpp))

SADFSD_SRC = $(filter-out \
	     $(SRC)/sadfsd/sadfsd-bootstrap.cpp, \
	     $(wildcard $(SRC)/sadfsd/*.cpp))

# build object file list
SADMD_OBJ = $(SADMD_SRC:$(SRC)/%.cpp=$(BUILD)/%.o)
SADCD_OBJ = $(SADCD_SRC:$(SRC)/%.cpp=$(BUILD)/%.o)
SADFSD_OBJ = $(SADFSD_SRC:$(SRC)/%.cpp=$(BUILD)/%.o)
COMMON_OBJ = $(COMMON_SRC:$(SRC)/%.cpp=$(BUILD)/%.o)

client: $(BIN)/sadfsd-bootstrap

master: $(BIN)/sadmd-bootstrap

chunk: $(BIN)/sadcd-bootstrap $(BIN)/sadcd

ex: $(patsubst example/%.cpp, $(BIN)/%, $(wildcard example/*.cpp))

# sadfs binaries
$(BIN)/%: mkdirs $(BUILD)/sadfsd/%.o
	$(CXX) $(filter-out $<, $^) $(CXX_LIB) -o $@

$(BIN)/%: mkdirs $(BUILD)/sadmd/%.o
	$(CXX) $(filter-out $<, $^) $(CXX_LIB) -o $@

$(BIN)/%: mkdirs $(BUILD)/sadcd/%.o
	$(CXX) $(filter-out $<, $^) $(CXX_LIB) -o $@

$(BUILD)/%.o: $(SRC)/%.cpp
	$(CXX) $(CXX_FLAGS) $(CXX_INC) $< -o $@

# example binaries
$(BIN)/%: mkdirs $(BUILD)/example/%.o $(COMMON_OBJ)
	$(CXX) $(filter-out $<, $^) -o $@

$(BUILD)/example/%.o: example/%.cpp
	$(CXX) $(CXX_FLAGS) $(CXX_INC) $< -o $@

clean:
	@-rm -r $(BUILD) >/dev/null 2>&1

mkdirs:
	@mkdir -p $(BUILD_DIRS:%=$(BUILD)/%)

.PHONY: clean mkdirs client master chunk ex

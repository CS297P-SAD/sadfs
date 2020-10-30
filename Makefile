BUILD = build
BIN = $(BUILD)/bin
SRC = src
INC = include
LIB = boost_program_options
LIB_DIRS =

MKDIR = mkdir -p
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

CXX_FLAGS = -c -std=c++17 -ggdb -W -Wall -Wextra -Werror
# do not optimize if requested
ifneq "$(OPT)" "no"
	CXX_FLAGS += -O
endif
CXX_INC = $(INC:%=-I%)       # add '-I' prefix to each value in INC
CXX_LIB = $(LIB:%=-l%)       # add '-l' prefix to each value in LIB
CXX_LIB += $(LIB_DIRS:%=-L%) # add '-L' prefix to each value in LIB_DIRS

# build source file list
COMMON_SRC = $(wildcard $(SRC)/comm/*.cpp)

SADMD_SRC = $(wildcard $(SRC)/sadmd/*.cpp))

SADCD_SRC = $(wildcard $(SRC)/sadcd/*.cpp))

SADFSD_SRC = $(wildcard $(SRC)/sadfsd/*.cpp))

# build object file list
SADMD_OBJ = $(SADMD_SRC:$(SRC)/%.cpp=$(BUILD)/%.o)
SADCD_OBJ = $(SADCD_SRC:$(SRC)/%.cpp=$(BUILD)/%.o)
SADFSD_OBJ = $(SADFSD_SRC:$(SRC)/%.cpp=$(BUILD)/%.o)
COMMON_OBJ = $(COMMON_SRC:$(SRC)/%.cpp=$(BUILD)/%.o)

client: $(BIN)/sadfsd-bootstrap

master: $(BIN)/sadmd-bootstrap $(BIN)/sadmd

chunk: $(BIN)/sadcd-bootstrap $(BIN)/sadcd

ex: $(patsubst example/%.cpp, $(BIN)/%, $(wildcard example/*.cpp))

all: client master chunk ex

# sadfs binaries
$(BIN)/%: $(BUILD)/sadfsd/%.o
	@$(MKDIR) $(@D)
	$(CXX) $^ $(CXX_LIB) -o $@

$(BIN)/%: $(BUILD)/sadmd/%.o
	@$(MKDIR) $(@D)
	$(CXX) $^ $(CXX_LIB) -o $@

$(BIN)/%: $(BUILD)/sadcd/%.o
	@$(MKDIR) $(@D)
	$(CXX) $^ $(CXX_LIB) -o $@

$(BIN)/%-bootstrap: $(BUILD)/bootstrap/%-bootstrap.o $(BUILD)/bootstrap/util.o
	@$(MKDIR) $(@D)
	$(CXX) $^ $(CXX_LIB) -o $@

$(BUILD)/%.o: $(SRC)/%.cpp
	@$(MKDIR) $(@D)
	$(CXX) $(CXX_FLAGS) $(CXX_INC) $^ -o $@

# example binaries
$(BIN)/%: $(BUILD)/example/%.o $(COMMON_OBJ)
	@$(MKDIR) $(@D)
	$(CXX) $^ -o $@

$(BUILD)/example/%.o: example/%.cpp
	@$(MKDIR) $(@D)
	$(CXX) $(CXX_FLAGS) $(CXX_INC) $^ -o $@

clean:
	@-rm -r $(BUILD) >/dev/null 2>&1

.PHONY: clean client master chunk ex

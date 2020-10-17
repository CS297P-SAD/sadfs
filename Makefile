BUILD = build
BUILD_DIRS = sadmd sadcd sadfsd
SRC = src
INC = include
LIB = boost_program_options

CXX = g++
CXX_FLAGS = -c -std=c++17 -ggdb
OPT ?= yes
ifeq "$(OPT)" "yes"
	CXX_FLAGS += -O
endif
CXX_INC = $(INC:%=-I%) # add '-I' prefix to each value in INC
CXX_LIB = $(LIB:%=-l%) # add '-l' prefix to each value in LIB

SADMD_SRC=$(filter-out $(SRC)/sadmd/sadmd-bootstrap.cpp, $(wildcard $(SRC)/sadmd/%.cpp))
SADCD_SRC=$(filter-out $(SRC)/sadcd/sadcd-bootstrap.cpp, $(wildcard $(SRC)/sadcd/%.cpp))
SADFSD_SRC=$(filter-out $(SRC)/sadfsd/sadfsd-bootstrap.cpp, $(wildcard $(SRC)/sadfsd/%.cpp))
SADMD_OBJ=$(SADMD:%.cpp=%.o)
SADCD_OBJ=$(SADCD:%.cpp=%.o)
SADFSD_OBJ=$(SADFSD:%.cpp=%.o)

all: sadmd-bootstrap

sadmd-bootstrap: mkdirs $(BUILD)/sadmd/sadmd-bootstrap.o
	$(CXX) $(filter-out $<, $^) $(CXX_LIB) -o $(BUILD)/sadmd/$@

$(BUILD)/%.o: $(SRC)/%.cpp
	$(CXX) $(CXX_FLAGS) $(CXX_INC) $^ -o $@

clean:
	@rm -r $(BUILD) >/dev/null 2>&1 || true

mkdirs:
	@mkdir -p $(BUILD_DIRS:%=$(BUILD)/%)

.PHONY: clean mkdirs all sadmd-bootstrap

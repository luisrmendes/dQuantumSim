CXX = mpic++
CXXFLAGS = -Wall -c
PROJ_NAME = simulator

OBJS_DIR := obj
SRCS_DIR := src
INC_DIR := include
SRCS_FILES := $(SRCS_DIR)/main.cpp $(SRCS_DIR)/QubitLayerMPI.cpp \
$(SRCS_DIR)/utils.cpp $(SRCS_DIR)/parser.cpp \
$(SRCS_DIR)/utilsMPI.cpp

OBJS_FILES = $(patsubst $(SRCS_DIR)/%.cpp,$(OBJS_DIR)/%.o,$(SRCS_FILES))

.PHONY: all clean test

all: simulator

simulator: $(OBJS_FILES)
	$(CXX) $(OBJS_FILES) $(LDFLAGS) -o $@


$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) $< -o $@

test:
	@echo "SRC FILES = '$(SRCS_FILES)'"
	@echo "OBJ FILES = '$(OBJS_FILES)'"

clean:
	rm -f $(OBJS_DIR)/*.o
	rm -f simulator

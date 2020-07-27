INC_DIR := include
SRC_DIR := src
BIN_DIR := bin
OBJ_DIR := $(BIN_DIR)/obj

CC := gcc
CXX := g++
LD := $(CXX)
CFLAGS := -I$(INC_DIR)
LDFLAGS :=

AR := ar
MKDIR := mkdir -p
RM := rm -rf
CP := cp
CD := cd
MAKE := make
CMAKE := cmake

# debug flags
# note that clean must be run when switching between debug and release
# to ensure that everything is built with the new flags
DEBUG ?= 0
ifeq ($(DEBUG), 1)
	CFLAGS += -g -DDEBUG
endif

# cimgui
CIMGUI_DIR := cimgui
CIMGUI_OBJ_DIR := $(OBJ_DIR)/$(CIMGUI_DIR)
CIMGUI_MAKE_DIR := $(BIN_DIR)/$(CIMGUI_DIR)
CIMGUI_INC_DIR := $(CIMGUI_DIR)/generator/output
CIMGUI_BUILD_DIR := $(CIMGUI_MAKE_DIR)/CMakeFiles/cimgui.dir
CIMGUI_OBJS := $(CIMGUI_BUILD_DIR)/cimgui.cpp.o $(CIMGUI_BUILD_DIR)/imgui/imgui.cpp.o $(CIMGUI_BUILD_DIR)/imgui/imgui_demo.cpp.o $(CIMGUI_BUILD_DIR)/imgui/imgui_draw.cpp.o $(CIMGUI_BUILD_DIR)/imgui/imgui_widgets.cpp.o
CIMGUI_OUT := $(CIMGUI_MAKE_DIR)/cimgui.a

$(CIMGUI_OUT): | $(CIMGUI_MAKE_DIR)
	$(CD) $(CIMGUI_MAKE_DIR) && $(CMAKE) ../../$(CIMGUI_DIR) -DIMGUI_STATIC="yes"
	$(CD) $(CIMGUI_MAKE_DIR) && $(MAKE)

$(CIMGUI_OBJS): $(CIMGUI_OUT)

$(CIMGUI_MAKE_DIR):
	$(MKDIR) $@

# all targets should be able to include cimgui as they are linked with it
CFLAGS += -I$(CIMGUI_DIR) -I$(CIMGUI_INC_DIR)

# imgui impl
IMGUI_DIR := $(CIMGUI_DIR)/imgui
IMGUI_IMPL_SRC_DIR := $(IMGUI_DIR)/examples
IMGUI_IMPL_OBJ_DIR := $(OBJ_DIR)/imgui_impl
IMGUI_IMPL_SRCS := $(IMGUI_IMPL_SRC_DIR)/imgui_impl_glfw.cpp $(IMGUI_IMPL_SRC_DIR)/imgui_impl_opengl3.cpp
IMGUI_IMPL_OBJS := $(IMGUI_IMPL_SRCS:$(IMGUI_IMPL_SRC_DIR)/%.cpp=$(IMGUI_IMPL_OBJ_DIR)/%.o)
IMGUI_IMPL_CXXFLAGS := -I$(IMGUI_DIR) -I$(INC_DIR) -DIMGUI_IMPL_API='extern "C"' -DIMGUI_IMPL_OPENGL_LOADER_CUSTOM='<core/gl.h>'

$(IMGUI_IMPL_OBJS): $(IMGUI_IMPL_OBJ_DIR)/%.o : $(IMGUI_IMPL_SRC_DIR)/%.cpp | $(IMGUI_IMPL_OBJ_DIR)
	$(CXX) -c $< -o $@ $(IMGUI_IMPL_CXXFLAGS)

$(IMGUI_IMPL_OBJ_DIR):
	$(MKDIR) $@

# core
CORE_DIR := core
CORE_INC_DIR := $(INC_DIR)/$(CORE_DIR)
CORE_SRC_DIR := $(SRC_DIR)/$(CORE_DIR)
CORE_OBJ_DIR := $(OBJ_DIR)/$(CORE_DIR)
CORE_SRCS := $(wildcard $(CORE_SRC_DIR)/*.c)
CORE_OBJS := $(CORE_SRCS:$(CORE_SRC_DIR)/%.c=$(CORE_OBJ_DIR)/%.o)
CORE_DEPS := $(CORE_OBJS:%.o=%.d)
CORE_CFLAGS := $(CFLAGS) -I$(CORE_INC_DIR)
CORE_LDFLAGS := $(LDFLAGS) -lglfw -lGL -lpng -pthread
CORE_OUT := $(BIN_DIR)/libcore.a

$(CORE_OUT): $(CORE_OBJS) $(CIMGUI_OBJS) $(IMGUI_IMPL_OBJS) | $(BIN_DIR)
	$(AR) rcs $@ $^

$(CORE_OBJS): $(CORE_OBJ_DIR)/%.o : $(CORE_SRC_DIR)/%.c | $(CORE_OBJ_DIR)
	$(CC) -MMD -c $< -o $@ $(CORE_CFLAGS)

$(CORE_OBJ_DIR):
	$(MKDIR) $@

# game
GAME_DIR := game
GAME_INC_DIR := $(INC_DIR)/$(GAME_DIR)
GAME_SRC_DIR := $(SRC_DIR)/$(GAME_DIR)
GAME_OBJ_DIR := $(OBJ_DIR)/$(GAME_DIR)
GAME_SRCS := $(wildcard $(GAME_SRC_DIR)/*.c)
GAME_OBJS := $(GAME_SRCS:$(GAME_SRC_DIR)/%.c=$(GAME_OBJ_DIR)/%.o)
GAME_DEPS := $(GAME_OBJS:%.o=%.d)
GAME_CFLAGS := $(CFLAGS) -I$(GAME_INC_DIR)
GAME_LDFLAGS := $(CORE_LDFLAGS)
GAME_OUT := $(BIN_DIR)/game

$(GAME_OUT): $(GAME_OBJS) $(CORE_OUT) | $(BIN_DIR)
	$(LD) $^ -o $@ $(GAME_LDFLAGS)

$(GAME_OBJS): $(GAME_OBJ_DIR)/%.o : $(GAME_SRC_DIR)/%.c | $(GAME_OBJ_DIR)
	$(CC) -MMD -c $< -o $@ $(GAME_CFLAGS)

$(GAME_OBJ_DIR):
	$(MKDIR) $@

# design
DESIGN_DIR := design
DESIGN_INC_DIR := $(INC_DIR)/$(DESIGN_DIR)
DESIGN_SRC_DIR := $(SRC_DIR)/$(DESIGN_DIR)
DESIGN_OBJ_DIR := $(OBJ_DIR)/$(DESIGN_DIR)
DESIGN_SRCS := $(wildcard $(DESIGN_SRC_DIR)/*.c)
DESIGN_OBJS := $(DESIGN_SRCS:$(DESIGN_SRC_DIR)/%.c=$(DESIGN_OBJ_DIR)/%.o)
DESIGN_DEPS := $(DESIGN_OBJS:%.o=%.d)
DESIGN_CFLAGS := $(CFLAGS) -I$(DESIGN_INC_DIR)
DESIGN_LDFLAGS := $(CORE_LDFLAGS)
DESIGN_OUT := $(BIN_DIR)/design

$(DESIGN_OUT): $(DESIGN_OBJS) $(CORE_OUT) | $(BIN_DIR)
	$(LD) $^ -o $@ $(DESIGN_LDFLAGS)

$(DESIGN_OBJS): $(DESIGN_OBJ_DIR)/%.o : $(DESIGN_SRC_DIR)/%.c | $(DESIGN_OBJ_DIR)
	$(CC) -MMD -c $< -o $@ $(DESIGN_CFLAGS)

$(DESIGN_OBJ_DIR):
	$(MKDIR) $@

# shared
cimgui: $(CIMGUI_OBJS)
imgui_impl: $(IMGUI_IMPL_OBJS)
core: $(CORE_OUT)
game: $(GAME_OUT)
design: $(DESIGN_OUT)
all: cimgui imgui_impl core game design
.DEFAULT_GOAL := game

$(BIN_DIR):
	$(MKDIR) $@

clean:
	$(RM) $(CIMGUI_MAKE_DIR) \
          $(IMGUI_IMPL_OBJS) \
          $(CORE_OUT) $(CORE_OBJS) $(CORE_DEPS) \
          $(GAME_OUT) $(GAME_OBJS) $(GAME_DEPS) \
          $(DESIGN_OUT) $(DESIGN_OBJS) $(DESIGN_DEPS)

# include the build generated dependency files
# this allows make to detect changes to header files and recompile accordingly
-include $(CORE_DEPS)
-include $(GAME_DEPS)
-include $(DESIGN_DEPS)

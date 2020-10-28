# get the target platform
ifeq ($(OS), Windows_NT)
	PLATFORM := WINDOWS
else
	UNAME := $(uname -s)
	ifeq ($(UNAME), Linux)
		PLATFORM := LINUX
	else
		(echo "unknown platform $(OS) ($(UNAME))"; exit 1)
	endif
endif

# constants
INC_DIR := include
SRC_DIR := src
BIN_DIR := bin
OBJ_DIR := $(BIN_DIR)/obj

CC := gcc
CXX := g++
LD := $(CXX)
CFLAGS := -I$(INC_DIR) -D$(PLATFORM)
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

# windows make
# make must be redirected to mingw make
ifeq ($(PLATFORM), WINDOWS)
	MAKE := mingw32-make.exe
endif

# windows cmake
# cmake must be instructed to use the mingw toolchain on windows
ifeq ($(PLATFORM), WINDOWS)
	CMAKE := cmake -G "MinGW Makefiles"
endif

# cimgui
CIMGUI_DIR := cimgui
CIMGUI_OBJ_DIR := $(OBJ_DIR)/$(CIMGUI_DIR)
CIMGUI_MAKE_DIR := $(BIN_DIR)/$(CIMGUI_DIR)
CIMGUI_INC_DIR := $(CIMGUI_DIR)/generator/output
CIMGUI_BUILD_DIR := $(CIMGUI_MAKE_DIR)/CMakeFiles/cimgui.dir
CIMGUI_OBJS := $(CIMGUI_BUILD_DIR)/cimgui.cpp.o $(CIMGUI_BUILD_DIR)/imgui/imgui.cpp.o $(CIMGUI_BUILD_DIR)/imgui/imgui_demo.cpp.o $(CIMGUI_BUILD_DIR)/imgui/imgui_draw.cpp.o $(CIMGUI_BUILD_DIR)/imgui/imgui_widgets.cpp.o
CIMGUI_OUT := $(CIMGUI_MAKE_DIR)/cimgui.a

# cimgui object files have an obj extension instead of o on windows
ifeq ($(PLATFORM), WINDOWS)
	CIMGUI_OBJS := $(CIMGUI_OBJS:%.o=%.obj)
endif

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
IMGUI_IMPL_CXXFLAGS := -I$(IMGUI_DIR) -DIMGUI_IMPL_API='extern "C"' -DIMGUI_IMPL_OPENGL_LOADER_GLEW

# windows are always reported as unfocused in non-main threads on windows, disabling mouse input
# so force windows to always be considered focused
ifeq ($(PLATFORM), WINDOWS)
	IMGUI_IMPL_CXXFLAGS += -D__EMSCRIPTEN__
endif

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
CORE_LDFLAGS := $(LDFLAGS) -lglfw3 -lpng16 -pthread
CORE_OUT := $(BIN_DIR)/libcore.a

# link opengl depending on the platform
ifeq ($(PLATFORM), WINDOWS)
	# windows
	CORE_LDFLAGS += -lopengl32 -lglew32
else ifeq ($(PLATFORM), LINUX)
	# linux
	CORE_LDFLAGS += -lGL -lGLEW
endif

$(CORE_OUT): $(CORE_OBJS) $(CIMGUI_OBJS) $(IMGUI_IMPL_OBJS) | $(BIN_DIR)
	$(AR) rcs $@ $^

$(CORE_OBJS): $(CORE_OBJ_DIR)/%.o : $(CORE_SRC_DIR)/%.c | $(CORE_OBJ_DIR)
	$(CC) -MMD -c $< -o $@ $(CORE_CFLAGS)

$(CORE_OBJ_DIR):
	$(MKDIR) $@

# sys2d
SYS2D_DIR := sys2d
SYS2D_INC_DIR := $(INC_DIR)/$(SYS2D_DIR)
SYS2D_SRC_DIR := $(SRC_DIR)/$(SYS2D_DIR)
SYS2D_OBJ_DIR := $(OBJ_DIR)/$(SYS2D_DIR)
SYS2D_SRCS := $(wildcard $(SYS2D_SRC_DIR)/*.c)
SYS2D_OBJS := $(SYS2D_SRCS:$(SYS2D_SRC_DIR)/%.c=$(SYS2D_OBJ_DIR)/%.o)
SYS2D_DEPS := $(SYS2D_OBJS:%.o=%.d)
SYS2D_CFLAGS := $(CFLAGS) -I$(SYS2D_INC_DIR)
SYS2D_LDFLAGS := $(LDFLAGS)
SYS2D_OUT := $(BIN_DIR)/libsys2d.a

$(SYS2D_OUT): $(SYS2D_OBJS) | $(BIN_DIR)
	$(AR) rcs $@ $^

$(SYS2D_OBJS): $(SYS2D_OBJ_DIR)/%.o : $(SYS2D_SRC_DIR)/%.c | $(SYS2D_OBJ_DIR)
	$(CC) -MMD -c $< -o $@ $(SYS2D_CFLAGS)

$(SYS2D_OBJ_DIR):
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

$(GAME_OUT): $(GAME_OBJS) $(CORE_OUT) $(SYS2D_OUT) | $(BIN_DIR)
	$(LD) -Wl,-whole-archive $^ -Wl,-no-whole-archive -o $@ $(GAME_LDFLAGS)

$(GAME_OBJS): $(GAME_OBJ_DIR)/%.o : $(GAME_SRC_DIR)/%.c | $(GAME_OBJ_DIR)
	$(CC) -MMD -c $< -o $@ $(GAME_CFLAGS)

$(GAME_OBJ_DIR):
	$(MKDIR) $@

# shared
cimgui: $(CIMGUI_OBJS)
imgui_impl: $(IMGUI_IMPL_OBJS)
core: $(CORE_OUT)
sys2d: $(SYS2D_OUT)
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
          $(SYS2D_OUT) $(SYS2D_OBJS) $(SYS2D_DEPS) \
          $(GAME_OUT) $(GAME_OBJS) $(GAME_DEPS) \
          $(DESIGN_OUT) $(DESIGN_OBJS) $(DESIGN_DEPS)

# include the build generated dependency files
# this allows make to detect changes to header files and recompile accordingly
-include $(CORE_DEPS)
-include $(SYS2D_DEPS)
-include $(GAME_DEPS)
-include $(DESIGN_DEPS)

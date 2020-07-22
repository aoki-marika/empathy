INC_DIR := include
SRC_DIR := src
BIN_DIR := bin
OBJ_DIR := $(BIN_DIR)/obj

CC := gcc
CFLAGS := -I$(INC_DIR)
LDFLAGS :=
AR := ar

MKDIR := mkdir -p
RM := rm -f

# core
CORE_DIR := core
CORE_INC_DIR := $(INC_DIR)/$(CORE_DIR)
CORE_SRC_DIR := $(SRC_DIR)/$(CORE_DIR)
CORE_OBJ_DIR := $(OBJ_DIR)/$(CORE_DIR)
CORE_SRCS := $(wildcard $(CORE_SRC_DIR)/*.c)
CORE_OBJS := $(CORE_SRCS:$(CORE_SRC_DIR)/%.c=$(CORE_OBJ_DIR)/%.o)
CORE_DEPS := $(CORE_OBJS:%.o=%.d)
CORE_CFLAGS := $(CFLAGS) -I$(CORE_INC_DIR)
CORE_LDFLAGS := $(LDFLAGS)
CORE_OUT := $(BIN_DIR)/libcore.a

$(CORE_OUT): $(CORE_OBJS) | $(BIN_DIR)
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
	$(CC) $^ -o $@ $(GAME_LDFLAGS)

$(GAME_OBJS): $(GAME_OBJ_DIR)/%.o : $(GAME_SRC_DIR)/%.c | $(GAME_OBJ_DIR)
	$(CC) -MMD -c $< -o $@ $(GAME_CFLAGS)

$(GAME_OBJ_DIR):
	$(MKDIR) $@

# shared
core: $(CORE_OUT)
game: $(GAME_OUT)
all: core game
.DEFAULT_GOAL := game

$(BIN_DIR):
	$(MKDIR) $@

clean:
	$(RM) $(CORE_OUT) $(CORE_OBJS) $(CORE_DEPS) $(GAME_OUT) $(GAME_OBJS) $(GAME_DEPS)

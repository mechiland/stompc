CC = gcc
LD = gcc -o
RM = rm -rf

LINK =
INCLUDE = -Iinclude
LIBS = 
CFLAGS = $(INCLUDE)

TARGET = stompc
SRC_DIR = src
OUT_DIR = out

SOURCES = $(shell find src -name "*.c")
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OUT_DIR)/%.o, $(SOURCES))

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	@echo "Linking $(@F) ..."
	@$(LD) $@ $^ $(LIBS)

clean:
	@$(RM) $(OBJS) $(TARGET)

$(OUT_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "compiling $< to $@ ..."
	@$(CC) $(CFLAGS) -c $< -o $@ 


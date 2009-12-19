CC = gcc
LD = gcc -o
RM = rm -rf

LINK =
INCLUDE = -Iinclude
LIBS = 
CFLAGS = $(INCLUDE)

TARGET = stompc

SOURCES = $(shell find src -name "*.c")
OBJS = $(patsubst %.c, %.o, $(SOURCES))

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	@echo "Linking $(@F) ..."
	@$(LD) $@ $^ $(LIBS)

clean:
	@$(RM) $(OBJS) $(TARGET)

%.o: %.c
	@echo "compiling $< ..."
	@$(CC) $(CFLAGS) -c $< -o $@ 


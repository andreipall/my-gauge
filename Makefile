# Application name
TARGET = my-app

# Compiler
CC = gcc

# GTK 4 configuration
PKG_CONFIG = pkg-config
GTK_CFLAGS = $(shell $(PKG_CONFIG) --cflags gtk4)
GTK_LIBS = $(shell $(PKG_CONFIG) --libs gtk4)

# Compiler and linker flags
CFLAGS = -Wall -O2 $(GTK_CFLAGS)
LDLIBS = $(GTK_LIBS) -lm

# Source and object files
SRCS = main.c my-gauge.c
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET)

# Link application
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDLIBS)

# Compile source files
%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# Include generated dependency files
-include $(OBJS:.o=.d)

# Run application
run: $(TARGET)
	./$(TARGET)

# Clean generated files
clean:
	rm -f $(TARGET) $(OBJS) $(OBJS:.o=.d)

# Rebuild everything
rebuild: clean all

# Phony targets
.PHONY: all run clean rebuild

# Makefile for TCDE Signal Processor POC
# Date: 10 Novembre 2025

CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11 -I../../../src
LDFLAGS = -lm

# Source files
CORE_SRCS = ../../../src/core/tcde_core.c \
            ../../../src/core/tcde_geometry.c

PROC_SRCS = tcde_signal_processor.c
TEST_SRCS = test_signal_processor.c

# Object files
CORE_OBJS = $(CORE_SRCS:.c=.o)
PROC_OBJS = $(PROC_SRCS:.c=.o)
TEST_OBJS = $(TEST_SRCS:.c=.o)

# Targets
TARGET = test_signal_processor

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(CORE_OBJS) $(PROC_OBJS) $(TEST_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "✅ Build successful: $(TARGET)"

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGET)
	@echo "Running TCDE Signal Processor tests..."
	@./$(TARGET)

clean:
	rm -f $(TARGET) $(PROC_OBJS) $(TEST_OBJS)
	@echo "✅ Clean complete"

# Help
help:
	@echo "TCDE Signal Processor POC - Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all    - Build the test program (default)"
	@echo "  test   - Build and run tests"
	@echo "  clean  - Remove build artifacts"
	@echo "  help   - Show this help message"

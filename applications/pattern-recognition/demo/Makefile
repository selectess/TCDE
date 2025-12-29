# Makefile for TCDE Pattern Recognizer POC
# Date: 10 Novembre 2025

CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11 -I../../../src
LDFLAGS = -lm

# Source files
CORE_SRCS = ../../../src/core/tcde_core.c \
            ../../../src/core/tcde_geometry.c

RECOG_SRCS = tcde_pattern_recognizer.c
TEST_SRCS = test_pattern_recognizer.c

# Object files
CORE_OBJS = $(CORE_SRCS:.c=.o)
RECOG_OBJS = $(RECOG_SRCS:.c=.o)
TEST_OBJS = $(TEST_SRCS:.c=.o)

# Targets
TARGET = test_pattern_recognizer

.PHONY: all clean test benchmark

all: $(TARGET)

$(TARGET): $(CORE_OBJS) $(RECOG_OBJS) $(TEST_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "✅ Build successful: $(TARGET)"

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGET)
	@echo "Running TCDE Pattern Recognizer tests..."
	@./$(TARGET)

benchmark: $(TARGET)
	@echo "Running pattern recognition benchmarks..."
	@echo "=== Performance Benchmark ==="
	@./$(TARGET) | grep -A 10 "Performance Benchmark"

clean:
	rm -f $(TARGET) $(RECOG_OBJS) $(TEST_OBJS)
	@echo "✅ Clean complete"

# Help
help:
	@echo "TCDE Pattern Recognizer POC - Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all       - Build the test program (default)"
	@echo "  test      - Build and run all tests"
	@echo "  benchmark - Run performance benchmarks"
	@echo "  clean     - Remove build artifacts"
	@echo "  help      - Show this help message"

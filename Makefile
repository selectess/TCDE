# TCDE Makefile
# Topological Continuous Diffusion Equation

CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11 -I./src -I./include
LDFLAGS = -lm -lpthread -lssl -lcrypto
DEBUG_FLAGS = -g -DDEBUG

# Directories
SRC_DIR = src
TEST_DIR = tests
BIN_DIR = bin
OBJ_DIR = obj

# Create directories
$(shell mkdir -p $(BIN_DIR) $(OBJ_DIR))

# Source files
CORE_SRCS = $(wildcard $(SRC_DIR)/core/*.c)
BENCHMARK_SRCS = $(wildcard $(SRC_DIR)/benchmarks/*.c)
METRIC_SRCS = $(wildcard $(SRC_DIR)/metrics/*.c)
VALIDATION_SRCS = $(wildcard $(SRC_DIR)/validation/*.c)
SECURITY_SRCS = $(wildcard $(SRC_DIR)/security/*.c)
EMERGENCE_SRCS = $(wildcard $(SRC_DIR)/emergence/*.c)
VIZ_SRCS = $(wildcard $(SRC_DIR)/visualization/*.c)
UTILS_SRCS = $(wildcard $(SRC_DIR)/utils/*.c)

ALL_SRCS = $(CORE_SRCS) $(BENCHMARK_SRCS) $(METRIC_SRCS) $(VALIDATION_SRCS) \
           $(SECURITY_SRCS) $(EMERGENCE_SRCS) $(VIZ_SRCS) $(UTILS_SRCS)

# Object files
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(ALL_SRCS))

# Test files
TEST_SRCS = $(wildcard $(TEST_DIR)/test_*.c)
TEST_BINS = $(patsubst $(TEST_DIR)/%.c,$(BIN_DIR)/%,$(TEST_SRCS))

# Main targets
.PHONY: all clean test help

all: $(BIN_DIR)/tcde_demo $(BIN_DIR)/tcde_benchmark

# Main demo
$(BIN_DIR)/tcde_demo: $(OBJS) $(SRC_DIR)/main_demo.c
	@echo "Building TCDE demo..."
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "✅ Demo built: $@"

# Benchmark
$(BIN_DIR)/tcde_benchmark: $(OBJS) $(SRC_DIR)/main_benchmark.c
	@echo "Building TCDE benchmark..."
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "✅ Benchmark built: $@"

# Compile object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Tests
test: $(TEST_BINS)
	@echo "Running all tests..."
	@for test in $(TEST_BINS); do \
		echo "Running $$test..."; \
		$$test || exit 1; \
	done
	@echo "✅ All tests passed!"

$(BIN_DIR)/test_%: $(TEST_DIR)/test_%.c $(OBJS)
	@echo "Building test $@..."
	$(CC) $(CFLAGS) -o $@ $< $(OBJS) $(LDFLAGS)

# Clean
clean:
	@echo "Cleaning build artifacts..."
	rm -rf $(BIN_DIR) $(OBJ_DIR)
	@echo "✅ Clean complete"

# Help
help:
	@echo "TCDE Makefile"
	@echo "============="
	@echo ""
	@echo "Targets:"
	@echo "  all        - Build all main executables"
	@echo "  test       - Build and run all tests"
	@echo "  clean      - Remove all build artifacts"
	@echo "  help       - Show this help message"
	@echo ""
	@echo "Examples:"
	@echo "  make all"
	@echo "  make test"
	@echo "  make clean"

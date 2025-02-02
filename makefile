###############################################################################
#                          NATIVE MAKE-BASED BUILD
###############################################################################

# Compiler and flags
CC        = gcc
NVCC      = nvcc
CFLAGS    = -Wall -Wextra -pedantic -std=c99 -O2 -Iinclude
CUDAFLAGS = -arch=sm_60 -O2
LDFLAGS   = -lssl -lcrypto -lcurl -lpthread
DEBUG_CFLAGS = -g -DDEBUG

# Directories (native build)
SRC_DIR       = ./src
NATIVE_BUILD_DIR = ./build
INCLUDE_DIR   = ./include
BINARIES_DIR  = ./binaries
LOG_DIR       = ./logs
INSTALL_DIR   ?= /opt/pmll  # Allow overriding the install directory

# Source files
SOURCES      = $(wildcard $(SRC_DIR)/*.c)
CUDA_SOURCES = $(wildcard $(SRC_DIR)/*.cu)
OBJECTS      = $(patsubst $(SRC_DIR)/%.c,  $(NATIVE_BUILD_DIR)/%.o, $(SOURCES)) \
               $(patsubst $(SRC_DIR)/%.cu, $(NATIVE_BUILD_DIR)/%.o, $(CUDA_SOURCES))

# Target binaries (native)
TARGETS = pmll arc_agi_benchmark pmll_np_solver sat_test api_llama api \
          vector_matrix silo_manager logic_loop custodian free gpu_program bugwatcher

# Versioning
VERSION       = 1.0.0
LDFLAGS      += -DVERSION=\"$(VERSION)\"

# ----------------------------------------------------------------------
# Phony targets for the native build
# ----------------------------------------------------------------------
.PHONY: native_all native_parallel native_clean native_orchestrate native_deploy \
        native_debug native_static_analysis native_deps native_test \
        prepare_dirs

# Default native build target
native_all: prepare_dirs $(TARGETS)

# Build in parallel using 4 jobs
native_parallel:
	$(MAKE) -j4 native_all

# Create needed directories
prepare_dirs:
	@mkdir -p $(NATIVE_BUILD_DIR) $(BINARIES_DIR) $(LOG_DIR)

# Build each target (example for pmll)
pmll: $(NATIVE_BUILD_DIR)/unified_voice.o $(NATIVE_BUILD_DIR)/pml_logic_loop.o \
      $(NATIVE_BUILD_DIR)/memory_silo.o $(NATIVE_BUILD_DIR)/io_socket.o \
      $(NATIVE_BUILD_DIR)/persistence.o $(NATIVE_BUILD_DIR)/cross_talk.o \
      $(NATIVE_BUILD_DIR)/custodian.o $(NATIVE_BUILD_DIR)/main.o \
      $(NATIVE_BUILD_DIR)/vector_matrix.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BINARIES_DIR)/$@ $^

# (Repeat the pattern for arc_agi_benchmark, pmll_np_solver, etc.)
arc_agi_benchmark: $(NATIVE_BUILD_DIR)/arc_agi_benchmark.o $(NATIVE_BUILD_DIR)/vector_matrix.o $(NATIVE_BUILD_DIR)/io_socket.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BINARIES_DIR)/$@ $^

pmll_np_solver: $(NATIVE_BUILD_DIR)/pmll_np_solver.o $(NATIVE_BUILD_DIR)/vector_matrix.o $(NATIVE_BUILD_DIR)/io_socket.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BINARIES_DIR)/$@ $^

sat_test: $(NATIVE_BUILD_DIR)/SAT_Compare.o $(NATIVE_BUILD_DIR)/Pmll_NP_Solver.o $(NATIVE_BUILD_DIR)/MiniSAT.o \
          $(NATIVE_BUILD_DIR)/generate_3sat_instance.o $(NATIVE_BUILD_DIR)/SAT_Solver.o \
          $(NATIVE_BUILD_DIR)/vector_matrix.o $(NATIVE_BUILD_DIR)/io_socket.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BINARIES_DIR)/$@ $^

api_llama: $(NATIVE_BUILD_DIR)/API_Llama.o $(NATIVE_BUILD_DIR)/vector_matrix.o $(NATIVE_BUILD_DIR)/io_socket.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BINARIES_DIR)/$@ $^

api: $(NATIVE_BUILD_DIR)/API.o $(NATIVE_BUILD_DIR)/vector_matrix.o $(NATIVE_BUILD_DIR)/io_socket.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BINARIES_DIR)/$@ $^

vector_matrix: $(NATIVE_BUILD_DIR)/vector_matrix.o $(NATIVE_BUILD_DIR)/io_socket.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BINARIES_DIR)/$@ $^

silo_manager: $(NATIVE_BUILD_DIR)/silo_manager.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BINARIES_DIR)/$@ $^

logic_loop: $(NATIVE_BUILD_DIR)/logic_loop.o $(NATIVE_BUILD_DIR)/pml_logic_loop.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BINARIES_DIR)/$@ $^

custodian: $(NATIVE_BUILD_DIR)/custodian.o $(NATIVE_BUILD_DIR)/io_socket.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BINARIES_DIR)/$@ $^

free: $(NATIVE_BUILD_DIR)/free.o $(NATIVE_BUILD_DIR)/json.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BINARIES_DIR)/$@ $^

gpu_program: $(NATIVE_BUILD_DIR)/GPU.o
	$(NVCC) $(CUDAFLAGS) -I$(INCLUDE_DIR) -o $(BINARIES_DIR)/$@ $^

bugwatcher: $(NATIVE_BUILD_DIR)/BugWatcher.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BINARIES_DIR)/$@ $^

# Compile rules for .c -> .o
$(NATIVE_BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(NATIVE_BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Compile rules for .cu -> .o
$(NATIVE_BUILD_DIR)/%.o: $(SRC_DIR)/%.cu
	@mkdir -p $(NATIVE_BUILD_DIR)
	$(NVCC) $(CUDAFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Clean build artifacts (native)
native_clean:
	@echo "Cleaning native build artifacts..."
	@rm -rf $(NATIVE_BUILD_DIR) $(BINARIES_DIR) $(LOG_DIR)
	@echo "Clean complete (native)."

# Orchestrate Health Check, Payload, and Binary Deployment
native_orchestrate: native_all
	@echo "Running orchestration script..."
	bash Orchestrate.sh > $(LOG_DIR)/orchestration.log 2>&1
	@echo "Orchestration completed."

# Deploy to system directory
native_deploy: native_all
	@echo "Deploying binaries to $(INSTALL_DIR)..."
	@sudo mkdir -p $(INSTALL_DIR) || { echo "Failed to create $(INSTALL_DIR). Check permissions."; exit 1; }
	@sudo install -m 755 $(BINARIES_DIR)/* $(INSTALL_DIR)/ || { echo "Failed to install binaries. Check permissions."; exit 1; }
	@echo "Deployment complete."

# Debug build
native_debug: CFLAGS += $(DEBUG_CFLAGS)
native_debug: CUDAFLAGS += -G -lineinfo
native_debug: native_all

# Static analysis with clang-tidy
native_static_analysis:
	@echo "Running static analysis on native code..."
	clang-tidy $(SOURCES) -- $(CFLAGS) -I$(INCLUDE_DIR) || { echo "Static analysis found issues."; exit 1; }
	@echo "Static analysis completed."

# Install dependencies (example)
native_deps:
	@echo "Installing necessary dependencies (native build)..."
	sudo apt-get update && sudo apt-get install -y libssl-dev libcrypto++-dev libboost-all-dev curl
	@echo "Dependencies installed (native)."

# Run tests (assuming run_tests.c exists)
native_test: native_all
	@echo "Running native tests..."
	$(CC) $(CFLAGS) $(SRC_DIR)/run_tests.c -o $(BINARIES_DIR)/run_tests $(LDFLAGS)
	$(BINARIES_DIR)/run_tests || { echo "Tests failed."; exit 1; }
	@echo "Tests completed."

###############################################################################
#                          CMAKE-BASED BUILD
###############################################################################

# Directory where CMake will generate its build files
CMAKE_BUILD_DIR ?= cmake_build
BUILD_TYPE      ?= Release
CMAKE_GENERATOR ?= "Unix Makefiles"

.PHONY: cmake_all cmake_configure cmake_build cmake_test cmake_install cmake_clean

# CMake "all" target
cmake_all: cmake_build

# 1) Configure
cmake_configure:
	@echo "==> [CMake] Configuring with build type: $(BUILD_TYPE)"
	mkdir -p $(CMAKE_BUILD_DIR)
	cmake -S . -B $(CMAKE_BUILD_DIR) \
	      -G $(CMAKE_GENERATOR) \
	      -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)

# 2) Build
cmake_build: cmake_configure
	@echo "==> [CMake] Building project..."
	cmake --build $(CMAKE_BUILD_DIR) --config $(BUILD_TYPE)

# 3) Test
cmake_test: cmake_build
	@echo "==> [CMake] Running ctest..."
	cd $(CMAKE_BUILD_DIR) && ctest --verbose -C $(BUILD_TYPE)

# 4) Install
cmake_install: cmake_build
	@echo "==> [CMake] Installing project..."
	cmake --build $(CMAKE_BUILD_DIR) --target install --config $(BUILD_TYPE)

# 5) Clean
cmake_clean:
	@echo "==> [CMake] Removing build artifacts from $(CMAKE_BUILD_DIR)..."
	rm -rf $(CMAKE_BUILD_DIR)

###############################################################################
#                          MASTER "ALL" and "CLEAN"
###############################################################################
# If you want a single 'make all' that does both builds:
.PHONY: all clean

all: native_all cmake_all
	@echo "Done building both native and CMake targets."

clean: native_clean cmake_clean
	@echo "Done cleaning both native and CMake builds."

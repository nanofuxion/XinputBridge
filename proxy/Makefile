# --- Compilers ---
# Default compiler for native builds (amd64 on GitHub runners)
CC_AMD64 = gcc
# Cross-compiler for ARM64
CC_ARM64 = aarch64-linux-gnu-gcc

# --- Flags ---
CFLAGS = -Wall -Wextra -pthread
LDFLAGS = -pthread

# --- Target Executables ---
TARGET_AMD64 = udp_proxy_amd64
TARGET_ARM64 = udp_proxy_arm64
SRC = proxy.c

# --- Build Targets ---

# Default target: build both architectures
all: amd64 arm64

# AMD64 target
amd64: $(TARGET_AMD64)

$(TARGET_AMD64): $(SRC)
	$(CC_AMD64) $(CFLAGS) -o $@ $(SRC) $(LDFLAGS)
	@echo "Built AMD64 target: $@"

# ARM64 target
arm64: $(TARGET_ARM64)

$(TARGET_ARM64): $(SRC)
	$(CC_ARM64) $(CFLAGS) -o $@ $(SRC) $(LDFLAGS)
	@echo "Built ARM64 target: $@"

# --- Cleanup ---
clean:
	rm -f $(TARGET_AMD64) $(TARGET_ARM64)
	@echo "Cleaned up build artifacts."
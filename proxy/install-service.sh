#!/bin/bash

# This script installs the UDP proxy as a systemd service.
# It must be run with root privileges.

# --- Configuration ---
PROXY_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
TARGET_BIN_PATH="/usr/local/bin/udp_proxy"
TARGET_SERVICE_PATH="/etc/systemd/system/proxy.service"

# --- Check for Root ---
if [ "$EUID" -ne 0 ]; then
  echo "Please run as root or with sudo."
  exit 1
fi

# --- Compile All Proxy Architectures ---
echo "Compiling the UDP proxy for all architectures..."
make -C "$PROXY_DIR" all
if [ $? -ne 0 ]; then
    echo "Compilation failed. Aborting."
    exit 1
fi
echo "Compilation successful."

# --- Detect Architecture and Select Binary ---
ARCH=$(uname -m)
SOURCE_BIN=""

if [ "$ARCH" = "x86_64" ]; then
    echo "Detected AMD64 architecture."
    SOURCE_BIN="$PROXY_DIR/udp_proxy_amd64"
elif [ "$ARCH" = "aarch64" ]; then
    echo "Detected ARM64 architecture."
    SOURCE_BIN="$PROXY_DIR/udp_proxy_arm64"
else
    echo "Unsupported architecture: $ARCH. Aborting."
    exit 1
fi

if [ ! -f "$SOURCE_BIN" ]; then
    echo "Error: Compiled binary not found at $SOURCE_BIN. Aborting."
    exit 1
fi

# --- Install Files ---
echo "Installing binary for $ARCH..."
cp "$SOURCE_BIN" "$TARGET_BIN_PATH"
if [ $? -ne 0 ]; then
    echo "Failed to copy executable to $TARGET_BIN_PATH. Aborting."
    exit 1
fi

echo "Installing systemd service file..."
cp "$PROXY_DIR/proxy.service" "$TARGET_SERVICE_PATH"
if [ $? -ne 0 ]; then
    echo "Failed to copy service file to $TARGET_SERVICE_PATH. Aborting."
    exit 1
fi
echo "Files installed successfully."

# --- Setup Systemd Service ---
echo "Setting up systemd service..."
systemctl daemon-reload
if [ $? -ne 0 ]; then
    echo "Failed to reload systemd daemon. Aborting."
    exit 1
fi

systemctl enable proxy.service
if [ $? -ne 0 ]; then
    echo "Failed to enable proxy.service. Aborting."
    exit 1
fi

systemctl start proxy.service
if [ $? -ne 0 ]; then
    echo "Failed to start proxy.service. Check 'journalctl -u proxy.service' for details."
    exit 1
fi

echo "Service 'proxy.service' has been enabled and started."
echo "Installation complete."
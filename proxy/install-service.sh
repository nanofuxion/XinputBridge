#!/bin/bash

# This script installs the UDP proxy as a systemd service.
# It must be run with root privileges from the extracted package directory.

# --- Configuration ---
# Assumes the script is run from the root of the extracted 'package' directory.
INSTALL_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
TARGET_BIN_PATH="/usr/local/bin/udp_proxy"
TARGET_SERVICE_PATH="/etc/systemd/system/proxy.service"

# --- Check for Root ---
if [ "$EUID" -ne 0 ]; then
  echo "Please run as root or with sudo."
  exit 1
fi

# --- Detect Architecture and Select Binary ---
ARCH=$(uname -m)
SOURCE_BIN=""

if [ "$ARCH" = "x86_64" ]; then
    echo "Detected AMD64 architecture."
    SOURCE_BIN="$INSTALL_DIR/amd64/udp_proxy"
elif [ "$ARCH" = "aarch64" ]; then
    echo "Detected ARM64 architecture."
    SOURCE_BIN="$INSTALL_DIR/arm64/udp_proxy"
else
    echo "Unsupported architecture: $ARCH. Aborting."
    exit 1
fi

if [ ! -f "$SOURCE_BIN" ]; then
    echo "Error: Pre-compiled binary not found at $SOURCE_BIN."
    echo "Please ensure you are running this script from the correct directory."
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
cp "$INSTALL_DIR/proxy.service" "$TARGET_SERVICE_PATH"
if [ $? -ne 0 ]; then
    echo "Failed to copy service file to $TARGET_SERVICE_PATH. Aborting."
    exit 1
fi
echo "Files installed successfully."

# --- Setup Systemd Service ---
echo "Setting up systemd service..."
# Stop the service if it's already running to ensure a clean start
systemctl stop proxy.service >/dev/null 2>&1

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
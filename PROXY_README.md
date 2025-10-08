# XinputBridge with UDP Proxy for Android Virtualization Framework (AVF)

This document explains how to use the `XinputBridge` application with the new UDP proxy, specifically for users running the desktop component within an Android Virtualization Framework (AVF) terminal.

## Architecture Overview

The new system consists of three main components:

1.  **The Android App (`XinputBridge.apk`):** This runs on your Android host device. It acts as the gamepad server. The app's main screen will now display the IP address of your phone on the `avf_tap_fixed` network interface. You will need this IP for the proxy to work.

2.  **The Xinput DLLs:** These are the original, unmodified Xinput DLLs that are loaded by your game or application inside the AVF terminal. They are designed to send gamepad data to `localhost` (127.0.0.1). **These DLLs are NOT built by this repository anymore.**

3.  **The UDP Proxy (`udp_proxy`):** This is a new, lightweight C-based application that runs in the background on your AVF terminal. It acts as a bridge between the Xinput DLLs and the Android app.
    - It listens for UDP traffic on `localhost:7947` (from the Xinput DLL).
    - It automatically discovers the IP address of your Android phone on the `avf_tap_fixed` network.
    - It forwards the gamepad data from `localhost` to your phone's IP address.
    - It forwards the response from your phone back to the Xinput DLL on `localhost`.

This architecture decouples the core Xinput functionality from the networking, making the system more modular and robust.

## How to Use

### Step 1: Get the Required Files

1.  **Android App:** Build the project or download the `app-debug.apk` from the [GitHub Actions artifacts](https://github.com/nanofuxion/XinputBridge/actions). Install it on your Android device.

2.  **UDP Proxy:** Download the `udp-proxy-package.zip` from the [GitHub Actions artifacts](https://github.com/nanofuxion/XinputBridge/actions). This zip file contains:
    - `amd64/udp_proxy`: The proxy for x86_64 terminals.
    - `arm64/udp_proxy`: The proxy for arm64 terminals.
    - `install-service.sh`: An installation script.
    - `proxy.service`: A systemd service file.

3.  **Xinput DLLs (CRITICAL):** You **must** download the pre-compiled Xinput DLLs from the original repository:
    - **[Download Xinput DLLs from Ilan12346-maya/XinputBridge](https://github.com/Ilan12346-maya/XinputBridge)**
    - Place these DLLs in the same directory as your game's executable inside the AVF terminal.

### Step 2: Setup on the AVF Terminal

1.  **Transfer Proxy Files:** Copy the contents of the `udp-proxy-package.zip` to a directory on your AVF terminal (e.g., `/home/user/proxy`).

2.  **Run the Installer:** Open a terminal in your AVF environment, navigate to the directory where you copied the files, and run the installation script. You will need root privileges.

    ```bash
    cd /home/user/proxy
    sudo ./install-service.sh
    ```

    This script automatically detects your system's architecture (e.g., `x86_64` or `aarch64`) and installs the correct pre-compiled binary from the package. It also copies the service file to `/etc/systemd/system` and enables the service to run automatically in the background.

3.  **Verify the Service (Optional):** You can check if the service is running correctly with:
    ```bash
    systemctl status proxy.service
    ```
    You can view its logs with:
    ```bash
    journalctl -u proxy.service -f
    ```

### Step 3: Run the System

1.  **Start the Android App:** Open the XinputBridge app on your phone. It should display the IP address (e.g., `IP: 10.174.15.114`).

2.  **Start Your Game:** Launch your game in the AVF terminal. The Xinput DLLs will load and start sending data to `localhost`.

3.  **Automatic Connection:** The `udp_proxy` service running on your terminal will automatically detect the packets from the DLL, discover the phone's IP address, and bridge the connection.

Your gamepad should now work in the game. The proxy will handle the network communication seamlessly in the background.
# XinputBridge Direct Connection Mode

This document explains the new direct connection feature that eliminates the need for a separate UDP proxy.

## What's New

The modified Xinput DLLs can now connect **directly** to the Android device without requiring the separate `udp_proxy` component. This simplifies deployment and reduces the number of moving parts.

## How It Works

The Xinput DLL now includes automatic IP discovery and direct connection capabilities:

1. **Environment Variable (Priority 1)**: Checks `XINPUT_BRIDGE_IP` environment variable
2. **Auto-Discovery (Priority 2)**: Scans common AVF IP ranges to find the Android device
3. **Fallback (Priority 3)**: Falls back to `127.0.0.1` (works with proxy if needed)

### Supported IP Ranges

The auto-discovery scans these common AVF network ranges:
- `10.174.15.x` (common AVF tap interface range)
- `10.0.2.x` (QEMU/KVM default range)
- `192.168.122.x` (libvirt default range)
- `192.168.210.x` (alternative AVF range)

## Usage

### Method 1: Environment Variable (Recommended)

Set the `XINPUT_BRIDGE_IP` environment variable to your Android device's IP address:

```bash
export XINPUT_BRIDGE_IP=10.174.15.114
```

Then run your game. The DLL will use this IP directly.

**Finding your Android IP:**
- Open the XinputBridge app on your Android device
- The IP is displayed on the main screen (e.g., "IP: 10.174.15.114")

### Method 2: Auto-Discovery

Simply run your game without setting the environment variable. The DLL will:
1. Scan common AVF IP ranges
2. Test connectivity to each potential IP
3. Use the first IP that responds successfully

**Note:** Auto-discovery takes a few seconds (typically 2-10 seconds depending on network).

### Method 3: Fallback to Proxy

If both methods above fail, the DLL falls back to `127.0.0.1:7947`, which works with the original proxy setup.

## Advantages

✅ **Simplified Deployment**: No need to install and manage a separate proxy service  
✅ **Faster Connection**: Direct connection eliminates proxy overhead  
✅ **More Reliable**: Fewer components means fewer points of failure  
✅ **Backward Compatible**: Still works with proxy if needed  
✅ **GitHub Actions Friendly**: Avoids complex Wine build environment issues  

## Configuration Examples

### For AVF Terminal (systemd)

Create `/etc/environment.d/xinput.conf`:
```bash
XINPUT_BRIDGE_IP=10.174.15.114
```

Or add to your `.bashrc` or `.profile`:
```bash
echo 'export XINPUT_BRIDGE_IP=10.174.15.114' >> ~/.bashrc
```

### For Wine Applications

```bash
XINPUT_BRIDGE_IP=10.174.15.114 wine your_game.exe
```

### For Proton/Steam

In Steam, set the game's launch options:
```
XINPUT_BRIDGE_IP=10.174.15.114 %command%
```

## Troubleshooting

### DLL can't find Android device

1. **Verify the Android app is running**: Open XinputBridge on your Android device
2. **Check the IP**: Note the IP shown in the app
3. **Set environment variable**: `export XINPUT_BRIDGE_IP=<your_ip>`
4. **Test connectivity**: Try pinging the IP from your terminal

### Connection is slow

- Use the environment variable method instead of auto-discovery
- Auto-discovery scans up to ~1000 IPs which takes time

### Want to use the old proxy method

Simply don't set `XINPUT_BRIDGE_IP` and ensure auto-discovery fails (or returns no results). The DLL will fall back to `127.0.0.1:7947`.

## Technical Details

### Connection Test

The DLL tests each potential IP by:
1. Creating a temporary UDP socket
2. Sending a `REQUEST_CODE_GET_GAMEPAD` packet
3. Waiting 500ms for a response
4. Considering the IP valid if a response is received

### Performance

- Environment variable: Instant connection
- Auto-discovery: 2-10 seconds (depends on which range contains the device)
- Fallback: Instant (assumes proxy is running)

## Migration Guide

### From Proxy Setup to Direct Connection

1. **Find your Android IP** (shown in the XinputBridge app)
2. **Set the environment variable**: `export XINPUT_BRIDGE_IP=<ip>`
3. **Remove the proxy service** (optional):
   ```bash
   sudo systemctl stop proxy.service
   sudo systemctl disable proxy.service
   ```
4. **Run your game** as usual

### Keeping Both Options

You can keep the proxy installed as a fallback. The DLL will:
- Try direct connection first (if environment variable is set or auto-discovery succeeds)
- Fall back to proxy if direct connection fails

This gives you the best of both worlds!


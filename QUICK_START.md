# Quick Start Guide: Direct Connection Mode

## TL;DR

Your Xinput DLL can now connect directly to Android without the proxy!

## Setup in 3 Steps

### Step 1: Find Your Android IP
Open the XinputBridge app on your Android device. The IP is displayed on screen (e.g., `IP: 10.174.15.114`)

### Step 2: Set Environment Variable
```bash
export XINPUT_BRIDGE_IP=10.174.15.114
```

Replace `10.174.15.114` with your actual Android IP.

### Step 3: Run Your Game
```bash
wine your_game.exe
```

That's it! Your gamepad should work.

---

## Three Usage Modes

### 🚀 Mode 1: Manual IP (Recommended - Fastest)
```bash
export XINPUT_BRIDGE_IP=10.174.15.114
wine your_game.exe
```
**Connection time:** Instant  
**Best for:** Daily use, stable networks

### 🔍 Mode 2: Auto-Discovery
```bash
wine your_game.exe
```
**Connection time:** 2-10 seconds  
**Best for:** Testing, dynamic IPs

### 🔌 Mode 3: Proxy Fallback
```bash
# Make sure proxy service is running
sudo systemctl start proxy.service
wine your_game.exe
```
**Connection time:** Instant (if proxy running)  
**Best for:** Troubleshooting, legacy setups

---

## Making It Permanent

### For Current Session
```bash
export XINPUT_BRIDGE_IP=10.174.15.114
```

### For All Sessions (Bash)
```bash
echo 'export XINPUT_BRIDGE_IP=10.174.15.114' >> ~/.bashrc
source ~/.bashrc
```

### For All Sessions (Zsh)
```bash
echo 'export XINPUT_BRIDGE_IP=10.174.15.114' >> ~/.zshrc
source ~/.zshrc
```

### System-Wide (AVF Terminal)
```bash
sudo mkdir -p /etc/environment.d
echo 'XINPUT_BRIDGE_IP=10.174.15.114' | sudo tee /etc/environment.d/xinput.conf
```

---

## Troubleshooting

### Problem: Can't find Android device

**Solution 1:** Set IP manually
```bash
export XINPUT_BRIDGE_IP=<your_android_ip>
```

**Solution 2:** Check Android app is running
- Open XinputBridge app
- Verify IP is displayed
- Try pinging the IP: `ping <android_ip>`

**Solution 3:** Use proxy mode
```bash
sudo systemctl start proxy.service
wine your_game.exe
```

### Problem: Connection is slow

**Cause:** Auto-discovery scanning many IPs  
**Solution:** Use Mode 1 (manual IP) instead

### Problem: IP keeps changing

**Cause:** Dynamic IP assignment  
**Solution:** Configure static IP on AVF network or update env var when it changes

---

## Comparison Chart

| Feature | Mode 1: Manual | Mode 2: Auto | Mode 3: Proxy |
|---------|---------------|--------------|---------------|
| Setup | Set env var | None | Install service |
| Speed | Instant | 2-10s | Instant |
| Maintenance | Low | None | Medium |
| Reliability | High | Medium | High |

---

## Advanced: Per-Game Configuration

### Steam Games
In game properties → Launch Options:
```
XINPUT_BRIDGE_IP=10.174.15.114 %command%
```

### Lutris Games
In game configuration → System Options → Environment Variables:
```
XINPUT_BRIDGE_IP=10.174.15.114
```

### Shell Script Wrapper
Create `run_game.sh`:
```bash
#!/bin/bash
export XINPUT_BRIDGE_IP=10.174.15.114
cd /path/to/game
wine game.exe
```

Make executable:
```bash
chmod +x run_game.sh
./run_game.sh
```

---

## Need Help?

- 📖 Full Documentation: [xinput_dev/DIRECT_CONNECTION.md](xinput_dev/DIRECT_CONNECTION.md)
- 💬 Community: [Telegram](https://t.me/+YLyovfrXSeYwMmUy)
- 🐛 Issues: [GitHub Issues](https://github.com/Ilan12346-maya/XinputBridge/issues)

---

**Pro Tip:** Add the Android IP to your notes app so you always have it handy!


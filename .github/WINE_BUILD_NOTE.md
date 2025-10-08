# Wine Build in GitHub Actions - Important Notes

## ⚠️ Build Complexity Warning

Building Wine DLLs in GitHub Actions is **extremely complex** and may fail due to:

1. **Long build time** (1-2 hours)
2. **Complex dependencies** (Ubuntu bootstraps, Wine source, MinGW)
3. **Resource constraints** (GitHub Actions runners have limited resources)
4. **Bootstrap creation** (requires debootstrap with root privileges)

## What to Expect

### First Build
- May **timeout** after 120 minutes
- May **fail** during bootstrap creation
- May **fail** during Wine configuration
- May **succeed** but take 90-120 minutes

### If Build Fails

The workflow is designed to still create releases even if the Xinput build fails:
- Android APK will still be released
- Xinput DLLs will be missing from the release

**Workaround:** Build DLLs locally and upload manually:
```bash
cd xinput_dev
./prepare.sh
./compile_xinput.sh
cd output
zip -r xinput-dlls.zip 32/ 64/
# Upload to GitHub release manually
```

## Optimization Attempts

The workflow tries to optimize by:
- ✅ Using `--depth 1` for Wine git clone
- ✅ Setting 120-minute timeout
- ✅ Installing only necessary dependencies
- ✅ Skipping unnecessary build steps

## Alternative: Use Proxy

If Wine builds are too unreliable in CI/CD, you can:

1. **Revert to proxy-based builds** (proxy is simple to compile)
2. **Build DLLs locally** and distribute separately
3. **Use pre-built DLLs** from previous releases

## Success Criteria

For the Wine build to succeed, GitHub Actions runners need:
- At least 2 CPU cores
- At least 8GB RAM
- At least 20GB disk space
- Ubuntu 22.04 or newer
- Root privileges for debootstrap

## Testing Locally

Before relying on CI/CD, test the build scripts work:

```bash
cd xinput_dev

# Create bootstraps (once)
sudo ./create_ubuntu_bootstraps.sh

# Download Wine (once per version)
cd work_temp
git clone --branch wine-9.3 --depth 1 https://github.com/wine-mirror/wine.git
cd ..

# Prepare Wine build (once per Wine version)
export BUILD_DIR="${PWD}/work_temp"
./prepare.sh

# Compile DLLs (fast after first build)
./compile_xinput.sh
```

If these work locally, they *should* work in CI/CD, but GitHub Actions environment differs from local.

## Recommendations

### For Development
- Build DLLs **locally** during development
- Test changes before committing
- Push only when ready

### For Releases
- Let CI/CD **attempt** the build
- If it fails, build locally and upload manually
- Consider caching Wine build artifacts between runs (advanced)

### For Users
- Download **Android APK** from CI/CD (always works)
- Download **Xinput DLLs** from successful builds or manual uploads
- If DLLs missing, check previous release

## Future Improvements

Potential ways to make CI/CD more reliable:

1. **Cache Wine Build**
   - Save `work_temp/build32` and `work_temp/build64`
   - Reduces subsequent builds to seconds
   
2. **Pre-built Docker Image**
   - Create Docker image with bootstraps and Wine ready
   - GitHub Actions pulls image instead of building from scratch
   
3. **Split into Multiple Jobs**
   - Job 1: Create bootstraps (1 hour, cache result)
   - Job 2: Prepare Wine (1 hour, cache result)
   - Job 3: Compile DLLs (5 minutes)
   
4. **Self-hosted Runner**
   - Run builds on your own hardware
   - More resources, better caching

## Current Status

The workflow **attempts** to build Xinput DLLs but may fail. This is **expected** and **acceptable** because:

✅ Android APK always builds successfully  
✅ Users can build DLLs locally if needed  
✅ Proxy is no longer needed (DLLs have direct connection)  
✅ Pre-built DLLs work across Wine versions  

The goal is to provide builds when possible, not to guarantee builds every time.


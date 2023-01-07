# Daisykit - Android

This is the Android demo app of [DaisyKit project](https://daisykit.org/). Visit [DaisyKit SDK](https://github.com/open-daisylab/daisykit).
## I. Build and Run

**Dependencies:** Android NDK 21.3, CMake 3.10.2.

### Step 1: Clone the source code

```
git clone --recurse-submodules https://github.com/open-daisylab/daisykit-android.git
```

Or 

```
git clone https://github.com/Daisykit-AI/daisykit-android.git
cd daisykit-android
git submodule update --init
```
### Step 2: Download NCNN

https://github.com/Tencent/ncnn/releases

- Download ncnn-YYYYMMDD-android-vulkan.zip or build ncnn for android yourself
- Extract ncnn-YYYYMMDD-android-vulkan.zip into **app/src/main/jni** and change the **ncnn_DIR** path to yours in **app/src/main/jni/CMakeLists.txt**

### Step 3: Download OpenCV

https://github.com/nihui/opencv-mobile

- Download opencv-mobile-XYZ-android.zip
- Extract opencv-mobile-XYZ-android.zip into **app/src/main/jni** and change the **OpenCV_DIR** path to yours in **app/src/main/jni/CMakeLists.txt**

### Step 4: Build

- Open this project with Android Studio, build it and enjoy!

## II. Some notes

- Android ndk camera is used for best efficiency.
- Crash may happen on very old devices for lacking HAL3 camera interface.
- All models are manually modified to accept dynamic input shape.
- Most small models run slower on GPU than on CPU, this is common.
- FPS may be lower in dark environment because of longer camera exposure time.

## III. Errors and Fixes

### 1. Errors because of build tool version

```
No toolchains found in the NDK toolchains folder for ABI with prefix: arm-linux-androideabi
```

```
clang ++: error: unknown argument: '-static-openmp'
```

**How to fix?** Install NDK 21.3, CMake 3.10.2.

### 2. Crashing app

See into assets folder at `app/src/main/assets`. There should be 3 subfolders here: `models`, `configs`, and `images`. Initialize all git submodules.

### 3. Android debug

Use above snippet to print to logcat.

```
#include <android/log.h>

#define  LOG_TAG    "testjni"
#define  ALOG(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
```

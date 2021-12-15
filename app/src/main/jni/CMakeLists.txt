project(daisykitlib)

cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fexceptions -frtti")

set(OpenCV_DIR ${CMAKE_SOURCE_DIR}/third_party/opencv-mobile-4.5.4-android/sdk/native/jni)
find_package(OpenCV REQUIRED core imgproc)

include_directories(.
        daisykitsdk/include/
        daisykitsdk/src/third_party/zxing-cpp/core/src
        daisykitsdk/src/third_party/zxing-cpp/opencv/src
        )

add_subdirectory(daisykitsdk)

add_library(daisykit_camera_lib SHARED daisykit_camera.cpp third_party/ndkcamera/ndkcamera.cpp asset_mgr_utils.cpp)
target_link_libraries(daisykit_camera_lib ${OpenCV_LIBS} camera2ndk mediandk daisykitsdk)

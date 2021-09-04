project(daisykitlib)

cmake_minimum_required(VERSION 3.10)

set(OpenCV_DIR ${CMAKE_SOURCE_DIR}/thirdparties/opencv-mobile-4.5.3-android/sdk/native/jni)
find_package(OpenCV REQUIRED core imgproc)

include_directories(. daisykitsdk/include/)

add_subdirectory(daisykitsdk)

add_library(daisykit_camera_lib SHARED daisykit_camera.cpp thirdparties/ndkcamera/ndkcamera.cpp)
target_link_libraries(daisykit_camera_lib ${OpenCV_LIBS} camera2ndk mediandk daisykitsdk)

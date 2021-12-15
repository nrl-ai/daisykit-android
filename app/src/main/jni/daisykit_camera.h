#pragma once

#include <stdio.h>
#include <algorithm>
#include <string>
#include <vector>

#include <android/asset_manager_jni.h>
#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <jni.h>

#include <opencv2/opencv.hpp>

#include "demo_app_enum.h"
#include "asset_mgr_utils.h"
#include "third_party/ndkcamera/ndkcamera.h"

#include <daisykit/common/types.h>
#include <daisykit/flows/face_detector_flow.h>
#include <daisykit/flows/background_matting_flow.h>
#include <daisykit/flows/barcode_scanner_flow.h>
#include <daisykit/flows/human_pose_movenet_flow.h>
#include <daisykit/flows/hand_pose_detector_flow.h>
#include <daisykit/flows/object_detector_flow.h>


static std::string read_file_from_assets(AAssetManager* mgr, const std::string &file_name) {
  assetistream::setAssetManager(mgr);
  assetistream as(file_name);
  std::string content(std::istreambuf_iterator<char>(as), {});
  return content;
}
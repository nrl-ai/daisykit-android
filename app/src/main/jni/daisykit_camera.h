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
#include "thirdparties/ndkcamera/ndkcamera.h"

#include <daisykitsdk/common/types.h>
#include <daisykitsdk/flows/pushup_counter_flow.h>
#include <daisykitsdk/flows/barcode_scanner_flow.h>

static std::string read_file_from_assets(AAssetManager* mgr, const std::string &file_name) {
  assetistream::setAssetManager(mgr);
  assetistream as(file_name);
  std::string content(std::istreambuf_iterator<char>(as), {});
  return content;
}
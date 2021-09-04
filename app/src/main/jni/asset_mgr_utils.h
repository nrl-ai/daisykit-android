#pragma once

#include <stdio.h>
#include <algorithm>
#include <vector>
#include <string>
#include <fstream>
#include <streambuf>

#include <android/asset_manager_jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <android/log.h>
#include <jni.h>

class asset_streambuf : public std::streambuf {
 public:
  asset_streambuf(AAssetManager* manager, const std::string& filename);
  virtual ~asset_streambuf();

  std::streambuf::int_type underflow() override;
  std::streambuf::int_type overflow(std::streambuf::int_type value) override;
  int sync() override;

 private:
  AAssetManager* manager;
  AAsset* asset;
  std::vector<char> buffer;
};

class assetistream : public std::istream {
 public:
  assetistream(AAssetManager* manager, const std::string& file);
  assetistream(const std::string& file);

  virtual ~assetistream();

  static void setAssetManager(AAssetManager* m);

 private:
  static AAssetManager* manager;
};
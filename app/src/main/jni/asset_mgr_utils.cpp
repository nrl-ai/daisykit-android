#include "asset_mgr_utils.h"

asset_streambuf::asset_streambuf(AAssetManager* manager,
                                 const std::string& filename)
    : manager(manager) {
  asset = AAssetManager_open(manager, filename.c_str(), AASSET_MODE_STREAMING);
  buffer.resize(1024);

  setg(0, 0, 0);
  setp(&buffer.front(), &buffer.front() + buffer.size());
}

asset_streambuf::~asset_streambuf() {
  sync();
  AAsset_close(asset);
}

std::streambuf::int_type asset_streambuf::underflow() {
  auto bufferPtr = &buffer.front();
  auto counter = AAsset_read(asset, bufferPtr, buffer.size());

  if (counter == 0) return traits_type::eof();
  if (counter < 0)  // error, what to do now?
    return traits_type::eof();

  setg(bufferPtr, bufferPtr, bufferPtr + counter);

  return traits_type::to_int_type(*gptr());
}

std::streambuf::int_type asset_streambuf::overflow(
    std::streambuf::int_type value) {
  return traits_type::eof();
};

int asset_streambuf::sync() {
  std::streambuf::int_type result = overflow(traits_type::eof());

  return traits_type::eq_int_type(result, traits_type::eof()) ? -1 : 0;
}

assetistream::assetistream(AAssetManager* manager, const std::string& file)
    : std::istream(new asset_streambuf(manager, file)) {}

assetistream::assetistream(const std::string& file)
    : std::istream(new asset_streambuf(manager, file)) {}

assetistream::~assetistream() { delete rdbuf(); }

void assetistream::setAssetManager(AAssetManager* m) { manager = m; }

AAssetManager* assetistream::manager;

cv::Mat ReadCVMatFromAsset(AAssetManager* mgr, const std::string& file_path) {
  AAsset* asset_file = AAssetManager_open(mgr, file_path.c_str(), AASSET_MODE_BUFFER);
  size_t file_length = AAsset_getLength(asset_file);
  char* data_buffer2 = (char*)malloc(file_length);
  // Read file data
  AAsset_read(asset_file, data_buffer2, file_length);
  // The data has been copied to dataBuffer2, so , close it
  AAsset_close(asset_file);

  // Decode the file data to cv::Mat
  std::vector<char> vec2(data_buffer2, data_buffer2 + file_length);
  cv::Mat mat2 = cv::imdecode(vec2, CV_LOAD_IMAGE_COLOR);

  return mat2;
}
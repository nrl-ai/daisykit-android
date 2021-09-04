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
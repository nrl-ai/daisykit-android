#include "daisykit_camera.h"

ncnn::Mutex g_lock;
static DemoApp current_demo = DemoApp::kDemoFaceDetector;
static daisykit::flows::FaceDetectorFlow* face_detector_flow;
static daisykit::flows::BackgroundMattingFlow* background_matting_flow;
static daisykit::flows::BarcodeScannerFlow* barcode_scanner_flow;
static daisykit::flows::HumanPoseMoveNetFlow* human_pose_movenet_flow;
static daisykit::flows::HandPoseDetectorFlow* hand_pose_detector_flow;
static daisykit::flows::ObjectDetectorFlow* object_detector_flow;

class DaisykitCam : public NdkCameraWindow {
 public:
  virtual void on_image_render(cv::Mat& rgb) const;
};

void DaisykitCam::on_image_render(cv::Mat& rgb) const {
  {
    ncnn::MutexLockGuard g(g_lock);
    switch (current_demo) {
      case DemoApp::kDemoFaceDetector: {
        if (!face_detector_flow) return;
        auto face_detector_result = face_detector_flow->Process(rgb);
        face_detector_flow->DrawResult(rgb, face_detector_result);
        break;
      }
      case DemoApp::kDemoBackgroundMatting: {
        if (!background_matting_flow) return;
        auto mask = background_matting_flow->Process(rgb);
        background_matting_flow->DrawResult(rgb, mask);
        break;
      }
      case DemoApp::kDemoBarcodeScanner: {
        if (!barcode_scanner_flow) return;
        auto barcode_result = barcode_scanner_flow->Process(rgb, true);
        break;
      }
      case DemoApp::kDemoHumanPoseMoveNet: {
        if (!human_pose_movenet_flow) return;
        auto human_poses = human_pose_movenet_flow->Process(rgb);
        human_pose_movenet_flow->DrawResult(rgb, human_poses);
        break;
      }
      case DemoApp::kDemoHandPoseDetector: {
        if (!hand_pose_detector_flow) return;
        auto hand_poses = hand_pose_detector_flow->Process(rgb);
        hand_pose_detector_flow->DrawResult(rgb, hand_poses);
        break;
      }
      case DemoApp::kDemoObjectDetector: {
        if (!object_detector_flow) return;
        auto objects = object_detector_flow->Process(rgb);
        object_detector_flow->DrawResult(rgb, objects);
        break;
      }
    }
  }
}

static DaisykitCam* g_camera = nullptr;

extern "C" {

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
  __android_log_print(ANDROID_LOG_DEBUG, "ncnn", "JNI_OnLoad");

  g_camera = new DaisykitCam;

  return JNI_VERSION_1_4;
}

JNIEXPORT void JNI_OnUnload(JavaVM* vm, void* reserved) {
  __android_log_print(ANDROID_LOG_DEBUG, "ncnn", "JNI_OnUnload");

  {
    ncnn::MutexLockGuard g(g_lock);
    delete face_detector_flow;
    face_detector_flow = nullptr;
    delete background_matting_flow;
    background_matting_flow = nullptr;
    delete barcode_scanner_flow;
    barcode_scanner_flow = nullptr;
    delete human_pose_movenet_flow;
    human_pose_movenet_flow = nullptr;
    delete hand_pose_detector_flow;
    hand_pose_detector_flow = nullptr;
    delete object_detector_flow;
    object_detector_flow = nullptr;
  }

  delete g_camera;
  g_camera = nullptr;
}

// public native boolean loadModel(AssetManager mgr, int demoid, int cpugpu);
JNIEXPORT jboolean JNICALL Java_com_daisykitai_daisykit_DaisykitCamera_loadDemo(
    JNIEnv* env, jobject thiz, jobject assetManager, jint demoid, jint cpugpu) {
  AAssetManager* mgr = AAssetManager_fromJava(env, assetManager);

  {
    ncnn::MutexLockGuard g(g_lock);
    current_demo = static_cast<DemoApp>(demoid);
    std::string config;
    switch (current_demo) {
      case DemoApp::kDemoFaceDetector: {
        config =
            read_file_from_assets(mgr, "configs/face_detector_config.json");
        face_detector_flow = new daisykit::flows::FaceDetectorFlow(mgr, config);
        break;
      }
      case DemoApp::kDemoBackgroundMatting: {
        cv::Mat background = ReadCVMatFromAsset(mgr, "images/background.jpg");
        config = read_file_from_assets(
            mgr, "configs/background_matting_config.json");
        background_matting_flow =
            new daisykit::flows::BackgroundMattingFlow(mgr, config, background);
        break;
      }
      case DemoApp::kDemoBarcodeScanner: {
        config =
            read_file_from_assets(mgr, "configs/barcode_scanner_config.json");
        barcode_scanner_flow =
            new daisykit::flows::BarcodeScannerFlow(config);
        break;
      }
      case DemoApp::kDemoHumanPoseMoveNet: {
        config = read_file_from_assets(
            mgr, "configs/human_pose_movenet_config.json");
        human_pose_movenet_flow =
            new daisykit::flows::HumanPoseMoveNetFlow(mgr, config);
        break;
      }
      case DemoApp::kDemoHandPoseDetector: {
        config = read_file_from_assets(
            mgr, "configs/hand_pose_yolox_mp_config.json");
        hand_pose_detector_flow =
            new daisykit::flows::HandPoseDetectorFlow(mgr, config);
        break;
      }
      case DemoApp::kDemoObjectDetector: {
        config = read_file_from_assets(
            mgr, "configs/object_detector_yolox_config.json");
        object_detector_flow =
            new daisykit::flows::ObjectDetectorFlow(mgr, config);
        break;
      }
    }
  }

  return JNI_TRUE;
}

// public native boolean openCamera(int facing);
JNIEXPORT jboolean JNICALL
Java_com_daisykitai_daisykit_DaisykitCamera_openCamera(JNIEnv* env,
                                                       jobject thiz,
                                                       jint facing) {
  if (facing < 0 || facing > 1) return JNI_FALSE;

  __android_log_print(ANDROID_LOG_DEBUG, "ncnn", "openCamera %d", facing);

  g_camera->open((int)facing);

  return JNI_TRUE;
}

// public native boolean closeCamera();
JNIEXPORT jboolean JNICALL
Java_com_daisykitai_daisykit_DaisykitCamera_closeCamera(JNIEnv* env,
                                                        jobject thiz) {
  __android_log_print(ANDROID_LOG_DEBUG, "ncnn", "closeCamera");

  g_camera->close();

  return JNI_TRUE;
}

// public native boolean setOutputWindow(Surface surface);
JNIEXPORT jboolean JNICALL
Java_com_daisykitai_daisykit_DaisykitCamera_setOutputWindow(JNIEnv* env,
                                                            jobject thiz,
                                                            jobject surface) {
  ANativeWindow* win = ANativeWindow_fromSurface(env, surface);

  __android_log_print(ANDROID_LOG_DEBUG, "ncnn", "setOutputWindow %p", win);

  g_camera->set_window(win);

  return JNI_TRUE;
}
}

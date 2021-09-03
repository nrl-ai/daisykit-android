#include "daisykit_camera.h"

ncnn::Mutex g_lock;
static BodyDetector* body_detector = 0;
static PoseDetector* pose_detector = 0;
static ActionClassifier* action_classifier = 0;
static PushupAnalyzer* pushup_analyzer = 0;

class DaisykitCam : public NdkCameraWindow {
 public:
  virtual void on_image_render(cv::Mat& rgb) const;
};

void DaisykitCam::on_image_render(cv::Mat& rgb) const {
  {
    ncnn::MutexLockGuard g(g_lock);
    if (body_detector && pose_detector && action_classifier &&
        pushup_analyzer) {
      // Detect human pose
      std::vector<Object> bodies = body_detector->detect(rgb);

      // Detect keypoints
      std::vector<std::vector<Keypoint>> keypoints =
          pose_detector->detect_multi(rgb, bodies);

      // Recognize action and count pushups
      float is_pushup_score;
      Action action = action_classifier->classify(rgb, is_pushup_score);
      bool is_pushup = action == Action::kPushup;
      int n_pushups = pushup_analyzer->count_pushups(rgb, is_pushup);

      // Draw result
      for (auto body : bodies) {
        cv::rectangle(rgb, cv::Rect(body.x, body.y, body.w, body.h),
                      cv::Scalar(0, 255, 0), 2);
      }
      for (auto kp_single : keypoints) {
        pose_detector->draw_pose(rgb, kp_single);
      }
      if (action == Action::kPushup) {
        VizUtils::draw_label(
            rgb, "Is pushing: " + std::to_string(is_pushup_score),
            cv::Point(20, 40), cv::FONT_HERSHEY_SIMPLEX, 0.8, 2, 10,
            cv::Scalar(0, 0, 0), cv::Scalar(0, 255, 0));
      } else {
        VizUtils::draw_label(
            rgb, "Is pushing: " + std::to_string(is_pushup_score),
            cv::Point(20, 40), cv::FONT_HERSHEY_SIMPLEX, 0.8, 2, 10,
            cv::Scalar(0, 0, 0), cv::Scalar(220, 220, 220));
      }
      VizUtils::draw_label(
            rgb, std::string("PushUps: ") + std::to_string(n_pushups),
            cv::Point(20, 80), cv::FONT_HERSHEY_SIMPLEX, 1.2, 2, 10,
            cv::Scalar(255, 255, 255), cv::Scalar(255, 0, 0));

    } else {
      draw_unsupported(rgb);
    }
  }
}

static DaisykitCam* g_camera = 0;

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
    delete pose_detector;
    pose_detector = 0;
    delete body_detector;
    body_detector = 0;
    delete action_classifier;
    action_classifier = 0;
    delete pushup_analyzer;
    pushup_analyzer = 0;
  }

  delete g_camera;
  g_camera = 0;
}

// public native boolean loadModel(AssetManager mgr, int modelid, int cpugpu);
JNIEXPORT jboolean JNICALL Java_com_vnopenai_daisykit_DaisykitCamera_loadModel(
    JNIEnv* env, jobject thiz, jobject assetManager, jint modelid,
    jint cpugpu) {
  AAssetManager* mgr = AAssetManager_fromJava(env, assetManager);

  {
    ncnn::MutexLockGuard g(g_lock);
    body_detector =
        new BodyDetector(mgr, "person_detector.param", "person_detector.bin");
    pose_detector = new PoseDetector(mgr, "Ultralight-Nano-SimplePose.param",
                                     "Ultralight-Nano-SimplePose.bin");
    action_classifier = new ActionClassifier(mgr, "action_classifier.param",
                                             "action_classifier.bin", true);
    pushup_analyzer = new PushupAnalyzer();
  }

  return JNI_TRUE;
}

// public native boolean openCamera(int facing);
JNIEXPORT jboolean JNICALL Java_com_vnopenai_daisykit_DaisykitCamera_openCamera(
    JNIEnv* env, jobject thiz, jint facing) {
  if (facing < 0 || facing > 1) return JNI_FALSE;

  __android_log_print(ANDROID_LOG_DEBUG, "ncnn", "openCamera %d", facing);

  g_camera->open((int)facing);

  return JNI_TRUE;
}

// public native boolean closeCamera();
JNIEXPORT jboolean JNICALL
Java_com_vnopenai_daisykit_DaisykitCamera_closeCamera(JNIEnv* env,
                                                      jobject thiz) {
  __android_log_print(ANDROID_LOG_DEBUG, "ncnn", "closeCamera");

  g_camera->close();

  return JNI_TRUE;
}

// public native boolean setOutputWindow(Surface surface);
JNIEXPORT jboolean JNICALL
Java_com_vnopenai_daisykit_DaisykitCamera_setOutputWindow(JNIEnv* env,
                                                          jobject thiz,
                                                          jobject surface) {
  ANativeWindow* win = ANativeWindow_fromSurface(env, surface);

  __android_log_print(ANDROID_LOG_DEBUG, "ncnn", "setOutputWindow %p", win);

  g_camera->set_window(win);

  return JNI_TRUE;
}
}

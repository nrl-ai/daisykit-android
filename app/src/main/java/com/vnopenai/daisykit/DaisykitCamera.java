package com.vnopenai.daisykit;

import android.content.res.AssetManager;
import android.view.Surface;

public class DaisykitCamera
{
    public native boolean loadModel(AssetManager mgr, int modelid, int cpugpu);
    public native boolean openCamera(int facing);
    public native boolean closeCamera();
    public native boolean setOutputWindow(Surface surface);

    static {
        System.loadLibrary("daisykit_camera_lib");
    }
}

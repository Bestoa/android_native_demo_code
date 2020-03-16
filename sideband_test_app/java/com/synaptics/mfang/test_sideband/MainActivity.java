package com.synaptics.mfang.test_sideband;

import android.app.Activity;
import android.os.Bundle;
import android.view.Surface;
import android.view.SurfaceView;
import android.view.SurfaceHolder;
import android.util.Log;

public class MainActivity extends Activity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("jni_SidebandHandle");
    }

    SurfaceView mSurfaceView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        mSurfaceView = (SurfaceView) findViewById(R.id.sample_surface_view);
        mSurfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                Log.e("++++++", "SurfaceView created");
                setSideband(mSurfaceView.getHolder().getSurface(), true);
            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {
                Log.e("++++++", "SurfaceView destroyed");
                setSideband(mSurfaceView.getHolder().getSurface(), false);
            }
        });
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native void setSideband(Surface sf, boolean on);
}

package com.fryit.androidtelemeter;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.LinkedList;
import java.util.Scanner;

import android.R.integer;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.hardware.Camera;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup.LayoutParams;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ImageButton;
import android.widget.LinearLayout;
import android.widget.Toast;

import com.fryit.androidtelemeter.jni.Processor;
import com.fryit.androidtelemeter.jni.cvcamera;
import com.opencv.camera.CameraConfig;
import com.opencv.camera.NativePreviewer;
import com.opencv.camera.NativeProcessor;
import com.opencv.camera.NativeProcessor.PoolCallback;
import com.opencv.jni.image_pool;
import com.opencv.opengl.GL2CameraViewer;

public class TelemeterActivity extends Activity {
	
	private int REFERENCE_LOCATION_X;
	private int REFERENCE_LOCATION_Y;
	private int OBJECT_TO_MEASURE_X;
	private int OBJECT_TO_MEASURE_Y;
	private LinkedList<PoolCallback> defaultcallbackstack = new LinkedList<PoolCallback>();
	public boolean IsPreviewStopped;

	void toasts(int id) {
		switch (id) {
//		case DIALOG_OPENING_TUTORIAL:
//			Toast.makeText(this, "Try clicking the menu for CV options.",
//					Toast.LENGTH_LONG).show();
//			break;
		default:
			break;
		}

	}
	
	/**
	 * Avoid that the screen get's turned off by the system.
	 */
	public void disableScreenTurnOff() {
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON,
				WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
	}

	/**
	 * Set's the orientation to landscape, as this is needed by AndAR.
	 */
	public void setOrientation() {
		setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
	}

	/**
	 * Maximize the application.
	 */
	public void setFullscreen() {
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
				WindowManager.LayoutParams.FLAG_FULLSCREEN);
	}

	public void setNoTitle() {
		requestWindowFeature(Window.FEATURE_NO_TITLE);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		menu.add("Settings");
		return true;
	}

	private NativePreviewer mPreview;
	private GL2CameraViewer glview;

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		LinkedList<PoolCallback> defaultcallbackstack = new LinkedList<PoolCallback>();
		defaultcallbackstack.addFirst(glview.getDrawCallback());
		
		if (item.getTitle().equals("Settings")) {

			Intent intent = new Intent(this,CameraConfig.class);
			startActivity(intent);
		}

		mPreview.addCallbackStack(defaultcallbackstack);
		
		return true;
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		setFullscreen();
		disableScreenTurnOff();

		FrameLayout frame = new FrameLayout(this);

		// Create our Preview view and set it as the content of our activity.
		mPreview = new NativePreviewer(getApplication(), 640, 480);

		LayoutParams params = new LayoutParams(LayoutParams.WRAP_CONTENT,
				LayoutParams.WRAP_CONTENT);
		params.height = getWindowManager().getDefaultDisplay().getHeight();
		params.width = (int) (params.height * 4.0 / 2.88);

		LinearLayout vidlay = new LinearLayout(getApplication());

		vidlay.setGravity(Gravity.CENTER);
		vidlay.addView(mPreview, params);
		frame.addView(vidlay);

		// make the glview overlay ontop of video preview
		mPreview.setZOrderMediaOverlay(false);

		mPreview.setOnTouchListener(new View.OnTouchListener() {
			
			@Override
			public boolean onTouch(View v, MotionEvent event) {
				
				if(REFERENCE_LOCATION_X == 0 && REFERENCE_LOCATION_Y == 0)
				{
					REFERENCE_LOCATION_X = (int) event.getX();
					REFERENCE_LOCATION_Y = (int) event.getY();
				}
				else if(OBJECT_TO_MEASURE_X == 0 && OBJECT_TO_MEASURE_Y == 0){
					OBJECT_TO_MEASURE_X = (int) event.getX();
					OBJECT_TO_MEASURE_Y = (int) event.getY();

					Toast.makeText(TelemeterActivity.this,"First: " + REFERENCE_LOCATION_X + ", " + REFERENCE_LOCATION_Y + "// Second: "+ OBJECT_TO_MEASURE_X + ", " + OBJECT_TO_MEASURE_Y,Toast.LENGTH_SHORT).show();
					
					defaultcallbackstack.addFirst(glview.getDrawCallback());
					defaultcallbackstack.addFirst(new MeasureAndDisplayProcessor()); //Launch MeasureAndDisplayProcessor
					mPreview.addCallbackStack(defaultcallbackstack);
					mPreview.ProcessorPost();
				}
				else
					Toast.makeText(TelemeterActivity.this,"Location already defined, Reference: " + REFERENCE_LOCATION_X + ", " + REFERENCE_LOCATION_Y + "// Object To Measure: "+ OBJECT_TO_MEASURE_X + ", " + OBJECT_TO_MEASURE_Y,Toast.LENGTH_SHORT).show();
				
				return false;
			}
		});
		
		glview = new GL2CameraViewer(getApplication(), false, 0, 0);
		glview.setZOrderMediaOverlay(true);

		LinearLayout gllay = new LinearLayout(getApplication());

		gllay.setGravity(Gravity.CENTER);
		gllay.addView(glview, params);
		frame.addView(gllay);

		//Button Capture
		ImageButton capture_button = new ImageButton(getApplicationContext());
		capture_button.setImageDrawable(getResources().getDrawable(
				android.R.drawable.ic_menu_camera));
		capture_button.setLayoutParams(new LayoutParams(
				LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT));
		capture_button.setOnClickListener(new View.OnClickListener() {

			@Override
			public void onClick(View v) {
				
				LayoutParams params = new LayoutParams(LayoutParams.WRAP_CONTENT,
						LayoutParams.WRAP_CONTENT);

				if(IsPreviewStopped == false)
				{
					mPreview.PausePreview();
					IsPreviewStopped = true;
				} else
				{
					mPreview.surfaceChanged(mPreview.GetSurfaceHolder(),1,params.width,params.height);
					IsPreviewStopped = false;
				}
				
				Toast.makeText(TelemeterActivity.this,"IsPreviewStopped: " + IsPreviewStopped,Toast.LENGTH_SHORT).show();
				//Toast.makeText(TelemeterActivity.this,"Please click on the object to measure",Toast.LENGTH_SHORT).show();
				
			}
		});
		
		LinearLayout buttons = new LinearLayout(getApplicationContext());
		buttons.setLayoutParams(new LayoutParams(LayoutParams.WRAP_CONTENT,
				LayoutParams.WRAP_CONTENT));

		buttons.addView(capture_button);
		
		//Button Focus
		Button focus_button = new Button(getApplicationContext());
		focus_button.setLayoutParams(new LayoutParams(
				LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT));
		focus_button.setText("Reset");
		focus_button.setOnClickListener(new View.OnClickListener() {

			@Override
			public void onClick(View v) {
				reset();
			}
		});
		buttons.addView(focus_button);

		frame.addView(buttons);
		setContentView(frame);
		//toasts(DIALOG_OPENING_TUTORIAL);
	}

	public void reset ()
	{
		REFERENCE_LOCATION_X = 0;
		REFERENCE_LOCATION_Y = 0;
		OBJECT_TO_MEASURE_X = 0;
		OBJECT_TO_MEASURE_Y = 0;
		if(!defaultcallbackstack.isEmpty())
			defaultcallbackstack.remove();

	}
	
	@Override
	protected void onPause() {
		super.onPause();

		// clears the callback stack
		mPreview.onPause();

		glview.onPause();

	}

	@Override
	protected void onResume() {
		super.onResume();
		glview.onResume();
		mPreview.setParamsFromPrefs(getApplicationContext());
		// add an initiall callback stack to the preview on resume...
		// this one will just draw the frames to opengl
		LinkedList<NativeProcessor.PoolCallback> cbstack = new LinkedList<PoolCallback>();
		cbstack.add(glview.getDrawCallback());
		mPreview.addCallbackStack(cbstack);
		mPreview.onResume();
	}

	// final processor so that these processor callbacks can access it
	final Processor processor = new Processor();

	class MeasureAndDisplayProcessor implements NativeProcessor.PoolCallback {

		@Override
		public void process(int idx, image_pool pool, long timestamp,
				NativeProcessor nativeProcessor) {
			processor.MeasureAndDisplay(idx, pool, REFERENCE_LOCATION_X, REFERENCE_LOCATION_Y, OBJECT_TO_MEASURE_X, OBJECT_TO_MEASURE_Y);
		}

	}
}

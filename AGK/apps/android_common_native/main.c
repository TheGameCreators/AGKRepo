/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

//BEGIN_INCLUDE(all)
#include <unistd.h>
#include <jni.h>
#include <errno.h>

#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>
#include <android/configuration.h>
#include <android/window.h>

#include <stdlib.h>
#include <string.h>

int getModel( ANativeActivity* pActivity, char *szOut );
int checkformat( ANativeActivity* pActivity );
void init(void *ptr);
void updateptr(void *ptr);
void clearptr(void *ptr);
void windowclosing();
int gettextstate();
void begin();
void loop();
int getinternaldata( int index );
void updatesize();
void setaccel( float x, float y, float z );
void touchdown( int id, float x, float y );
void touchmoved( int id, float x, float y );
void touchup( int id, float x, float y );
void mousedown( int id );
void mousemoved( int id, float x, float y );
void mouseup( int id );
void joystickaxis( int id, int stick, int axis, float value );
void joystickbutton( int id, int button, int state );
void keyboardmode( int mode );
void keydown( int key );
void keyup( int key );
void cleanup();
void pauseapp();
void resumeapp(); 
void resumeapp2();
void onstart(void* ptr);
void exit(int);
void reset();
void devicerotate();
void setgyro( float x, float y, float z );
void setproximity( float dist );
void setlight( float light );
void setmagnetic( float x, float y, float z );
void setrotvector( float x, float y, float z, float w );
float getagktimer();

void logInput( int action, int pointer, int source, int device, int pointers, int id, int flags, float x, float y );

int windowHeight = 0;
int windowWidth = 0;

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "native-activity", __VA_ARGS__))


/**
 * Shared state for our app.
 */
struct engine {
    struct android_app* app;

    ASensorManager* sensorManager;
    const ASensor* accelerometerSensor;
	const ASensor* gyroSensor;
    const ASensor* proximitySensor;
    const ASensor* lightSensor;
    const ASensor* magneticSensor;
    const ASensor* rotVectorSensor;
    ASensorEventQueue* sensorEventQueue;

    int animating;
    int32_t width;
    int32_t height;
};

struct initdata {
    struct ANativeActivity* activity;
    ANativeWindow* window;
};

extern float AMotionEvent_getAxisValue(const AInputEvent* motion_event, int32_t axis, size_t pointer_index);
static typeof(AMotionEvent_getAxisValue) *p_AMotionEvent_getAxisValue;
#define AMotionEvent_getAxisValue (*p_AMotionEvent_getAxisValue)
#include <dlfcn.h>

static int done = 0;
static int app_mode = -2;
static int g_iDisableLightProxSensor = 0;
static int rotationID = 0;

/**
 * Initialize an EGL context for the current display.
 */
static int engine_init_display(struct engine* engine) 
{        
    struct initdata data;
    data.activity = engine->app->activity;
    data.window = engine->app->window;

	windowWidth = ANativeWindow_getWidth( engine->app->window );
	windowHeight = ANativeWindow_getHeight( engine->app->window );

    if ( done != 0 )
    {
    	LOGI( "Updating" );
		updateptr( &data );
    	updatesize();
    }
    else
    {
    	LOGI( "Initialising" );
    	init( &data );
    }

    p_AMotionEvent_getAxisValue = dlsym(RTLD_DEFAULT, "AMotionEvent_getAxisValue");

    AConfiguration *config2 = AConfiguration_new();
	AConfiguration_fromAssetManager( config2, engine->app->activity->assetManager);
	int keyboard = AConfiguration_getKeyboard( config2 );
	AConfiguration_delete( config2 );

	if ( keyboard == ACONFIGURATION_KEYBOARD_QWERTY ) keyboardmode( 1 ); // physical
	else keyboardmode( 2 ); // virtual

    done = 1;
    return 0;
}

/**
 * Just the current frame in the display.
 */
static void engine_draw_frame(struct engine* engine) 
{
    if (engine->app->window == NULL) return;

	int width = ANativeWindow_getWidth( engine->app->window );
	int height = ANativeWindow_getHeight( engine->app->window );

	if ( (width != windowWidth || height != windowHeight) && width > 0 && height > 0 )
	{
		LOGI( "Detected window size change: %dx%d -> %dx%d", windowWidth, windowHeight, width, height );
		windowWidth = width;
		windowHeight = height;

		updatesize();
	}

    loop();

    if ( getinternaldata(4) == 1 && engine->accelerometerSensor == 0 )
	{
		engine->accelerometerSensor = ASensorManager_getDefaultSensor(engine->sensorManager, ASENSOR_TYPE_ACCELEROMETER);
		if ( engine->accelerometerSensor != NULL )
		{
			ASensorEventQueue_enableSensor(engine->sensorEventQueue, engine->accelerometerSensor);
			int minRate = ASensor_getMinDelay(engine->accelerometerSensor);
			ASensorEventQueue_setEventRate(engine->sensorEventQueue, engine->accelerometerSensor, minRate);
		}
	}

	if ( getinternaldata(5) == 1 && engine->gyroSensor == 0 )
	{
		engine->gyroSensor = ASensorManager_getDefaultSensor(engine->sensorManager, ASENSOR_TYPE_GYROSCOPE);
		if (engine->gyroSensor != NULL) 
		{
			ASensorEventQueue_enableSensor(engine->sensorEventQueue, engine->gyroSensor);
			int minRate = ASensor_getMinDelay(engine->gyroSensor);
			ASensorEventQueue_setEventRate(engine->sensorEventQueue, engine->gyroSensor, minRate);
		}
	}

	if ( !g_iDisableLightProxSensor )
	{
		if ( getinternaldata(6) == 1 && engine->proximitySensor == 0 )
		{
			engine->proximitySensor = ASensorManager_getDefaultSensor(engine->sensorManager, ASENSOR_TYPE_PROXIMITY);
			if (engine->proximitySensor != NULL) 
			{
				ASensorEventQueue_enableSensor(engine->sensorEventQueue, engine->proximitySensor);
				int minRate = ASensor_getMinDelay(engine->proximitySensor);
				ASensorEventQueue_setEventRate(engine->sensorEventQueue, engine->proximitySensor, minRate);
			}
		}

		if ( getinternaldata(7) == 1 && engine->lightSensor == 0 )
		{
			engine->lightSensor = ASensorManager_getDefaultSensor(engine->sensorManager, ASENSOR_TYPE_LIGHT);
			if (engine->lightSensor != NULL) 
			{
				ASensorEventQueue_enableSensor(engine->sensorEventQueue, engine->lightSensor);
				int minRate = ASensor_getMinDelay(engine->lightSensor);
				ASensorEventQueue_setEventRate(engine->sensorEventQueue, engine->lightSensor, minRate);
			}
		}
	}

	if ( getinternaldata(8) == 1 && engine->magneticSensor == 0 )
	{
		engine->magneticSensor = ASensorManager_getDefaultSensor(engine->sensorManager, ASENSOR_TYPE_MAGNETIC_FIELD);
		if (engine->magneticSensor != NULL) 
		{
			ASensorEventQueue_enableSensor(engine->sensorEventQueue, engine->magneticSensor);
			int minRate = ASensor_getMinDelay(engine->magneticSensor);
			ASensorEventQueue_setEventRate(engine->sensorEventQueue, engine->magneticSensor, minRate);
		}
	}

	if ( getinternaldata(9) == 1 && engine->rotVectorSensor == 0 )
	{	  
		rotationID = 15;
		engine->rotVectorSensor = ASensorManager_getDefaultSensor(engine->sensorManager, 15); // ASENSOR_TYPE_GAME_ROTATION_VECTOR
		if ( engine->rotVectorSensor == 0 ) 
		{
			LOGW("Failed to get sensor 15, trying sensor 11");
			rotationID = 11;
			engine->rotVectorSensor = ASensorManager_getDefaultSensor(engine->sensorManager, 11); // ASENSOR_TYPE_ROTATION_VECTOR
		}
		if (engine->rotVectorSensor != NULL) 
		{
			ASensorEventQueue_enableSensor(engine->sensorEventQueue, engine->rotVectorSensor);
			int minRate = ASensor_getMinDelay(engine->rotVectorSensor);
			ASensorEventQueue_setEventRate(engine->sensorEventQueue, engine->rotVectorSensor, minRate);
		}
	}
}

/**
 * Tear down the EGL context currently associated with the display.
 */
static void engine_term_display(struct engine* engine)
{
    windowclosing();
    engine->animating = 0;
}

int initialised = 0;

/**
 * Process the next input event.
 */
enum {AXIS_X = 0, AXIS_Y = 1, AXIS_RX = 11, AXIS_RY = 14, AXIS_DPADX = 15, AXIS_DPADY = 16, AXIS_LT = 17, AXIS_RT = 18};

static int32_t engine_handle_input(struct android_app* app, AInputEvent* event)
{
	if ( initialised == 0 ) return 0;

	if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
	{
		// touch event

		int action = AMotionEvent_getAction( event ) & 0xff;
		int pointer = (AMotionEvent_getAction( event ) >> 8) & 0xff;
		int flags = AMotionEvent_getFlags( event );
		int pointers = AMotionEvent_getPointerCount( event );
		int id = AMotionEvent_getPointerId( event, pointer );
		int device = AInputEvent_getDeviceId( event );
		int source = AInputEvent_getSource( event );

		float x = AMotionEvent_getX( event, pointer );
		float y = AMotionEvent_getY( event, pointer );
		float size = AMotionEvent_getSize( event, pointer );
		float pressure = AMotionEvent_getPressure( event, pointer );
		//logInput( action, pointer, source, device, pointers, id, flags, x, y );
		//LOGI("Action: %d, pointer: %d, source: %d, device: %d, count: %d, id: %d, flags: %d, X: %.1f, Y: %.1f", action, pointer, source, device, pointers, id, flags, x, y);
		//if ( action != 2 ) LOGI("Action: %d, pointer: %d, count: %d, id: %d", action, pointer, pointers, id);
		//else LOGI("Pressure: %f, Size: %f, x: %f, y: %f, id: %d", pressure, size, x, y, id);

		switch ( action )
		{
			case 0:
			case 5:
				{
					if ( (source & AINPUT_SOURCE_MOUSE) == AINPUT_SOURCE_MOUSE ) mousedown( 0 );
					else if ( (source & AINPUT_SOURCE_TOUCHSCREEN) == AINPUT_SOURCE_TOUCHSCREEN ) touchdown( id, x, y );
					break;
				}
			case 1:
			case 3:
			case 6:
				{
					if ( (source & AINPUT_SOURCE_MOUSE) == AINPUT_SOURCE_MOUSE ) mouseup( 0 );
					else if ( (source & AINPUT_SOURCE_TOUCHSCREEN) == AINPUT_SOURCE_TOUCHSCREEN ) touchup( id, x, y );
					break;
				}
			case 2:
				{
					if ( (source & AINPUT_SOURCE_TOUCHSCREEN) == AINPUT_SOURCE_TOUCHSCREEN )
					{
						// touch screen
						// only the primary pointer sends move messages so get the other pointer positions while we are here
						int i = 0;
						for ( i = 0; i < pointers; i++ )
						{
							int id = AMotionEvent_getPointerId( event, i );
							x = AMotionEvent_getX( event, i );
							y = AMotionEvent_getY( event, i );
							touchmoved( id, x, y );
						}
					}
					else if ( (source & AINPUT_SOURCE_MOUSE) == AINPUT_SOURCE_MOUSE ) 
					{
						// mouse
						mousemoved( 0, x, y );
					}
					else
					{
						// joystick?
						float lx = AMotionEvent_getAxisValue(event, AXIS_X, pointer);
						float ly = AMotionEvent_getAxisValue(event, AXIS_Y, pointer);
						float rx = AMotionEvent_getAxisValue(event, AXIS_RX, pointer);
						float ry = AMotionEvent_getAxisValue(event, AXIS_RY, pointer);
						float lt = AMotionEvent_getAxisValue(event, AXIS_LT, pointer);
						float rt = AMotionEvent_getAxisValue(event, AXIS_RT, pointer);
						float lt2 = AMotionEvent_getAxisValue(event, 23, pointer); // Amazon controller
						float rt2 = AMotionEvent_getAxisValue(event, 22, pointer); // Amazon controller

						joystickaxis( device, 0, 0, lx );
						joystickaxis( device, 0, 1, ly );
						if ( lt > lt2 ) joystickaxis( device, 0, 2, lt );
						else joystickaxis( device, 0, 2, lt2 );
						joystickaxis( device, 1, 0, rx );
						joystickaxis( device, 1, 1, ry );
						if ( rt > rt2 ) joystickaxis( device, 1, 2, rt );
						else joystickaxis( device, 1, 2, rt2 );

						float dx = AMotionEvent_getAxisValue(event, AXIS_DPADX, pointer);
						float dy = AMotionEvent_getAxisValue(event, AXIS_DPADY, pointer);

						static int maxdevice = 6;
						static int* usesdpadaxis = 0;
						if ( usesdpadaxis == 0 )
						{
							usesdpadaxis = (int*) malloc( 6*4 );
							int i;
							for(i = 0; i < 6; i++ ) usesdpadaxis[i] = 0;
						}

						if ( dx < -0.9 || dx > 0.9 || dy < -0.9 || dy > 0.9 )
						{
							if ( device >= maxdevice )
							{
								int* temp = (int*) malloc( (device+5)*4 );
								int i;
								for(i = 0; i < maxdevice; i++ ) temp[i] = usesdpadaxis[i];
								maxdevice = device+1;
								free( usesdpadaxis );
								usesdpadaxis = temp;
							}

							usesdpadaxis[ device ] = 1;
						}

						if ( usesdpadaxis[ device ] == 1 )
						{
							// xbox controller produces axis events for the dpad
							if ( dx < -0.9 ) { joystickbutton(device,12,1); keydown(AKEYCODE_DPAD_LEFT); }
							else { joystickbutton(device,12,0); keyup(AKEYCODE_DPAD_LEFT); }

							if ( dx > 0.9 ) { joystickbutton(device,14,1); keydown(AKEYCODE_DPAD_RIGHT); }
							else { joystickbutton(device,14,0); keyup(AKEYCODE_DPAD_RIGHT); }

							if ( dy < -0.9 ) { joystickbutton(device,13,1); keydown(AKEYCODE_DPAD_UP); }
							else { joystickbutton(device,13,0); keyup(AKEYCODE_DPAD_UP); }

							if ( dy > 0.9 ) { joystickbutton(device,15,1); keydown(AKEYCODE_DPAD_DOWN); }
							else { joystickbutton(device,15,0); keyup(AKEYCODE_DPAD_DOWN); }
						}

						//LOGI("Joystick X1: %.1f, Y1: %.1f, X2: %.1f, Y2: %.1f, LT: %.1f, RT: %.1f", lx,ly, rx, ry, lt, rt);
						//LOGI("Joystick DX: %.1f, DY: %.1f", dx,dy);
					}
					break;
				}
			case 7: // mouse movement (Ouya touchpad reports itself as a mouse)
				{
					if ( (source & AINPUT_SOURCE_MOUSE) == AINPUT_SOURCE_MOUSE ) 
					{
						mousemoved( 0, x, y );
					}
					else if ( (source & AINPUT_SOURCE_TOUCHSCREEN) == AINPUT_SOURCE_TOUCHSCREEN ) 
					{
						touchmoved( id, x, y );
					}
					break;
				}
			// ouya also sends these duplicates for mouse clicking?
			//case 10: mousedown(0); break;
			//case 9: mouseup(0); break;

			default: break;
		}

		return 1;
	}
	else if ( AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY )
	{
		// key event

		int action = AKeyEvent_getAction( event );
		int flags = AKeyEvent_getFlags( event );
		int keyCode = AKeyEvent_getKeyCode( event );
		int device = AInputEvent_getDeviceId( event );
		int source = AInputEvent_getSource( event );

		int gamepad = 1;
		// 0x400 = gamepad
		if ( (source & 0x400) != 0 )
		{
			// set joystick buttons instead
			int state = (action == AKEY_EVENT_ACTION_DOWN) ? 1 : 0;

			switch(keyCode)
			{
				case 96:  joystickbutton(device,0,state); break; //0
				case 99:  joystickbutton(device,2,state); break; //2
				case 100: joystickbutton(device,3,state); break; //3
				case 97:  joystickbutton(device,1,state); break; //1

				case 82: joystickbutton(device,7,state); break; // Home //7
				case 108: joystickbutton(device,7,state); break; // Start //7

				case 102: joystickbutton(device,4,state); break; // L1 //4
				case 104: joystickbutton(device,10,state); break; // L2 //10
				case 106: joystickbutton(device,8,state); break; // L3 //8
				case 103: joystickbutton(device,5,state); break; // R1 //5
				case 105: joystickbutton(device,11,state); break; // R2 //11
				case 107: joystickbutton(device,9,state); break; // R3 //9

				// let dpad buttons fall through to non-gamepad input for left/right/up/down as well
				case AKEYCODE_DPAD_LEFT: joystickbutton(device,12,state); gamepad = 0; break;
				case AKEYCODE_DPAD_UP: joystickbutton(device,13,state); gamepad = 0; break;
				case AKEYCODE_DPAD_RIGHT: joystickbutton(device,14,state); gamepad = 0; break;
				case AKEYCODE_DPAD_DOWN: joystickbutton(device,15,state); gamepad = 0; break;

				default: gamepad = 0; break;
			}
		}
		else gamepad = 0;

		//LOGI("Action: %d, flags: %d, keyCode: %d", action, flags, keyCode);
		if ( gamepad == 0 )
		{
			switch( action )
			{
				case AKEY_EVENT_ACTION_DOWN:
				{
					keydown( keyCode );
					break;
				}

				case AKEY_EVENT_ACTION_UP:
				{
					keyup( keyCode );
					break;
				}
			}
		}

		if ( keyCode == AKEYCODE_HOME /*|| keyCode == AKEYCODE_MENU*/ ) return 0;
		//if ( keyCode == AKEYCODE_MENU && action == AKEY_EVENT_ACTION_UP ) return 0;
		if ( keyCode == AKEYCODE_VOLUME_DOWN || keyCode == AKEYCODE_VOLUME_UP ) return 0;
		return 1;
	}
	return 0;
}

/**
 * Process the next main command.
 */
static void engine_handle_cmd(struct android_app* app, int32_t cmd)
{
    struct engine* engine = (struct engine*)app->userData;
    switch (cmd) {
        case APP_CMD_SAVE_STATE:
        	LOGI("Save State");
            break;
        case APP_CMD_PAUSE:
			LOGI("App Paused");
			if ( app_mode < 0 )
			{
        		app_mode = 0;
				if (engine->accelerometerSensor != NULL) {
					ASensorEventQueue_disableSensor(engine->sensorEventQueue, engine->accelerometerSensor);
				}
				if (engine->gyroSensor != NULL) {
					ASensorEventQueue_disableSensor(engine->sensorEventQueue, engine->gyroSensor);
				}
        		if (engine->proximitySensor != NULL) {
					ASensorEventQueue_disableSensor(engine->sensorEventQueue, engine->proximitySensor);
				}
        		if (engine->lightSensor != NULL) {
					ASensorEventQueue_disableSensor(engine->sensorEventQueue, engine->lightSensor);
				}
        		if (engine->magneticSensor != NULL) {
					ASensorEventQueue_disableSensor(engine->sensorEventQueue, engine->magneticSensor);
				}
				if (engine->rotVectorSensor != NULL) {
					ASensorEventQueue_disableSensor(engine->sensorEventQueue, engine->rotVectorSensor);
				}
        		engine->animating = 0;
        		if ( initialised > 0 ) pauseapp();
			}
        	break;
        case APP_CMD_RESUME:
        	LOGI("App Resumed");
        	if ( app_mode == 0 || app_mode == -2 )
        	{
        		onstart(engine->app->activity);
				resumeapp();
				engine->animating = 1;
				if (engine->accelerometerSensor != NULL) {
					ASensorEventQueue_enableSensor(engine->sensorEventQueue, engine->accelerometerSensor);
					int minRate = ASensor_getMinDelay(engine->accelerometerSensor);
					//if ( minRate < 16667 ) minRate = 16667;
					ASensorEventQueue_setEventRate(engine->sensorEventQueue, engine->accelerometerSensor, minRate);
				}
				if (engine->gyroSensor != NULL) {
					ASensorEventQueue_enableSensor(engine->sensorEventQueue, engine->gyroSensor);
					int minRate = ASensor_getMinDelay(engine->gyroSensor);
					//if ( minRate < 16667 ) minRate = 16667;
					ASensorEventQueue_setEventRate(engine->sensorEventQueue, engine->gyroSensor, minRate);
				}
				if (engine->proximitySensor != NULL) {
					ASensorEventQueue_enableSensor(engine->sensorEventQueue, engine->proximitySensor);
					int minRate = ASensor_getMinDelay(engine->proximitySensor);
					//if ( minRate < 16667 ) minRate = 16667;
					ASensorEventQueue_setEventRate(engine->sensorEventQueue, engine->proximitySensor, minRate);
				}
				if (engine->lightSensor != NULL) {
					ASensorEventQueue_enableSensor(engine->sensorEventQueue, engine->lightSensor);
					int minRate = ASensor_getMinDelay(engine->lightSensor);
					//if ( minRate < 16667 ) minRate = 16667;
					ASensorEventQueue_setEventRate(engine->sensorEventQueue, engine->lightSensor, minRate);
				}
				if (engine->magneticSensor != NULL) {
					ASensorEventQueue_enableSensor(engine->sensorEventQueue, engine->magneticSensor);
					int minRate = ASensor_getMinDelay(engine->magneticSensor);
					//if ( minRate < 16667 ) minRate = 16667;
					ASensorEventQueue_setEventRate(engine->sensorEventQueue, engine->magneticSensor, minRate);
				}
				if (engine->rotVectorSensor != NULL) {
					ASensorEventQueue_enableSensor(engine->sensorEventQueue, engine->rotVectorSensor);
					int minRate = ASensor_getMinDelay(engine->rotVectorSensor);
					//if ( minRate < 16667 ) minRate = 16667;
					ASensorEventQueue_setEventRate(engine->sensorEventQueue, engine->rotVectorSensor, minRate);
				}
				app_mode = -1;
        	}
        	break;
        case APP_CMD_INIT_WINDOW:
        	LOGI("Window Init");
            // The window is being shown, get it ready.
            engine_init_display(engine);
            initialised = 1;
            resumeapp2();
            engine->animating = 1;
            break;
        case APP_CMD_TERM_WINDOW:
        	LOGI("Window Terminate");
            // The window is being hidden or closed, clean it up.
        	engine_term_display(engine);
            break;
        case APP_CMD_GAINED_FOCUS:
        	LOGI("Gained Focus");
        	if ( app_mode == 1 )
			{
        		onstart(engine->app->activity);
				resumeapp();
				engine->animating = 1;
				if (engine->accelerometerSensor != NULL) {
					ASensorEventQueue_enableSensor(engine->sensorEventQueue, engine->accelerometerSensor);
					int minRate = ASensor_getMinDelay(engine->accelerometerSensor);
					//if ( minRate < 16667 ) minRate = 16667;
					ASensorEventQueue_setEventRate(engine->sensorEventQueue, engine->accelerometerSensor, minRate);
				}
				if (engine->gyroSensor != NULL) {
					ASensorEventQueue_enableSensor(engine->sensorEventQueue, engine->gyroSensor);
					int minRate = ASensor_getMinDelay(engine->gyroSensor);
					//if ( minRate < 16667 ) minRate = 16667;
					ASensorEventQueue_setEventRate(engine->sensorEventQueue, engine->gyroSensor, minRate);
				}
				if (engine->proximitySensor != NULL) {
					ASensorEventQueue_enableSensor(engine->sensorEventQueue, engine->proximitySensor);
					int minRate = ASensor_getMinDelay(engine->proximitySensor);
					//if ( minRate < 16667 ) minRate = 16667;
					ASensorEventQueue_setEventRate(engine->sensorEventQueue, engine->proximitySensor, minRate);
				}
				if (engine->lightSensor != NULL) {
					ASensorEventQueue_enableSensor(engine->sensorEventQueue, engine->lightSensor);
					int minRate = ASensor_getMinDelay(engine->lightSensor);
					//if ( minRate < 16667 ) minRate = 16667;
					ASensorEventQueue_setEventRate(engine->sensorEventQueue, engine->lightSensor, minRate);
				}
				if (engine->magneticSensor != NULL) {
					ASensorEventQueue_enableSensor(engine->sensorEventQueue, engine->magneticSensor);
					int minRate = ASensor_getMinDelay(engine->magneticSensor);
					//if ( minRate < 16667 ) minRate = 16667;
					ASensorEventQueue_setEventRate(engine->sensorEventQueue, engine->magneticSensor, minRate);
				}
				if (engine->rotVectorSensor != NULL) {
					ASensorEventQueue_enableSensor(engine->sensorEventQueue, engine->rotVectorSensor);
					int minRate = ASensor_getMinDelay(engine->rotVectorSensor);
					//if ( minRate < 16667 ) minRate = 16667;
					ASensorEventQueue_setEventRate(engine->sensorEventQueue, engine->rotVectorSensor, minRate);
				}
				app_mode = -1;
			}
            break;
        case APP_CMD_LOST_FOCUS:
			LOGI("Lost Focus");
        	if ( app_mode < 0 )
			{
        		app_mode = 1;
        		if (engine->accelerometerSensor != NULL) {
					ASensorEventQueue_disableSensor(engine->sensorEventQueue, engine->accelerometerSensor);
				}
				if (engine->gyroSensor != NULL) {
					ASensorEventQueue_disableSensor(engine->sensorEventQueue, engine->gyroSensor);
				}
        		if (engine->proximitySensor != NULL) {
					ASensorEventQueue_disableSensor(engine->sensorEventQueue, engine->proximitySensor);
				}
        		if (engine->lightSensor != NULL) {
					ASensorEventQueue_disableSensor(engine->sensorEventQueue, engine->lightSensor);
				}
        		if (engine->magneticSensor != NULL) {
					ASensorEventQueue_disableSensor(engine->sensorEventQueue, engine->magneticSensor);
				}
        		if (engine->rotVectorSensor != NULL) {
					ASensorEventQueue_disableSensor(engine->sensorEventQueue, engine->rotVectorSensor);
				}
        		engine->animating = 0;
        		if ( initialised > 0 ) pauseapp();
			}
            break;

       case APP_CMD_CONFIG_CHANGED:
       {
    	   AConfiguration *config2 = AConfiguration_new();
		   AConfiguration_fromAssetManager( config2, engine->app->activity->assetManager);
		   int orien = AConfiguration_getOrientation( config2 );
		   int keyboard = AConfiguration_getKeyboard( config2 );
    	   int keyshidden = AConfiguration_getKeysHidden( config2 );
    	   LOGI("Config change, Orientation: %d, Keyboard: %d, KeysHidden: %d", orien, keyboard, keyshidden);
    	   AConfiguration_delete( config2 );

		   if ( keyboard == ACONFIGURATION_KEYBOARD_QWERTY ) keyboardmode( 1 ); // physical
    	   else keyboardmode( 2 ); // virtual

    	   break;
       }
       default:
       {
    	   //LOGW( "Unknown engine command: %d", cmd );
    	   break;
       }

    }
}

static void process_input(struct android_app* app, struct android_poll_source* source) {
    AInputEvent* event = NULL;
    int eventID = 0;

	// This generates lots of log messages like "Failed to receive dispatch signal.  status=-11" which can be avoided by using 
	// AInputQueue_hasEvents before AInputQueue_getEvent, but AInputQueue_hasEvents can cause ANR crashes in some situations (lots of input events?)
	// The log messages don't seem to do any harm so leave it as it is
	while ( (eventID = AInputQueue_getEvent(app->inputQueue, &event)) >= 0 )
	{
		//if ( AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY )
		//	LOGI("New input event: type=%d e=%d d=%d s=%d a=%d f=%d k=%d c=%d m=%d\n", AInputEvent_getType(event), eventID, AInputEvent_getDeviceId(event), AInputEvent_getSource(event), AKeyEvent_getAction(event),
		//										AKeyEvent_getFlags(event), AKeyEvent_getKeyCode(event), AKeyEvent_getScanCode(event), AKeyEvent_getMetaState(event));

        // if we predispatch this event it will crash some devices, e.g. pressing back when keyboard is visible
        if ( !(AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY && AKeyEvent_getKeyCode(event) == AKEYCODE_BACK) )
        {
        	if (AInputQueue_preDispatchEvent(app->inputQueue, event))
        	{
				// if backspace released and captured by something else, tell AGK anyway
				// this fixes the problem of backspace presses with no characters being forwarded without corresponding release events
				if ( AKeyEvent_getKeyCode(event) == 67 && AKeyEvent_getAction(event) == AKEY_EVENT_ACTION_UP )
				{
					keyup( 67 );
				}
				continue;
        	}
		}

        int32_t handled = 0;
        if (app->onInputEvent != NULL) handled = app->onInputEvent(app, event);
		if ( handled > 1 ) handled = 1;
        AInputQueue_finishEvent(app->inputQueue, event, handled);
    }
}

/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void android_main(struct android_app* state) {
    struct engine engine;

    ANativeActivity_setWindowFlags( state->activity, AWINDOW_FLAG_KEEP_SCREEN_ON, 0 );
	
    memset(&engine, 0, sizeof(engine));
    state->userData = &engine;
    state->onAppCmd = engine_handle_cmd;
    state->onInputEvent = engine_handle_input;
    state->inputPollSource.process = process_input;
    engine.app = state;

	char szModel[ 256 ];
	if ( getModel( engine.app->activity, szModel ) )
	{
		if ( strcmp( szModel, "ASUS_Z017D" ) == 0 ) 
		{
			g_iDisableLightProxSensor = 1; // for some reason this crashes this device
			LOGW("Disabling light and proximity sensors as they may crash this device");
		}
	}

    // Prepare to monitor accelerometer
    engine.sensorManager = ASensorManager_getInstance();
	engine.accelerometerSensor = 0;
    engine.gyroSensor = 0;
    engine.proximitySensor = 0;
	engine.lightSensor = 0;
	engine.magneticSensor = 0;
    engine.rotVectorSensor = 0;

    engine.sensorEventQueue = ASensorManager_createEventQueue(engine.sensorManager, state->looper, LOOPER_ID_USER, NULL, NULL);

    // loop waiting for stuff to do.

    while (1) {
        // Read all pending events.
        int ident;
        int events;
        struct android_poll_source* source;

        // If not animating, we will block forever waiting for events.
        // If animating, we loop until all events are read, then continue
        // to draw the next frame of animation.

        while ( (ident=ALooper_pollAll( 0, NULL, &events, (void**)&source)) >= 0 )
        {
            // Process this event.
            if (source != NULL) {
                source->process(state, source);
            }

            // If a sensor has data, process it now.
            if (ident == LOOPER_ID_USER)
            {
            	ASensorEvent event;
                while ( ASensorEventQueue_getEvents(engine.sensorEventQueue, &event, 1) > 0)
                {
                	switch (event.type) {
						case ASENSOR_TYPE_ACCELEROMETER:
							//LOGI( "Accel status: %d", event.acceleration.status );
							setaccel( event.acceleration.x, event.acceleration.y, event.acceleration.z );
						break;
						case ASENSOR_TYPE_GYROSCOPE:
							setgyro( event.vector.x, event.vector.y, event.vector.z );
						break;
						case ASENSOR_TYPE_PROXIMITY:
							setproximity( event.distance );
						break;
						case ASENSOR_TYPE_LIGHT:
							setlight( event.light );
						break;
						case ASENSOR_TYPE_MAGNETIC_FIELD:
							setmagnetic( event.magnetic.x,event.magnetic.y,event.magnetic.z );
						break;
						case 11:
						case 15:
							if ( event.type == rotationID )	setrotvector( event.data[0],event.data[1],event.data[2],event.data[3] );
						break;
					}
                }
            
            }

            // Check if we are exiting.
            if (state->destroyRequested != 0)
            {
            	LOGI("Exiting");
				// deleting anything here will cause an error if the activity is recreated since the native
				// side of the end never truely ends. The thread stops but the heap remains. Also can't call
				// exit(0) to force the native side to end as that's a crash in Android 10
                //cleanup();
				//done = 0;
				//initialised = 0;
				//app_mode = -2;
				//g_iDisableLightProxSensor = 0;
				//rotationID = 0;
                //exit(0);
                return;
            }
			
        }

        if (engine.animating)
        {
            engine_draw_frame(&engine);
        }
        else
        {
        	usleep( 20000 );
        }
    }
}
//END_INCLUDE(all)

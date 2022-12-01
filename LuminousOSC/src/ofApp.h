#pragma once

#include "ofMain.h"

#include "ofxCv.h"
#include "ofxGui.h"
#include "ofxOpenCv.h"
#include "ofxOsc.h"
#include "ofxXmlSettings.h"
#include "opencv2/opencv.hpp"

#ifdef TARGET_WIN32
    // for Visual Studio
    #include "../apps/myApps/LuminousOSC/src/Tracker.h"
    #include "../apps/myApps/LuminousOSC/src/MenuItem.h"
    #include "../apps/myApps/LuminousOSC/src/RangeSliderItem.h"
    #include "../apps/myApps/LuminousOSC/src/ofTrueTypeFontExt.h"
#else
    // for Xcode
    #include "Tracker.h"
    #include "MenuItem.h"
    #include "RangeSliderItem.h"
    #include "ofTrueTypeFontExt.h"
#endif

class ofApp : public ofBaseApp {

	public:
		void setup();
		void update();
		void draw();
		void exit();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void minAreaChanged(int& value);
		void maxAreaChanged(int& value);
		void maxPointsChanged(int& value);
		void useTrackingChanged(bool& value);
		void showInputChanged(bool& value);
		void showBinaryChanged(bool& value);
		void camIdChanged(string& value);
		void camWidthChanged(string& value);
		void camHeightChanged(string& value);
		void camFpsChanged(string& value);
		void camSettingsButtonPressed();
		void oscIpAddressChanged(string& value);
		void oscPortChanged(string& value);

		void loadFileButtonPressed();
		void switchToCameraButtonPressed();

		void setupMainUI();
		void releaseMainUI();

		void setupVideoUI();
		void releaseVideoUI();

		bool isVideoFileMode();
		bool isCameraMode();
		void setVideoFileMode();
		void setCameraMode();

		void inputRangeSlider(ofVec2f& mouse);


		const string GUI_FONT_PATH = "ui/DIN.otf";

		const string GUI_LABEL_THRESHOLD = "Threshold";
		const string GUI_LABEL_MIN_AREA = "Min Area to Detect";
		const string GUI_LABEL_MAX_AREA = "Max Area to Detect";
		const string GUI_LABEL_MAX_POINTS = "Max Number of Points to Send";
		const string GUI_LABEL_DIST_MATCHING = "Matching Distance";
		const string GUI_LABEL_SHOW_INPUT = "Show Input Image";
		const string GUI_LABEL_SHOW_BINARY = "Show Binary Image";
		const string GUI_LABEL_CAM_ID = "Camera Id";
		const string GUI_LABEL_CAM_WIDTH = "Camera Frame Width";
		const string GUI_LABEL_CAM_HEIGHT = "Camera Frame Height";
		const string GUI_LABEL_CAM_FPS = "Camera FPS";
		const string GUI_LABEL_CAM_SETTINGS_BUTTON = "Show Camera Settings";
		const string GUI_LABEL_OSC_IP_ADDRESS = "OSC IP Address";
		const string GUI_LABEL_OSC_PORT = "OSC Port Number";
		const string GUI_LABEL_OSC_ADDRESS_PATTERN = "OSC Address Pattern";
		const string GUI_LABEL_USE_TRACKING = "Use Tracking";
		const string GUI_LABEL_LOAD_VIDEO_FILE_BUTTON = "Load Video File";
		const string GUI_LABEL_SWITCH_TO_CAMERA_BUTTON = "Switch to Camera Mode";

		const int GUI_WIDTH = 300;
		const int DEFAULT_MIN_AREA_MIN = 0;
		const int DEFAULT_MIN_AREA_MAX = 1000;
		const int DEFAULT_MAX_AREA_MIN = 0;
		const int DEFAULT_MAX_AREA_MAX = 5000;
		const int DEFAULT_MAX_POINTS_MIN = 0;
		const int DEFAULT_MAX_POINTS_MAX = 50;
		const int DEFAULT_MATCHING_DIST_MIN = 0;
		const int DEFAULT_MATCHING_DIST_MAX = 300;
		const int DEFAULT_MAX_LIFE = 10;
		const int DEFAULT_LIFE_RECOVERY_AMOUNT = 5;
		const int DEFAULT_LIFE_REDUCING_AMOUNT = 1;
		const int DEFAULT_SHOW_CONSOLE_WINDOW = 0;

		ofxPanel gui;
		ofParameter<int> threshold;
		ofParameter<int> min_area;
		ofParameter<int> max_area;
		ofParameter<int> max_points;
		ofParameter<int>  dist_matching;

		ofParameter<bool> use_tracking;

		ofParameter<bool> show_input;
		ofParameter<bool> show_binary;
		ofxButton cam_settings_button;

		ofxButton load_video_file_button;
		ofxButton switch_to_camera_button;

		ofParameter<string> cam_id;
		ofParameter<string> cam_width;
		ofParameter<string> cam_height;
		ofParameter<string> cam_fps;
		ofParameter<string> osc_ip_address;
		ofParameter<string> osc_port;
		ofParameter<string> osc_address_pattern;

		// 変更フラグ
		bool bRequestChangeOSC = false;
		bool bRequestChangeCamera = false;
		bool bRequestChangeMaxPoints = false;
		bool bRequestChangeUseTracking = false;
		bool bRequestChangeVideoFile = false;
		bool bRequestSwitchToCamera = false;
		bool bMainUIInitializing = true;

		// 変更前のパラメータ
		string prev_cam_id;
		string prev_cam_width;
		string prev_cam_height;
		string prev_cam_fps;
		string prev_osc_ip_address;
		string prev_osc_port;
		ofVec2f prev_range = ofVec2f(0.0, 1.0);

		ofVideoGrabber	grabber;
		cv::Mat frame;
		cv::Mat gray;
		cv::Mat binary;
		cv::Mat label;
		ofImage	view_img;

		ofxOscSender sender;
		Tracker tracker;

		ofxXmlSettings adv_set;

		ofVideoPlayer video;
		bool isVideoFile = false;
		string video_file_path = "";

		std::map<std::string, MenuItem*> ui_items;
		RangeSliderItem* range_slider;

		// ofTrueTypeFontExt gui_font_small;
		ofTrueTypeFontExt gui_font_large;

		int loop_start_frame;
		int loop_end_frame;

		bool isShowConsole;
};

// Utils
vector<string> split(const string& str, char delim);
bool isNumber(const string& str);
bool isIpAddress(const string& value);

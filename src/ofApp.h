#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxOsc.h"

#include "CameraRay.h"
#include "HomographyTransform.h"
//#include "TweliteReceiver.h"

//static const int CAMERA_W = 1920;
//static const int CAMERA_H = 1080;
static const int CAMERA_W = 1280;
static const int CAMERA_H = 720;

static const int PLANE_SIZE = 100;
static const int MAX_TOUCH_DISTANCE = 100;

enum APP_VIEW_MODE {
	MODE_IMAGE_PROCESSING,
	MODE_3D,
};

enum APP_SEQUENCE {
	SEQUENCE_SET_CAMERA,
	SEQUENCE_CALIBRATION,
	SEQUENCE_RUN,
	SEQUENCE_TEST,
};

enum CALIB_SEQUENCE {
	CALIB_LT = 0,
	CALIB_RT,
	CALIB_RB,
	CALIB_LB,
	CALIB_NONE,
};


class ofApp : public ofBaseApp{

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
		
private:

	void draw_3d_view();
	void draw_plane();
	void calibration_2d();
	void calc_cross_point();
	ofPoint calc_cross_point_2d(ofPoint p1, ofPoint p2, ofPoint p3, ofPoint p4);
	ofPoint correct_position_2d(ofVec3f p);
	void set_calib_seq(int key);
	void updateHomography();
	void setup_osc();
	void send_data(ofPoint pos, bool isTouched);
	ofVec2f adjust_position(ofPoint pos);
	void draw_calibration_state();
	ofPoint calc_centroid(vector<ofPoint> _points);
	float calc_height(ofPoint p1, ofPoint p2, ofPoint input);
	//void draw_fude_direction();

	// tracking camera
	CameraRay *cam1;
	CameraRay *cam2;
	vector<ofPoint> cross_points;

	// rendering camera
	ofEasyCam cam;

	// homography params
	CHomographyTransform homography;
	ofVec2f src_points[4];

	// TWELITE
	//TweliteReceiver tweliteReceiver;

	//OSC params------------------
	string targetIP;
	int targetPort;
	string oscAddress;
	ofxOscSender sender;
	bool touchFlag;
	ofVec3f touchPoint;
	float wall_to_point;

	// gui
	ofxPanel cam1_gui, cam2_gui, adjust_gui;// , twelite_gui;
	//ofParameterGroup gui_params;
	bool showGui;

	ofParameterGroup adjust_params;
	ofParameter<bool> flipX;
	ofParameter<bool> flipY;
	ofParameter<bool> swapXY;
	ofParameter<ofVec2f> camera_offset;
	ofParameter<ofVec2f> camera_offset_tune;
	ofParameter<ofVec2f> camera_scale;
	ofParameter<ofVec2f> camera_scale_tune;
	ofParameter<bool> manual_mode;
	ofParameter<ofVec2f> src_tl;
	ofParameter<ofVec2f> src_tr;
	ofParameter<ofVec2f> src_br;
	ofParameter<ofVec2f> src_bl;
	ofParameter<float> touch_distance;

	APP_VIEW_MODE mode;
	APP_SEQUENCE sequence;
	CALIB_SEQUENCE calib_seq;
};

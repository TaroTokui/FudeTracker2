#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxOsc.h"

#include "CameraRay.h"
#include "HomographyTransform.h"

static const int CAMERA_W = 1280;
static const int CAMERA_H = 720;

static const int PLANE_SIZE = 100;

enum APP_VIEW_MODE {
	MODE_IMAGE_PROCESSING,
	MODE_3D,
};

enum APP_SEQUENCE {
	SEQUENCE_CALIBRATION,
	SEQUENCE_RUN,
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
	ofPoint calc_cross_point_2d(ofPoint p1, ofPoint p2, ofPoint p3, ofPoint p4);
	ofPoint correct_position_2d(ofVec3f p);
	void set_calib_seq(int key);
	void updateHomography();
	void setup_osc();
	void send_data(ofPoint pos, bool isTouched);
	ofVec2f adjust_position(ofPoint pos);
	void draw_calibration_state();

	// tracking camera
	CameraRay *cam1;
	CameraRay *cam2;
	vector<ofPoint> cross_points;

	// rendering camera
	ofEasyCam cam;

	// homography params
	CHomographyTransform homography;
	ofVec2f src_points[4];

	//OSC params------------------
	string targetIP;
	int targetPort;
	string oscAddress;
	ofxOscSender sender;

	// gui
	ofxPanel cam1_gui, cam2_gui, adjust_gui;
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

	APP_VIEW_MODE mode;
	APP_SEQUENCE sequence;
	CALIB_SEQUENCE calib_seq;
};

#pragma once

#include "ofMain.h"
//#include "ofxCv.h"	// oF 0.9.8 ver.
//#include "ofxOpenCv.h"
#include "ofxGui.h"

#include "CameraRay.h"

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
	void calc_cross_point_2d();
	void correct_position_2d();
	void set_calib_seq(int key);

	// tracking camera
	CameraRay *cam1;
	CameraRay *cam2;

	// rendering camera
	ofEasyCam cam;

	// gui
	ofxPanel gui;
	ofParameterGroup gui_params;
	bool showGui;

	APP_VIEW_MODE mode;
	APP_SEQUENCE sequence;
	CALIB_SEQUENCE calib_seq;
};

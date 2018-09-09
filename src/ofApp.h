#pragma once

#include "ofMain.h"
//#include "ofxCv.h"	// oF 0.9.8 ver.
//#include "ofxOpenCv.h"
#include "ofxGui.h"

#include "CameraRay.h"

static const int CAMERA_W = 1280;
static const int CAMERA_H = 720;

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

	CameraRay *cam1;
	CameraRay *cam2;

	// gui
	ofxPanel gui;
	ofParameterGroup gui_params;
	bool showGui;
};

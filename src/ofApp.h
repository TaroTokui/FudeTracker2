#pragma once

#include "ofMain.h"
#include "ofxCv.h"	// oF 0.9.8 ver.
#include "ofxOpenCv.h"
#include "ofxGui.h"

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

	void setup_camera();

	ofVideoGrabber cam_1;
	ofVideoGrabber cam_2;

	bool camera_is_initialized;

	// cv params
	cv::Mat src_mat_1;
	cv::Mat src_mat_2;
	cv::Mat gray_mat_1;
	cv::Mat gray_mat_2;
	cv::Mat binary_mat_1;
	cv::Mat binary_mat_2;
	cv::Mat morph_mat_1;
	cv::Mat morph_mat_2;

	ofImage img_1;
	ofImage img_2;

	// gui
	ofxPanel gui;
	ofParameterGroup gui_params;
	ofParameter<int> binarize_threshold;
	bool showGui;
};

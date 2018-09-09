#include "ofApp.h"

using namespace cv;

//--------------------------------------------------------------
void ofApp::setup()
{
	// app settings
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	ofSetWindowTitle("Fude Tracker");

	// initialize camera
	//setup_camera();
	cam1 = new CameraRay(CAMERA_W, CAMERA_H, 1);
	cam2 = new CameraRay(CAMERA_W, CAMERA_H, 2);

	// load shader

	// setup gui
	gui_params.setName("camera settings");
	gui_params.add(cam1->params);
	gui_params.add(cam2->params);

	string gui_settings = "settings";
	gui.setName(gui_settings);
	gui.setup(gui_params);
	gui.loadFromFile(gui_settings + ".xml");
	showGui = true;

}

//--------------------------------------------------------------
void ofApp::update()
{
	// grab new frame and find rays
	cam1->update();
	cam2->update();

	// calc fude position

	// send osc message

}

//--------------------------------------------------------------
void ofApp::draw(){

	int w = CAMERA_W / 2;
	int h = CAMERA_H / 2;

	ofBackground(0);

	cam1->draw_processed_image(0, 0);
	cam2->draw_processed_image(w, 0);

	ofSetColor(255);
	if (showGui)
	{
		gui.draw();
	}

	ofDrawBitmapString("fps: " + ofToString(ofGetFrameRate()), 10, 10);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	switch (key)
	{
	case 'g':
		showGui = !showGui;
		break;

	case 'f':
		ofToggleFullscreen();
		break;

	default:
		break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}


//==============================================================
// private method
//==============================================================

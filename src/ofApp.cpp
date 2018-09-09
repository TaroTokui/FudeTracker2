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
	setup_camera();

	// allocate cv mat
	src_mat_1 = Mat(CAMERA_H, CAMERA_W, CV_8UC3);
	src_mat_2 = Mat(CAMERA_H, CAMERA_W, CV_8UC3);
	gray_mat_1 = Mat(CAMERA_H, CAMERA_W, CV_8UC1);
	gray_mat_2 = Mat(CAMERA_H, CAMERA_W, CV_8UC1);
	binary_mat_1 = Mat(CAMERA_H, CAMERA_W, CV_8UC1);
	binary_mat_2 = Mat(CAMERA_H, CAMERA_W, CV_8UC1);
	morph_mat_1 = Mat(CAMERA_H, CAMERA_W, CV_8UC1);
	morph_mat_2 = Mat(CAMERA_H, CAMERA_W, CV_8UC1);

	img_1.allocate(CAMERA_W, CAMERA_H, OF_IMAGE_GRAYSCALE);
	img_2.allocate(CAMERA_W, CAMERA_H, OF_IMAGE_GRAYSCALE);
	processed_fbo_1.allocate(CAMERA_W, CAMERA_H);
	processed_fbo_2.allocate(CAMERA_W, CAMERA_H);

	// load shader

	// setup gui
	gui_params.setName("tracking");
	gui_params.add(binarize_threshold.set("binarize", 128, 0, 255));
	gui_params.add(contour_min.set("contour_min", 100, 0, 1000));
	gui_params.add(contour_max.set("contour_max", 1000, 0, 10000));

	string gui_settings = "settings";
	gui.setName(gui_settings);
	gui.setup(gui_params);
	gui.loadFromFile(gui_settings + ".xml");
	showGui = true;

	// setup contour finder
	contour_finder_1.setFindHoles(false);
	contour_finder_1.setMinArea(contour_min);
	contour_finder_1.setMaxArea(contour_max);
	contour_finder_1.setThreshold(15);
	contour_finder_1.getTracker().setPersistence(15);
	contour_finder_1.getTracker().setMaximumDistance(32);

	contour_finder_2.setFindHoles(false);
	contour_finder_2.setMinArea(contour_min);
	contour_finder_2.setMaxArea(contour_max);
	contour_finder_2.setThreshold(15);
	contour_finder_2.getTracker().setPersistence(15);
	contour_finder_2.getTracker().setMaximumDistance(32);

}

//--------------------------------------------------------------
void ofApp::update()
{
	// check camera
	if (!camera_is_initialized) return;

	// grab new frame
	cam_1.update();
	cam_2.update();

	// find fude marker
	image_prcessing();

	// calc fude position

	// send osc message

}

//--------------------------------------------------------------
void ofApp::draw(){

	int w = CAMERA_W / 2;
	int h = CAMERA_H / 2;

	ofBackground(0);

	//cam_1.draw(0, 0, w, h);
	//cam_2.draw(w, 0, w, h);
	processed_fbo_1.draw(0, 0, w, h);
	processed_fbo_2.draw(w, 0, w, h);

	img_1.setFromPixels(morph_mat_1.data, CAMERA_W, CAMERA_H, OF_IMAGE_GRAYSCALE);
	img_1.update();
	img_1.draw(0, h, w, h);

	img_2.setFromPixels(binary_mat_2.data, CAMERA_W, CAMERA_H, OF_IMAGE_GRAYSCALE);
	img_2.update();
	img_2.draw(w, h, w, h);

	//contour_finder_1.draw();
	//contour_finder_1.draw();

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

//--------------------------------------------------------------
void ofApp::setup_camera()
{
	camera_is_initialized = false;

	//we can now get back a list of devices.
	vector<ofVideoDevice> devices = cam_1.listDevices();

	for (int i = 0; i < devices.size(); i++) {
		if (devices[i].bAvailable) {
			ofLogNotice() << devices[i].id << ": " << devices[i].deviceName;
		}
		else {
			ofLogNotice() << devices[i].id << ": " << devices[i].deviceName << " - unavailable ";
		}
	}

	// check camera count
	// cam 1, cam2 and face cam
	if (devices.size() < 3)
	{
		cout << "this program use 2 cameras." << endl;
		return;
	}

	cam_1.setDeviceID(1);
	cam_1.setDesiredFrameRate(60);
	cam_1.initGrabber(CAMERA_W, CAMERA_H);

	cam_2.setDeviceID(2);
	cam_2.setDesiredFrameRate(60);
	cam_2.initGrabber(CAMERA_W, CAMERA_H);

	//videoInverted.allocate(camWidth, camHeight, OF_PIXELS_RGB);
	//videoTexture.allocate(videoInverted);


	camera_is_initialized = true;

	return;
}

//--------------------------------------------------------------
void ofApp::image_prcessing()
{

	// convert to cv mat
	ofxCv::toOf(src_mat_1, cam_1.getPixels());
	cvtColor(src_mat_1, gray_mat_1, CV_BGR2GRAY);
	ofxCv::toOf(src_mat_2, cam_2.getPixels());
	cvtColor(src_mat_2, gray_mat_2, CV_BGR2GRAY);

	// binarize with threshold
	threshold(gray_mat_1, binary_mat_1, binarize_threshold, 255, THRESH_BINARY);
	threshold(gray_mat_2, binary_mat_2, binarize_threshold, 255, THRESH_BINARY);

	// noise reduction
	morphologyEx(binary_mat_1, morph_mat_1, MORPH_OPEN, cv::Mat(), cv::Point(-1, -1), 1);
	morphologyEx(binary_mat_2, morph_mat_2, MORPH_OPEN, cv::Mat(), cv::Point(-1, -1), 1);

	// find contour
	contour_finder_1.setMinArea(contour_min);
	contour_finder_1.setMaxArea(contour_max);
	contour_finder_2.setMinArea(contour_min);
	contour_finder_2.setMaxArea(contour_max);

	contour_finder_1.findContours(morph_mat_1);
	contour_finder_2.findContours(morph_mat_2);

	// update processed image
	ofPushStyle();
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	processed_fbo_1.begin();
	ofBackground(0);
	cam_1.draw(0, 0, CAMERA_W, CAMERA_H);
	ofSetColor(255, 0, 255);
	contour_finder_1.draw();
	processed_fbo_1.end();
	ofPopStyle();

	ofPushStyle();
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	processed_fbo_2.begin();
	ofBackground(0);
	cam_2.draw(0, 0, CAMERA_W, CAMERA_H);
	ofSetColor(255, 0, 255);
	contour_finder_2.draw();
	processed_fbo_2.end();
	ofPopStyle();

}
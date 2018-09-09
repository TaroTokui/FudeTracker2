#include "CameraRay.h"

using namespace cv;

//--------------------------------------------------------------
CameraRay::CameraRay(int w, int h, int index)
{
	cam_w = w;
	cam_h = h;

	setup_camera(index);

	// allocate cv mat
	src_mat = Mat(h, w, CV_8UC3);
	gray_mat = Mat(h, w, CV_8UC1);
	binary_mat = Mat(h, w, CV_8UC1);
	morph_mat = Mat(h, w, CV_8UC1);

	img.allocate(w, h, OF_IMAGE_GRAYSCALE);
	processed_fbo.allocate(w, h);

	// setup params
	params.setName("cam " + ofToString(index));
	params.add(binarize_threshold.set("binarize", 128, 0, 255));
	params.add(contour_min.set("contour_min", 100, 0, 1000));
	params.add(contour_max.set("contour_max", 1000, 0, 10000));
	params.add(position.set("pos", ofVec3f(0, 0, 0), ofVec3f(-10, -10, -10), ofVec3f(10, 10, 10)));
	params.add(rotation.set("rotation", ofVec3f(0, 0, 0), ofVec3f(-10, -10, -10), ofVec3f(10, 10, 10)));
	params.add(up_vector.set("up", ofVec3f(0, 0, 0), ofVec3f(-1, -1, -10), ofVec3f(1, 1, 1)));

	// setup contour finder
	contour_finder.setFindHoles(false);
	contour_finder.setThreshold(15);
	contour_finder.getTracker().setPersistence(15);
	contour_finder.getTracker().setMaximumDistance(32);
}

//--------------------------------------------------------------
CameraRay::~CameraRay() {}

//--------------------------------------------------------------
void CameraRay::update()
{
	// check camera
	if (!camera_is_initialized) return;

	// grab new frame
	cam.update();

	// find fude marker
	image_prcessing();
}

//--------------------------------------------------------------
void CameraRay::draw_processed_image(int x, int y)
{
	int w = cam_w / 2;
	int h = cam_h / 2;

	processed_fbo.draw(x, y, w, h);

	img.setFromPixels(morph_mat.data, cam_w, cam_h, OF_IMAGE_GRAYSCALE);
	img.update();
	img.draw(x, y+h, w, h);

}

//--------------------------------------------------------------
void CameraRay::setup_camera(int id)
{
	camera_is_initialized = false;

	//we can now get back a list of devices.
	vector<ofVideoDevice> devices = cam.listDevices();

	for (int i = 0; i < devices.size(); i++) {
		if (devices[i].bAvailable) {
			ofLogNotice() << devices[i].id << ": " << devices[i].deviceName;
		}
		else {
			ofLogNotice() << devices[i].id << ": " << devices[i].deviceName << " - unavailable ";
		}
	}

	// check camera count
	if (devices.size() < CAMERA_COUNT)
	{
		cout << "this program use 2 cameras." << endl;
		return;
	}

	cam.setDeviceID(id);
	cam.setDesiredFrameRate(60);
	cam.initGrabber(cam_w, cam_h);

	camera_is_initialized = true;
}

//--------------------------------------------------------------
void CameraRay::image_prcessing()
{
	// convert to cv mat
	ofxCv::toOf(src_mat, cam.getPixels());
	cvtColor(src_mat, gray_mat, CV_BGR2GRAY);

	// binarize with threshold
	threshold(gray_mat, binary_mat, binarize_threshold, 255, THRESH_BINARY);

	// noise reduction
	morphologyEx(binary_mat, morph_mat, MORPH_OPEN, cv::Mat(), cv::Point(-1, -1), 1);

	// find contour
	contour_finder.setMinArea(contour_min);
	contour_finder.setMaxArea(contour_max);

	contour_finder.findContours(morph_mat);

	// update processed image
	ofPushStyle();
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	processed_fbo.begin();
	ofBackground(0);
	cam.draw(0, 0, cam_w, cam_h);
	ofSetColor(255, 0, 255);
	contour_finder.draw();
	processed_fbo.end();
	ofPopStyle();

	cout << "cam1: " << contour_finder.size() << endl;

}
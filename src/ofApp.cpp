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

	adjust_params.setName("params");
	adjust_params.add(manual_mode.set("manual mode", false));
	adjust_params.add(flipX.set("flipX", true));
	adjust_params.add(flipY.set("flipY", true));
	adjust_params.add(swapXY.set("swapXY", true));
	adjust_params.add(camera_offset.set("offset", ofVec2f(0, 0), ofVec2f(-0.5, -0.5), ofVec2f(1.5, 1.5)));
	adjust_params.add(camera_offset_tune.set("offset tune", ofVec2f(0, 0), ofVec2f(-0.1, -0.1), ofVec2f(0.1, 0.1)));
	adjust_params.add(camera_scale.set("scale", ofVec2f(1, 1), ofVec2f(0.3, 0.3), ofVec2f(1.5, 1.5)));
	adjust_params.add(camera_scale_tune.set("scale tune", ofVec2f(0, 0), ofVec2f(-0.1, -0.1), ofVec2f(0.1, 0.1)));
	
	adjust_gui.setup(adjust_params, "adjust_settings.xml");
	adjust_gui.loadFromFile("adjust_settings.xml");


	showGui = true;

	// setup camera
	cam.setDistance(100);

	mode = MODE_IMAGE_PROCESSING;
	sequence = SEQUENCE_RUN;

	src_points[0] = ofVec2f(0, 0);
	src_points[1] = ofVec2f(100, 0);
	src_points[2] = ofVec2f(100, 100);
	src_points[3] = ofVec2f(0, 100);

	updateHomography();

	// setup osc
	setup_osc();
}

//--------------------------------------------------------------
void ofApp::update()
{
	// grab new frame and find rays
	cam1->update();
	cam2->update();

	// calc cross point
	cross_points.clear();
	ofPoint p1 = cam1->getPosition();
	ofPoint p3 = cam2->getPosition();
	for each (auto ray1 in cam1->getRays())
	{
		auto p2 = ray1;

		for each (auto ray2 in cam2->getRays())
		{
			auto p4 = ray2;
			cross_points.push_back(calc_cross_point_2d(p1, p2, p3, p4));
		}
	}


	switch (sequence)
	{
	case SEQUENCE_CALIBRATION:
		calibration_2d();
		break;

	case SEQUENCE_RUN:
		//correct_position_2d();
		break;

	default:
		break;
	}

	if (cross_points.size() < 1) return;

	ofVec3f p = ofVec3f(cross_points[0].x, cross_points[0].z, 0);
	//cout << p << endl;
	//cout << correct_position_2d(p) << endl;

	// send osc message
	send_data(p, ofGetKeyPressed(' '));
}

//--------------------------------------------------------------
void ofApp::draw(){

	int w = CAMERA_W / 2;
	int h = CAMERA_H / 2;

	ofBackground(0);

	switch (mode)
	{
	case MODE_IMAGE_PROCESSING:
		cam1->draw_processed_image(0, 0);
		cam2->draw_processed_image(w, 0);
		break;

	case MODE_3D:
		draw_3d_view();
		break;

	default:
		break;
	}


	ofSetColor(255);
	if (showGui)
	{
		gui.draw();
		adjust_gui.draw();
	}

	ofDrawBitmapString("fps: " + ofToString(ofGetFrameRate()), 10, 10);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	switch (key)
	{
	case '1':
		mode = MODE_IMAGE_PROCESSING;
		break;

	case '2':
		mode = MODE_3D;
		break;

	case 'h':
		updateHomography();
		break;

	case 'g':
		showGui = !showGui;
		break;

	case 'f':
		ofToggleFullscreen();
		break;

	case 'r':
		cam.reset();
		break;

	default:
		break;
	}

	set_calib_seq(key);
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
void ofApp::draw_3d_view()
{
	ofEnableAlphaBlending();
	cam.begin();

	ofDrawAxis(100);
	draw_plane();

	cam1->draw_camera_position();
	cam2->draw_camera_position();

	cam1->draw_rays();
	cam2->draw_rays();

	ofSetColor(255);
	for each (auto p in cross_points)
	{
		ofPushMatrix();
		//ofDrawEllipse(p, 5, 5);
		ofTranslate(p);
		ofDrawSphere(1);
		ofPopMatrix();
	}

	ofSetColor(255,0,0);
	for each (auto p in src_points)
	{
		ofPushMatrix();
		ofTranslate(p.x, 0, p.y);
		ofDrawSphere(1);
		ofPopMatrix();
	}

	cam.end();
}

//--------------------------------------------------------------
void ofApp::draw_plane()
{
	ofPushMatrix();
	ofSetColor(255, 32);
	ofTranslate(PLANE_SIZE / 2, 0, PLANE_SIZE / 2);
	ofDrawBox(ofPoint(0, 0, 0), PLANE_SIZE, 0.1, PLANE_SIZE);
	ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::calibration_2d()
{

}

//--------------------------------------------------------------
ofPoint ofApp::correct_position_2d(ofVec3f p)
{
	return homography.getTransformedPoint(p);
}

//--------------------------------------------------------------
ofPoint ofApp::calc_cross_point_2d(ofPoint p1, ofPoint p2, ofPoint p3, ofPoint p4)
{
	//cout << "p1:" << p1 << ", p2:" << p2 << ", p3:" << p3 << ", p4:" << p4 << endl;
	auto a1 = (p2.z - p1.z) / (p2.x - p1.x);
	auto a3 = (p4.z - p3.z) / (p4.x - p3.x);

	auto x = (a1*p1.x - p1.z - a3*p3.x + p3.z) / (a1 - a3);
	auto y = (p2.z - p1.z) / (p2.x - p1.x)*(x - p1.x) + p1.z;

	ofPoint p(x, 0, y);

	//cout << p << endl;

	return p;
}

//--------------------------------------------------------------
void ofApp::set_calib_seq(int key)
{
	if (key == OF_KEY_RETURN)
	{
		if (cross_points.size() > 0)
		{
			src_points[(int)calib_seq] = ofVec2f(cross_points[0].x, cross_points[0].z);	// use first point
		}
	}

	if (key != OF_KEY_RIGHT) return;

	// set next sequence 
	calib_seq = static_cast<CALIB_SEQUENCE>(calib_seq + 1);

	if (calib_seq >= CALIB_NONE)
	{
		calib_seq = CALIB_LT;
	}

	cout << "caliblation sequence " << calib_seq << endl;
}

//--------------------------------------------------------------
void ofApp::updateHomography()
{
	vector<cv::Point2f> srcPoints, dstPoints;

	// input points from cross points
	//ofVec2f* maskPoints = maskGenerator.getHomographyPoints();
	srcPoints.push_back(cv::Point2f(src_points[0].x, src_points[0].y));
	srcPoints.push_back(cv::Point2f(src_points[1].x, src_points[1].y));
	srcPoints.push_back(cv::Point2f(src_points[2].x, src_points[2].y));
	srcPoints.push_back(cv::Point2f(src_points[3].x, src_points[3].y));

	dstPoints.push_back(cv::Point2f(0, 0));
	dstPoints.push_back(cv::Point2f(1, 0));
	dstPoints.push_back(cv::Point2f(1, 1));
	dstPoints.push_back(cv::Point2f(0, 1));
	homography.calcHomography(srcPoints, dstPoints);

	//cout << homography.getTransformedPoint(ofPoint(0.5,0.5)) << endl;
}

//--------------------------------------------------------------
void ofApp::setup_osc()
{
	// OSC
	ofXml XML;
	if (XML.load("osc_settings.xml")) {
		cout << "mySettings.xml loaded!" << endl;
	}

	if (XML.exists("//IP")) {
		targetIP = XML.getValue<string>("//IP");
	}
	else {
		targetIP = "localhost";
	}

	if (XML.exists("//PORT")) {
		targetPort = XML.getValue<int>("//PORT");
	}
	else {
		targetPort = 20001;
	}

	if (XML.exists("//ADDRESS")) {
		oscAddress = "/" + XML.getValue<string>("//ADDRESS");
	}
	else {
		oscAddress = "/kinect1";
	}

	sender.setup(targetIP, targetPort); // open an outgoing connection to HOST:PORT
}

//--------------------------------------------------------------
void ofApp::send_data(ofPoint pos, bool isTouched)
{
	ofxOscMessage m;

	// scale and add offset
	ofPoint posNormal = adjust_position(pos);

	//cout << depthPos << endl;
	m.clear();
	m.setAddress("/position");
	m.addFloatArg(posNormal.x);
	m.addFloatArg(posNormal.y);
	m.addFloatArg(posNormal.z);

	sender.sendMessage(m, false);

	m.clear();
	m.setAddress("/isTouch");

	if (manual_mode)
	{
		//if (ofGetKeyPressed(' '))
		//{
		//	m.addIntArg(1);
		//}
		//else {
		//	m.addIntArg(0);
		//}
		m.addIntArg(1);
	}
	else if (isTouched)
	{
		m.addIntArg(1);
	}
	else {
		m.addIntArg(0);
	}
	sender.sendMessage(m, false);

	cout << posNormal.x << " " << posNormal.y << " " << isTouched << endl;

}

//--------------------------------------------------------------
ofVec2f ofApp::adjust_position(ofPoint pos)
{
	if (flipX)
	{
		pos.x = 1.0 - pos.x;
	}
	if (flipY)
	{
		pos.y = 1.0 - pos.y;
	}
	float x = pos.x * (camera_scale->x + camera_scale_tune->x) + camera_offset->x + camera_offset_tune->x;
	float y = pos.y * (camera_scale->y + camera_scale_tune->y) + camera_offset->y + camera_offset_tune->y;
	if (swapXY)
	{
		float tmp = x;
		x = y;
		y = tmp;
	}
	return ofVec2f(x, y);
}

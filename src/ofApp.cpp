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
	cam1 = new CameraRay(CAMERA_W, CAMERA_H, 0);
	cam2 = new CameraRay(CAMERA_W, CAMERA_H, 2);

	// TWELITE
	//string port = "COM3";
	////ofXml XML;
	////if (XML.exists("//TWELITE_PORT")) {
	////	port = XML.getValue<string>("//TWELITE_PORT");
	////}
	//int baud = 115200;
	//tweliteReceiver.setup(port, baud);

	// load shader

	// setup gui
	//gui_params.setName("camera settings");
	//gui_params.add(cam1->params);
	//gui_params.add(cam2->params);

	string gui_settings;
	// cam 1
	gui_settings = "cam1_settings";
	cam1_gui.setName(gui_settings);
	cam1_gui.setup(cam1->params, gui_settings + ".xml");
	cam1_gui.loadFromFile(gui_settings + ".xml");

	// cam 2
	gui_settings = "cam2_settings";
	cam2_gui.setName(gui_settings);
	cam2_gui.setup(cam2->params, gui_settings + ".xml");
	cam2_gui.loadFromFile(gui_settings + ".xml");
	cam2_gui.setPosition(cam1_gui.getPosition() + ofPoint(cam1_gui.getWidth(), 0));

	// twelite
	//twelite_gui.setup(tweliteReceiver.params, "twelite_settings.xml");
	//twelite_gui.loadFromFile("twelite_settings.xml");
	//twelite_gui.setPosition(cam2_gui.getPosition() + ofPoint(cam2_gui.getWidth(), 0));


	// other
	adjust_params.setName("params");
	adjust_params.add(manual_mode.set("manual mode", false));
	adjust_params.add(flipX.set("flipX", true));
	adjust_params.add(flipY.set("flipY", true));
	adjust_params.add(swapXY.set("swapXY", true));
	adjust_params.add(camera_offset.set("offset", ofVec2f(0, 0), ofVec2f(-0.5, -0.5), ofVec2f(1.5, 1.5)));
	adjust_params.add(camera_offset_tune.set("offset tune", ofVec2f(0, 0), ofVec2f(-0.1, -0.1), ofVec2f(0.1, 0.1)));
	adjust_params.add(camera_scale.set("scale", ofVec2f(1, 1), ofVec2f(0.3, 0.3), ofVec2f(1.5, 1.5)));
	adjust_params.add(camera_scale_tune.set("scale tune", ofVec2f(0, 0), ofVec2f(-0.1, -0.1), ofVec2f(0.1, 0.1)));
	adjust_params.add(src_tl.set("src_tl", ofVec2f(0, 0), ofVec2f(0, 0), ofVec2f(100, 100)));
	adjust_params.add(src_tr.set("src_tr", ofVec2f(100, 0), ofVec2f(0, 0), ofVec2f(100, 100)));
	adjust_params.add(src_br.set("src_br", ofVec2f(100, 100), ofVec2f(0, 0), ofVec2f(100, 100)));
	adjust_params.add(src_bl.set("src_bl", ofVec2f(0, 100), ofVec2f(0, 0), ofVec2f(100, 100)));
	adjust_params.add(touch_distance.set("touch_distance", 30, 0, 100));
	
	adjust_gui.setup(adjust_params, "adjust_settings.xml");
	adjust_gui.loadFromFile("adjust_settings.xml");
	//adjust_gui.setPosition(gui.getPosition() + ofPoint(gui.getWidth(),0));
	adjust_gui.setPosition(cam2_gui.getPosition() + ofPoint(cam2_gui.getWidth(), 0));

	showGui = true;

	// setup camera
	cam.setDistance(100);

	mode = MODE_IMAGE_PROCESSING;
	sequence = SEQUENCE_RUN;

	src_points[0] = src_tl;
	src_points[1] = src_tr;
	src_points[2] = src_br;
	src_points[3] = src_bl;

	updateHomography();

	// setup osc
	setup_osc();
}

//--------------------------------------------------------------
void ofApp::update()
{
	// receive acceleration
	//tweliteReceiver.update();

	// grab new frame and find rays
	cam1->update();
	cam2->update();

	// calc cross point
	calc_cross_point();

	// set touch flag and position
	if (cross_points.size() > 0) {
		//touchFlag = true;
		//touchPoint = ofVec3f(cross_points[0].x, cross_points[0].z, 0);
		touchFlag = (wall_to_point < touch_distance) ? true : false;
		touchPoint = calc_centroid(cross_points) * 0.1 + touchPoint * 0.9;
	}
	else {
		touchFlag = false;
		//wall_to_point = MAX_TOUCH_DISTANCE;
	}
	
	// send osc message
	send_data(correct_position_2d(touchPoint), touchFlag);
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
		cam1_gui.draw();
		cam2_gui.draw();
		adjust_gui.draw();
		//twelite_gui.draw();
	}

	//ofDrawBitmapString("fps: " + ofToString(ofGetFrameRate()), 10, 10);
	draw_calibration_state();
}

//--------------------------------------------------------------
void ofApp::exit()
{
	//tweliteReceiver.exit();
	//ofSleepMillis(100);
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

	case 'g':
		showGui = !showGui;
		break;

	case 'f':
		ofToggleFullscreen();
		break;

	case 'r':
		cam.reset();
		break;

	//case 't':
	//	tweliteReceiver.setBaseAcc();
	//	break;

	case OF_KEY_TAB:
		sequence = SEQUENCE_CALIBRATION;
		break;

	case ' ':
		sequence = SEQUENCE_RUN;
		break;

	default:
		break;
	}

	switch (sequence)
	{
	case SEQUENCE_CALIBRATION:
		set_calib_seq(key);
		break;

	case SEQUENCE_RUN:
		//correct_position_2d();
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

	send_data(ofPoint((float)x / (float)CAMERA_W, (float)y / (float)CAMERA_H), true);
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

#if 1
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
#endif

	//draw_fude_direction();

	cam.end();

	ofDrawBitmapString("fps: " + ofToString(ofGetFrameRate()), 10, 10);
	ofDrawBitmapString("cam 1 angle: " + ofToString(ofGetFrameRate()), 10, 22);
	ofDrawBitmapString("cam 2 angle: " + ofToString(ofGetFrameRate()), 10, 34);
	ofDrawBitmapString("height: " + ofToString(wall_to_point), 10, 46);
}

////--------------------------------------------------------------
//void ofApp::draw_fude_direction()
//{
//
//	// unit vector
//	ofVec3f Znormal(0, 0, 1);
//	ofVec3f Xnormal(1, 0, 0);
//	ofVec3f Ynormal(0, 1, 0);
//
//	// roll, pitch, yaw
//	float roll = tweliteReceiver.getAcc().z;
//	float pitch = tweliteReceiver.getAcc().x;
//	float yaw = tweliteReceiver.getAcc().y;
//
//	// calc quaternion
//	ofQuaternion qr(roll, Znormal);
//	ofQuaternion qp(pitch, Xnormal);
//	ofQuaternion qy(yaw, Ynormal);
//	ofQuaternion qt;    //  total quaternion
//
//	qt = qr * qp * qy;
//
//	ofVec3f qAxsis;
//	float   angle;
//	qt.getRotate(angle, qAxsis);
//
//	ofPushMatrix();
//	ofRotate(angle, qAxsis.x, qAxsis.y, qAxsis.z);
//	//ofRotateX(tweliteReceiver.getAcc().x);
//	//ofRotateY(tweliteReceiver.getAcc().y);
//	//ofRotateZ(tweliteReceiver.getAcc().z);
//	
//	ofDrawRotationAxes(10, 0.1);
//	ofDrawAxis(20);
//	ofPopMatrix();
//
//}

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
void ofApp::calc_cross_point()
{
	cross_points.clear();
	ofPoint p1 = cam1->getPosition();
	ofPoint p3 = cam2->getPosition();
	for each (auto ray1 in cam1->getRays())
	{
		auto p2 = ray1;

		for each (auto ray2 in cam2->getRays())
		{
			auto p4 = ray2;
			auto p = calc_cross_point_2d(p1, p2, p3, p4);
			//cross_points.push_back(calc_cross_point_2d(p1, p2, p3, p4));

			// calc hight
			float h1 = calc_height(p1, p2, p);
			float h2 = calc_height(p3, p4, p);

			// check hight
			//cout << "height: " << h1 << ", diff: " << abs(h1 - h2) << endl;
			if (abs(h1 - h2) < 1.5)
			{
				// add cross point
				cross_points.push_back(p);

				// update height
				wall_to_point = h1;

			}

		}
	}
}

//--------------------------------------------------------------
ofPoint ofApp::calc_cross_point_2d(ofPoint p1, ofPoint p2, ofPoint p3, ofPoint p4)
{
	//cout << "p1:" << p1 << ", p2:" << p2 << ", p3:" << p3 << ", p4:" << p4 << endl;
	auto a1 = (p2.z - p1.z) / (p2.x - p1.x);
	auto a3 = (p4.z - p3.z) / (p4.x - p3.x);

	auto x = (a1*p1.x - p1.z - a3*p3.x + p3.z) / (a1 - a3);
	auto z = (p2.z - p1.z) / (p2.x - p1.x)*(x - p1.x) + p1.z;

	ofPoint p(x, 0, z);

	//cout << p << endl;

	return p;
}

//--------------------------------------------------------------
float ofApp::calc_height(ofPoint p1, ofPoint p2, ofPoint input)
{
	float t = (p1.x - input.x) / (p1.x - p2.x);	// calc line
	float h = (1-t) * p1.y + t * p2.y;
	return h;
}

//--------------------------------------------------------------
void ofApp::set_calib_seq(int key)
{

	if (key == 'h')
	{
		updateHomography();
	}

	if (key == 'u')
	{
		src_points[0] = src_tl;
		src_points[1] = src_tr;
		src_points[2] = src_br;
		src_points[3] = src_bl;

		updateHomography();

	}

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

	//cout << "caliblation sequence " << calib_seq << endl;
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

	// update src_points for gui
	src_tl.set(src_points[0]);
	src_tr.set(src_points[1]);
	src_br.set(src_points[2]);
	src_bl.set(src_points[3]);

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

	cout << "ip: " << targetIP << ", port: " << targetPort << ", address: " << oscAddress << endl;
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

	//cout << posNormal.x << " " << posNormal.y << " " << isTouched << endl;

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

//--------------------------------------------------------------
void ofApp::draw_calibration_state()
{

	ofDrawBitmapString("h: set homography mat", 10, ofGetHeight() - 36);

	string text = "";

	text = "App sequence: ";
	switch (sequence)
	{
	case SEQUENCE_CALIBRATION:
		text += "calibration";
		break;
	case SEQUENCE_RUN:
		text += "run";
		break;
	default:
		break;
	}

	ofDrawBitmapString(text, 10, ofGetHeight() - 24);

	text = "calibration sequence: ";
	switch (calib_seq)
	{
	case CALIB_LT:
		text += "top left";
		break;
	case CALIB_RT:
		text += "top right";
		break;
	case CALIB_RB:
		text += "bottom right";
		break;
	case CALIB_LB:
		text += "bottom left";
		break;
	case CALIB_NONE:
		break;
	default:
		break;
	}

	ofDrawBitmapString(text, 10, ofGetHeight() - 12);
}

//--------------------------------------------------------------
ofPoint ofApp::calc_centroid(vector<ofPoint> _points)
{
	ofPoint centroid(0,0);
	int count = _points.size();
	for (size_t i = 0; i < count; i++)
	{
		//centroid += _points[i] / (float)count;
		centroid.x += _points[i].x / (float)count;
		centroid.y += _points[i].z / (float)count;
	}

	return centroid;
}
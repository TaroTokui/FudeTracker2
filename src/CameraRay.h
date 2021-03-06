//
//  CHomographyTransform.hpp
//  HomographyPointTest
//
//  Created by Taro Tokui on 2018/9/17.
//
//

#pragma once

#include "ofMain.h"
#include "ofxCv.h"	// oF 0.9.8 ver.
#include "ofxOpenCv.h"
#include "ofxGui.h"

static const int CAMERA_COUNT = 3;	// cam 1, cam2 and face cam
static const int RAY_LENGTH = 100;

class CameraRay
{
public:
	CameraRay(int w, int h, int index);
	~CameraRay();

	void update();
	void draw_processed_image(int x, int y);
	void draw_camera_position();
	void draw_rays();

	// getter
	ofVec3f getPosition() { return position; }
	vector<ofVec3f> getRays() { return rays_global; }

	// setter
	//void setPosition(ofVec3f _position) { position = _position; }

	ofParameterGroup params;
private:

	void setup_camera(int id);
	void image_prcessing();
	void marker_to_ray();
	ofVec3f calc_global_ray(ofVec3f _ray);

	ofVideoGrabber cam;

	bool camera_is_initialized;

	// cv params
	cv::Mat src_mat;
	cv::Mat gray_mat;
	cv::Mat binary_mat;
	cv::Mat morph_mat;
	cv::Mat mask_mat;
	cv::Mat roi_mat;

	ofxCv::ContourFinder contour_finder;

	ofImage img;

	ofFbo processed_fbo;

	// marker points
	vector<ofVec2f> markers;

	// gui
	ofParameter<int> binarize_threshold;
	ofParameter<int> contour_min;
	ofParameter<int> contour_max;
	ofParameter<ofVec3f> position;
	ofParameter<ofVec3f> rotation;
	ofParameter<ofVec3f> up_vector;
	ofParameter<float> fulcrum_distance;
	ofParameter<ofVec2f> mask_tl;
	ofParameter<ofVec2f> mask_br;

	int cam_w;
	int cam_h;
	vector<ofVec3f> rays;
	vector<ofVec3f> rays_global;
};
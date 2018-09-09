//
//  CHomographyTransform.hpp
//  HomographyPointTest
//
//  Created by Taro Tokui on 2015/12/16.
//
//

#pragma once

#include <iostream>
#include "ofMain.h"
#include "ofxCv.h"

class CHomographyTransform {
public:
	CHomographyTransform();
	~CHomographyTransform();

	// each points(srcPoints and dstPoints) need 4 point data
	void calcHomography(vector<cv::Point2f> srcPoints, vector<cv::Point2f> dstPoints);

	ofPoint getTransformedPoint(ofPoint input);

private:

	cv::Mat mHomography;
};

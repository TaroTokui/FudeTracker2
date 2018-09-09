//
//  CHomographyTransform.cpp
//  HomographyPointTest
//
//  Created by Taro Tokui on 2015/12/16.
//
//

#include "HomographyTransform.h"

using namespace cv;

//--------------------------------------------------------------
CHomographyTransform::CHomographyTransform()
{
	vector<Point2f> srcPoints, dstPoints;

	srcPoints.push_back(Point2f(0, 0));
	srcPoints.push_back(Point2f(1, 0));
	srcPoints.push_back(Point2f(1, 1));
	srcPoints.push_back(Point2f(0, 1));

	dstPoints.push_back(Point2f(0, 0));
	dstPoints.push_back(Point2f(1, 0));
	dstPoints.push_back(Point2f(1, 1));
	dstPoints.push_back(Point2f(0, 1));

	// generate a default homography
	mHomography = findHomography(Mat(srcPoints), Mat(dstPoints));

}

//--------------------------------------------------------------
CHomographyTransform::~CHomographyTransform()
{
}

//--------------------------------------------------------------
void CHomographyTransform::calcHomography(vector<Point2f> srcPoints, vector<Point2f> dstPoints)
{
	if (srcPoints.size() != 4 || dstPoints.size() != 4)
	{
		cout << "faild: homography array wasn't caluculated." << endl;
		cout << "       You must set 4 points." << endl;
		return;
	}

	// generate a homography from input points
	mHomography = findHomography(Mat(srcPoints), Mat(dstPoints));
}

//--------------------------------------------------------------
ofPoint CHomographyTransform::getTransformedPoint(ofPoint input)
{
	//Mat inputMat = (Mat_<double>(3, 1) << input.x, input.y, input.z);
	Mat inputMat = (Mat_<double>(3, 1) << input.x, input.y, 1.0);
	Mat tmpMat = mHomography * inputMat;

	float dst_x = tmpMat.at<double>(0, 0) / tmpMat.at<double>(2, 0);
	float dst_y = tmpMat.at<double>(1, 0) / tmpMat.at<double>(2, 0);

	return ofPoint(dst_x, dst_y);
}
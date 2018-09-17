//
//  CHomographyTransform.hpp
//  HomographyPointTest
//
//  Created by Taro Tokui on 2017/11/22.
//
//

#pragma once
#include "ofMain.h"

static const string TAG_X = ":x=";
static const string TAG_Y = ":y=";
static const string TAG_Z = ":z=";
// :ed=810F2CD2
static const int VALUE_LENGTH = 4;
static const int MAX_MESSAGE_SIZE = 100;

class TweliteReceiver {
public:
	TweliteReceiver() {}
	~TweliteReceiver() {}

	void setup(string port, int baud);
	void update();
	void exit();

	void setBaseAcc() { base_acc = acc; }
	const ofVec3f getAcc() { return diff_acc; }

	ofParameterGroup params;
private:
	//ofVec3f acc;
	ofSerial	serial;

	char		bytesRead[MAX_MESSAGE_SIZE];				// data from serial, we will be trying to read 3
	char		bytesReadString[MAX_MESSAGE_SIZE + 1];			// a string needs a null terminator, so we need 3 + 1 bytes
	int			nBytesRead;					// how much did we read?
	int			nTimesRead;					// how many times did we read?	

	ofParameter<bool> isActive;
	ofParameter<bool> manual_mode;
	ofParameter<ofVec3f> acc;
	ofParameter<ofVec3f> base_acc;
	ofParameter<ofVec3f> diff_acc;
	ofParameter<bool> isFalling;
	ofParameter<int> fallThreshold;
	ofParameter<int> acc_threshold;
};
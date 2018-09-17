#include "TweliteReceiver.h"

//--------------------------------------------------------------
void TweliteReceiver::setup(string port, int baud)
{
	serial.setup(port, baud);

	params.setName("Accelerometer");
	params.add(isActive.set("isActive", true));
	params.add(manual_mode.set("manual mode", false));
	params.add(acc.set("acceleration", ofVec3f(0, 0, 0), ofVec3f(-500, -500, -500), ofVec3f(500, 500, 500)));
	params.add(base_acc.set("base_acc", ofVec3f(0, 0, 0), ofVec3f(-500, -500, -500), ofVec3f(500, 500, 500)));
	params.add(diff_acc.set("diff_acc", ofVec3f(0, 0, 0), ofVec3f(-500, -500, -500), ofVec3f(500, 500, 500)));
	params.add(acc_threshold.set("acc threshold", 3, 0, 10));
	params.add(isFalling.set("isFalling", false));
	params.add(fallThreshold.set("fall threshold", 10, 0, 100));
}

//--------------------------------------------------------------
void TweliteReceiver::update()
{
	unsigned char bytesReturned[MAX_MESSAGE_SIZE];

	memset(bytesReadString, 0, MAX_MESSAGE_SIZE + 1);
	memset(bytesReturned, 0, MAX_MESSAGE_SIZE);

	nTimesRead = 0;
	while (serial.readBytes(bytesReturned, MAX_MESSAGE_SIZE) > 0) {
		nTimesRead++;
	};

	if (!isActive) return;

	memcpy(bytesReadString, bytesReturned, MAX_MESSAGE_SIZE);

	string result = bytesReadString;
	//cout << result << endl;

	if (nTimesRead > 0)
	{
		// find xyz value
		string::size_type index_x = result.rfind(TAG_X);
		string::size_type index_y = result.rfind(TAG_Y);
		string::size_type index_z = result.rfind(TAG_Z);
		if (index_x != string::npos)
		{
			// convert to ofvec3
			ofVec3f tmp_acc;
			tmp_acc.x = ofToInt(result.substr(index_x + TAG_X.length(), VALUE_LENGTH));
			tmp_acc.y = ofToInt(result.substr(index_y + TAG_Y.length(), VALUE_LENGTH));
			tmp_acc.z = ofToInt(result.substr(index_z + TAG_Z.length(), VALUE_LENGTH));

			// fall decision
			if (tmp_acc.length() < fallThreshold)
			{
				// remove 1 frame noise
				if (!isFalling)
				{
					isFalling = true;
					return;
				}
			}
			else {
				isFalling = false;
			}

			acc = tmp_acc;
			ofVec3f tmpVec3 = acc.get() - base_acc.get();
			if (abs(tmpVec3.x) < acc_threshold)
			{
				tmpVec3.x = 0;
			}
			if (abs(tmpVec3.y) < acc_threshold)
			{
				tmpVec3.y = 0;
			}
			if (abs(tmpVec3.z) < acc_threshold)
			{
				tmpVec3.z = 0;
			}

			diff_acc = tmpVec3;// acc.get() - base_acc.get();

							   //cout << "acc: " << acc << endl;
							   //cout << int(acc.get().length()) << endl;
		}
	}

}

//--------------------------------------------------------------
void TweliteReceiver::exit()
{
	serial.close();
}

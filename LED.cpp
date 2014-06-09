// File:	LED.cpp
// By:		Minh Dang
// Date:	May 2014
// Info:	This class in charge of storing YUV values
//			for image filtering to detect objects.

#include "LED.h"

LED::LED(void) {
}

LED::LED(string name) {
	setType(name);

	if (name == "wLight") {
		setYUVMin(Scalar(149, 0, 252));
		setYUVMax(Scalar(153, 1, 256));
	}

	if (name == "red") {
		// Orange ping pong ball @HOME
		setYUVMin(Scalar(16, 96, 226));
		setYUVMax(Scalar(35, 256, 256));
		setColour(Scalar(0, 0, 255));

		// TESTING FOR QUAD LIGHT
		//setYUVMin(Scalar(142, 0, 254));
		//setYUVMax(Scalar(153, 7, 256));
		//setColour(Scalar(0, 0, 255));

		// TEST BALL @LAB
		//setYUVMin(Scalar(0, 88, 168));
		//setYUVMax(Scalar(39, 228, 256));
		//setColour(Scalar(0, 0, 255));
	}

	if (name == "blue") {
		// Testing code
	//	setYUVMin(Scalar(0, 126, 142));
	//	setYUVMax(Scalar(251, 195, 252));
	//	setColour(Scalar(255, 0, 0));
	}

	if (name == "yellow") {

	}
}

LED::~LED(void) {
}
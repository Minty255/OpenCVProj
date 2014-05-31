// File:	LED.cpp
// By:		Minh Dang
// Date:	May 2014
// Info:	The purpose of this file is to use for 
//			classifying more than one type of object

#include "LED.h"

LED::LED(void)
{

}

LED::LED(string name) {
	setType(name);

	if (name == "wLight") {
		setHSVMin(Scalar(149, 0, 252));
		setHSVMax(Scalar(153, 1, 256));
	}

	if (name == "red") {
		// Orange ping pong ball @HOME

		/* HSV FOR RED HALO
		HSVMin(160, 91, 251)
		HSVMax(202, 256, 256)*/

		setHSVMin(Scalar(0, 100, 175));
		setHSVMax(Scalar(70, 256, 256));
		setColour(Scalar(0, 0, 255));

	}

	if (name == "blue") {
		// Testing code
	//	setHSVMin(Scalar(0, 126, 142));
	//	setHSVMax(Scalar(251, 195, 252));
	//	setColour(Scalar(255, 0, 0));
	}

	if (name == "yellow") {

	}
}

LED::~LED(void)
{
}
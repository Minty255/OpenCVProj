// File:	LED.h
// By:		Minh Dang
// Date:	May 2014
// Info:	Definition for LED.cpp

#pragma once
#include <string>
#include <opencv\cv.h>
#include <opencv\highgui.h>

using namespace cv;
using namespace std;

class LED
{
public:
	LED(void);
	~LED(void);

	LED(string name);

	int getXPos() { return xPos; }
	void setXPos(int x) { xPos = x; }
	
	int getYPos() { return yPos; }
	void setYPos(int y) { yPos = y; }

	Scalar getYUVMin() { return YUVMin; }
	void setYUVMin(Scalar min) { YUVMin = min; }

	Scalar getYUVMax() { return YUVMax; }
	void setYUVMax(Scalar max) { YUVMax = max; }

	string getType() { return type; }
	void setType(string t) { type = t; }

	Scalar getColour() { return colour; }
	void setColour(Scalar c) { colour = c; }

private:
	int xPos, yPos;
	string type;
	Scalar YUVMin, YUVMax;
	Scalar colour;
};


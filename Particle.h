// File:	Particle.h
// By:		Minh Dang
// Date:	May 2014

#pragma once
#include <string>
#include <opencv\cv.h>
#include <opencv\highgui.h>

using namespace cv;
using namespace std;

class Particle
{
public:
	Particle(void);
	~Particle(void);

	Particle(int x, int y, int z, int roll, int pitch, int yaw);

	int getScore() { return m_score; }
	void setScore(double score) { m_score = score; }

public:
	double m_score;
};


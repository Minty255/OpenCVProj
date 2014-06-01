// File:	LED.cpp
// By:		Minh Dang
// Date:	May 2014
// Info:	The purpose of this file is to use for 
//			classifying more than one type of object

#include "Particle.h"

Particle::Particle(void)
{
}

Particle::Particle(int x, int y, int z, int roll, int pitch, int yaw) : Position(x, y, z, roll, pitch, yaw) {
	m_score = 0;
}

Particle::~Particle(void)
{
}
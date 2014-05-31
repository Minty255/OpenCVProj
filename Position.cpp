// File:	Position.cpp
// By:		Minh Dang
// Date:	May 2014

#include "Position.h"

Position::Position(void)
{
}

Position::Position(int x, int y, int z, int roll, int pitch, int yaw) {
	m_x = x;
	m_y = y;
	m_z = z;
	m_roll = roll;
	m_pitch = pitch;
	m_yaw = yaw;
}

Position::~Position(void)
{
}
// File:	main.cpp
// By:		Minh Dang
// Date:	May 2014
// Info:	Multiple objects detection using blob detection approach
//			This code has been downloaded with modification and built on top.

//Written by  Kyle Hounslow 2013

//Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software")
//, to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
//and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

//The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
//IN THE SOFTWARE.

#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include <ctime>
#include <math.h>
#include "LED.h"
#include "Position.h"
#include "Particle.h"

//initial min and max HSV filter values.
//these will be changed using trackbars
int H_MIN = 0;
int H_MAX = 256;
int S_MIN = 0;
int S_MAX = 256;
int V_MIN = 0;
int V_MAX = 256;
//default capture width and height
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;
//max number of objects to be detected in frame
const int MAX_NUM_OBJECTS=500;
//minimum and maximum object area
const int MIN_OBJECT_AREA = 10*10;
const int MAX_OBJECT_AREA = 40*40;//FRAME_HEIGHT*FRAME_WIDTH/1.5;
//numbers of particle
const int NUM_PARTICLES = 1;
// constant for adjusting
const double LAMDA = 20.0;
//names that will appear at the top of each window
const string windowName = "Original Image";
const string windowName1 = "HSV Image";
const string windowName2 = "Thresholded Image";
const string windowName3 = "After Morphological Operations";
const string trackbarWindowName = "Trackbars";

vector<Position> objectCoord;
vector<Particle> particles;

void on_trackbar(int, void*)
{
	//This function gets called whenever a
	// trackbar position is changed
}

string intToString(int number) {
	std::stringstream ss;
	ss << number;
	return ss.str();
}

void createTrackbars() {
	//create window for trackbars
	namedWindow(trackbarWindowName, 0);
	//create memory to store trackbar name on window
	char TrackbarName[50];
	sprintf(TrackbarName, "H_MIN", H_MIN);
	sprintf(TrackbarName, "H_MAX", H_MAX);
	sprintf(TrackbarName, "S_MIN", S_MIN);
	sprintf(TrackbarName, "S_MAX", S_MAX);
	sprintf(TrackbarName, "V_MIN", V_MIN);
	sprintf(TrackbarName, "V_MAX", V_MAX);
	//create trackbars and insert them into window
	//3 parameters are: the address of the variable that is changing when the trackbar is moved(eg.H_LOW),
	//the max value the trackbar can move (eg. H_HIGH), 
	//and the function that is called whenever the trackbar is moved(eg. on_trackbar)
	//                                  ---->    ---->     ---->      
	createTrackbar("H_MIN", trackbarWindowName, &H_MIN, H_MAX, on_trackbar );
	createTrackbar("H_MAX", trackbarWindowName, &H_MAX, H_MAX, on_trackbar );
	createTrackbar("S_MIN", trackbarWindowName, &S_MIN, S_MAX, on_trackbar );
	createTrackbar("S_MAX", trackbarWindowName, &S_MAX, S_MAX, on_trackbar );
	createTrackbar("V_MIN", trackbarWindowName, &V_MIN, V_MAX, on_trackbar );
	createTrackbar("V_MAX", trackbarWindowName, &V_MAX, V_MAX, on_trackbar );
}

void drawObject(vector<LED> theLEDs,Mat &frame) {
	for (int ii = 0; ii < theLEDs.size(); ii++) {
		Particle temp(theLEDs[ii].getXPos(), theLEDs[ii].getYPos(), 0, 0, 0, 0);
		objectCoord.push_back(temp);
		cv::circle(frame, cv::Point(theLEDs[ii].getXPos(), theLEDs[ii].getYPos()), 5, cv::Scalar(0, 0, 255));
		// Label the object with coordinate
		cv::putText(frame, intToString(theLEDs[ii].getXPos())+ " , " + intToString(theLEDs[ii].getYPos()), cv::Point(theLEDs[ii].getXPos(), theLEDs[ii].getYPos()+20), 1, 1, Scalar(0, 255, 0));
		//cv::putText(frame, theLEDs[ii].getType(), cv::Point(theLEDs[ii].getXPos(), theLEDs[ii].getYPos()-30), 1, 2, theLEDs[ii].getColour()); 
	}
}

void morphOps(Mat &thresh) {
	//create structuring element that will be used to "dilate" and "erode" image.

	Mat erodeElement = getStructuringElement(MORPH_RECT, Size(1, 1));
	//dilate with larger element so make sure object is nicely visible
	Mat dilateElement = getStructuringElement(MORPH_RECT, Size(2, 2));

	erode(thresh, thresh, erodeElement);
	erode(thresh, thresh, erodeElement);

	dilate(thresh, thresh, dilateElement);
	dilate(thresh, thresh, dilateElement);
}

void trackFilteredObject(Mat threshold,Mat HSV, Mat &cameraFeed) {

	vector<LED> reds;

	Mat temp;
	threshold.copyTo(temp);
	//these two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );
	//use moments method to find our filtered object
	double refArea = 0;
	bool objectFound = false;
	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();
		//if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
		if(numObjects<MAX_NUM_OBJECTS){
			for (int index = 0; index >= 0; index = hierarchy[index][0]) {

				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

				//if the area is less than 20 px by 20px then it is probably just noise
				//if the area is the same as the 3/2 of the image size, probably just a bad filter
				//we only want the object with the largest area so we safe a reference area each
				//iteration and compare it to the area in the next iteration.
				if(area>MIN_OBJECT_AREA && area<MAX_OBJECT_AREA){
					LED red;

					red.setXPos(moment.m10/area);
					red.setYPos(moment.m01/area);
					
					reds.push_back(red);

					objectFound = true;

				}else objectFound = false;


			}
			//let user know you found an object
			if(objectFound ==true){
				//draw object location on screen
				drawObject(reds,cameraFeed);}

		}else putText(cameraFeed,"TOO MUCH NOISE! ADJUST FILTER",Point(0,50),1,2,Scalar(0,0,255),2);
	}
}

// Overloading method for multiple object tracking
void trackFilteredObject(LED theLED, Mat threshold,Mat HSV, Mat &cameraFeed) {
	vector<LED> leds;
	Mat temp;

	threshold.copyTo(temp);
	//these two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );
	//use moments method to find our filtered object
	double refArea = 0;
	bool objectFound = false;
	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();
		//if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
		if(numObjects<MAX_NUM_OBJECTS){
			for (int index = 0; index >= 0; index = hierarchy[index][0]) {

				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

				// Checking for object area - has been modified to check for small light source
				if(area>MIN_OBJECT_AREA){
					LED led;

					led.setXPos(moment.m10/area);
					led.setYPos(moment.m01/area);
					led.setType(theLED.getType());
					led.setColour(theLED.getColour());
					leds.push_back(led);

					// TESTING
				//	unsigned char *input = (unsigned char*)(cameraFeed.data);
				//	int r, g, b;

				//	g = input[cameraFeed.cols * (led.getYPos() + 10) + (led.getXPos()+10)];
				//	b = input[cameraFeed.cols * (led.getYPos() + 10) + (led.getXPos()+10) + 1];
				//	r = input[cameraFeed.cols * (led.getYPos() + 10) + (led.getXPos()+10) + 2];
					
			//		if ((r > 150) && (g < 150) && (b < 150))
						objectFound = true;

				} else objectFound = false;
			}

			//let user know you found an object
			if(objectFound == true){
				//draw object location on screen
				drawObject(leds,cameraFeed);
			}
		} else putText(cameraFeed,"TOO MUCH NOISE! ADJUST FILTER",Point(0,50),1,2,Scalar(0,0,255),2);
	}
}

// Callback function for mouse event
void CallBackFunc(int event, int x, int y, int flags, void* userdata) {
	if(event == EVENT_LBUTTONDOWN) {
		particles[0].setX(x);
		particles[0].setY(y);

		cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	} else if(event == EVENT_RBUTTONDOWN) {
		cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	} else if(event == EVENT_MBUTTONDOWN) {
		cout << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	} else if (event == EVENT_MOUSEMOVE) {
		//cout << "Mouse move over the window - position (" << x << ", " << y << ")" << endl;
	}
}

// Scoring particles function
void scoreParticles(Mat &feed) {
	double temp = 0, total = 0;
	double d1, d2, t1, t2;

	//for (int ii = 0; ii < particles.size(); ii++) {
	//	Particle &p = particles[ii];
	//	int size = 5;
	//	Mat roi = feed(Rect(p.m_x-size, p.m_y-size, size*2, size*2));
	//	Vec3f col;

	//	for(int x = 0; x < roi.cols; x++) {
	//		for(int y = 0; y < roi.rows; y++) {
	//			Vec3f temp = roi.at<Vec3f>(y,x);
	//			col += temp;
	//		}
	//	}
	//	cout << col[0] << ',' << col[1] << ',' << col[2] << endl;
	//}

	// This will iterate through and calculate the distance between each particle and each object detected
	// Then it will feed the calculated distance into a function to give it a score
	// Highest score (closest distance) between a particle and an object will be store
	for (int ii = 0; ii < particles.size(); ii++) {
		for (int jj = 0; jj < objectCoord.size(); jj++) {
			d1 = pow((double)(particles[ii].m_x - objectCoord[ii].m_x), 2);
			d2 = pow((double)(particles[ii].m_y - objectCoord[ii].m_y), 2);
			t1 = 1/LAMDA;
			t2 = sqrt(d1+d2);
			temp = t1*exp(-t1*t2);
			
			printf("D1: %f D2: %f T1: %f T2: %f TEMP: %.50f\n", d1, d2, t1, t2, temp);
			
			// Updating particles' score and also calculate the total score
			if (temp > particles[ii].m_score) {
				total -= particles[ii].getScore();
				particles[ii].m_score = temp;
				total += particles[ii].getScore();
			}
		}
	}

	// Normalise the score for all particles
	for (int ii = 0; ii < particles.size(); ii++)
		particles[ii].m_score /= total;
}

// Resample particles to generate new one
void resample() {
	
}

// Iterate through and output all particles' score
void outputScores() {
	for (int ii = 0; ii < particles.size(); ii++)
		printf("Particle (%d, %d): %f\n", particles[ii].m_x, particles[ii].m_y, particles[ii].m_score);
}

int main(int argc, char* argv[])
{
	//if we would like to calibrate our filter values, set to true.
	bool calibrationMode = false;
	
	//Matrix to store each frame of the webcam feed
	Mat cameraFeed;
	Mat threshold;
	Mat HSV;

	if(calibrationMode){
		//create slider bars for HSV filtering
		createTrackbars();
	}
	//video capture object to acquire webcam feed
	VideoCapture capture;
	//open capture object at location zero (default location for webcam)
	capture.open(0);
	//set height and width of capture frame
	capture.set(CV_CAP_PROP_FRAME_WIDTH,FRAME_WIDTH);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT,FRAME_HEIGHT);
	

	// Generate random particles
	for (int ii = 0; ii < NUM_PARTICLES; ii++) {
		Particle temp(abs(rand()%(FRAME_WIDTH)), abs(rand()%(FRAME_HEIGHT)), 0,0,0,0);
//		Particle temp(100, 100, 0, 0, 0, 0);
		particles.push_back(temp);
	}

	// Mouse click event
	setMouseCallback(windowName, CallBackFunc, NULL);

	namedWindow(windowName);
	//start an infinite loop where webcam feed is copied to cameraFeed matrix
	//all of our operations will be performed within this loop
	while(1) {
		// Clear objects in vector so could store new one 
		objectCoord.clear();
		
		//store image to matrix	
		capture.read(cameraFeed);
		//convert frame from BGR to HSV colorspace
		cvtColor(cameraFeed,HSV,COLOR_BGR2HSV);

		if(calibrationMode==true) {
			//if in calibration mode, we track objects based on the HSV slider values.
			cvtColor(cameraFeed,HSV,COLOR_BGR2HSV);
			inRange(HSV,Scalar(H_MIN,S_MIN,V_MIN),Scalar(H_MAX,S_MAX,V_MAX),threshold);
			morphOps(threshold);
			imshow(windowName2,threshold);
			trackFilteredObject(threshold,HSV,cameraFeed);
		} else {
			// Declare a list of object to be label - LED's colours
			LED red("red");

			cvtColor(cameraFeed,HSV,COLOR_BGR2HSV);
			inRange(HSV, red.getHSVMin(), red.getHSVMax(), threshold);
			trackFilteredObject(red, threshold,HSV,cameraFeed);
		}
		
		// Display particle on the actual image frame
		for (int ii = 0; ii < particles.size(); ii++) {
			cv::circle(cameraFeed, cv::Point(particles[ii].getX(), particles[ii].getY()), 2, cv::Scalar(0, 0, 255));
		}

		scoreParticles(cameraFeed);
		//outputScores();

		//show frames 
		imshow(windowName,cameraFeed);
		imshow(windowName1,threshold);

		//delay 30ms so that screen can refresh.
		//image will not appear without this waitKey() command
		waitKey(30);
	}

	return 0;
}

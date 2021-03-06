// openCV test project.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <opencv2/core/core.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <string.h>
#include "SerialPort.h"
#include "PID.h"
using namespace cv;
using namespace std;
int offset[2] = { 0,0 };
float ratio[2] = { 0.7,1 };
vector <float> setPtX;
vector <float> setPtY;
int currentStep;

//String for getting the output from arduino
char output[MAX_DATA_LENGTH];

/*Portname must contain these backslashes, and remember to
replace the following com port*/
char port_name[] = "\\\\.\\COM7";

//String for incoming data
char incomingData[MAX_DATA_LENGTH];

void load(string fname) {
	fname += ".AbNAr";
	ifstream file;
	file.open(fname.c_str(), ios::in|ios::binary);
	setPtX.resize(0);
	setPtY.resize(0);
	currentStep = 0;
	int size;
	file.read((char *)&size, sizeof(size));
	for(int i = 0 ; i < size/2 ; ++i) {
		float data;
		file.read((char*)&data, sizeof(data));
		setPtX.push_back(data);
		file.read((char*)&data, sizeof(data));
		setPtY.push_back(data);
	}
}

bool getOVal(char &x, char &y, int height , int width , float ptX , float ptY , int cValX , int cValY) {
	cValX -= width / 2;
	cValX -= offset[0];
	cValY -= height / 2;
	cValY -= offset[1];
	//cout << " cval = ( " << cValX << " , " << cValY << " ) ";
	//getting set point;
	ptX *= width * ratio[0] / 2;
	ptY *= height * ratio[1] / 2;
	int err[2];
	err[0] = cValX - ptX;
	err[1] = cValY - ptY;
	//cout << " err = ( " << err[0] << " , " << err[1] << " ) ";
	//cout << " ( " << width * ratio[0] << " , " << (height*ratio[1]) << " ) ";
	if ((err[0] > width * ratio[0] / 2) || (err[0] < -width * ratio[0] / 2))goto end;
	if ((err[1] > height * ratio[1] / 2) || (err[1] < -height * ratio[1] / 2))goto end;
	x = int(float(err[0])*256 / (width*ratio[0]));
	y = int(float(err[1])*256 / (height*ratio[1]));
	return 0;
	//cout << " ( " << (int)x << " , " << (int)y << " ) ";
	end:;
	return 1;
}


int main()
{
	//pid pidX ,pidY;
	start:
	{
		int temp;
		SerialPort arduino(port_name);
		if (arduino.isConnected()) cout << "Connection Established" << endl;
		else cout << "ERROR, check port name", cin >> temp;

		VideoCapture capWebcam(0);
		
		Mat imgOriginal;
		Mat imgHSV;
		Mat imgThresh;
		Mat imgThresh2;

		std::vector<Vec3f> v3fCircles;

		char checkForEscKey = 0;
		load("sqrure");
		char dataX = 0;
		char dataY = 0;
		while (checkForEscKey != 27 && capWebcam.isOpened()) {
			capWebcam.set(CAP_PROP_BRIGHTNESS, 500);
			capWebcam.read(imgOriginal);
			cvtColor(imgOriginal, imgHSV, CV_BGR2HSV);
			inRange(imgHSV, Scalar(0, 128, 200), Scalar(18, 255, 255), imgThresh);
			inRange(imgHSV, Scalar(165, 128, 200), Scalar(180, 255, 255), imgThresh2);
			imgThresh = imgThresh + imgThresh2;
			GaussianBlur(imgThresh, imgThresh, cv::Size(3, 3), 0);
			Mat structuringElement = getStructuringElement(MORPH_RECT, Size(3, 3));
			dilate(imgThresh, imgThresh, structuringElement);
			erode(imgThresh, imgThresh, structuringElement);
			HoughCircles(imgThresh, v3fCircles, CV_HOUGH_GRADIENT, 2, imgThresh.rows / 4, 100, 50, 10, 70);
			//for (int i = 0; i < v3fCircles.size(); ++i) {
				//cout << "ball position,radius = ( " << v3fCircles[i][0] << " , " << v3fCircles[i][1] << " , " << v3fCircles[i][2] << " )" << endl;
			//	circle(imgOriginal, Point((int)v3fCircles[i][0], (int)v3fCircles[i][1]), 3, Scalar(0, 255, 0), CV_FILLED);
			//	circle(imgOriginal, Point((int)v3fCircles[i][0], (int)v3fCircles[i][1]), (int)v3fCircles[i][2], Scalar(255, 0 , 0),4);

			//}

			int  X = 0, Y = 0;
			cv::Size Sz = imgThresh.size();
			for (int i = 0; i < v3fCircles.size(); ++i) {
				X = v3fCircles[i][0];
				Y = v3fCircles[i][1];
				if (!getOVal(dataX, dataY, Sz.height, Sz.width, 0, 0, v3fCircles[i][0], v3fCircles[i][1])) {
					circle(imgOriginal, Point((int)v3fCircles[i][0], (int)v3fCircles[i][1]), 3, Scalar(0, 255, 0), CV_FILLED);
					circle(imgOriginal, Point((int)v3fCircles[i][0], (int)v3fCircles[i][1]), (int)v3fCircles[i][2], Scalar(255, 0, 0), 4);
					break;
				}
				//dataX = char((X - Sz.width / 2.0) * 256 / Sz.width);
				//dataY = char((Y - Sz.height / 2.0) * 256 / Sz.height);
				//cout << "( " << (int)dataX << " , " << (int)dataY << " )        ";
				//getOVal(dataX, dataY, Sz.height, Sz.width, 0, 0, v3fCircles[0][0], v3fCircles[0][1]);
				//cout << "( " << (int)dataX << " , " << (int)dataY << " ) " << endl;
			}
			//draw all points
			for (int i = 0; i < setPtX.size(); ++i) {
				int displayX, displayY;
				displayX = setPtX[i] * Sz.width*ratio[0] / 2 + Sz.width / 2 + offset[0];
				displayY = setPtY[i] * Sz.height*ratio[1] / 2 + Sz.height / 2 + offset[1];
				circle(imgOriginal, Point((int)displayX, (int)displayY), 2, Scalar(0, 100, 255), CV_FILLED);
			}
			//draw current point
			if (currentStep < setPtX.size()) {
				int displayX, displayY;
				displayX = setPtX[currentStep] * Sz.width*ratio[0] / 2 + Sz.width / 2 + offset[0];
				displayY = setPtY[currentStep] * Sz.height*ratio[1] / 2 + Sz.height / 2 + offset[1];
				circle(imgOriginal, Point((int)displayX, (int)displayY), 3, Scalar(0, 255, 255), CV_FILLED);
			}
			else {
				currentStep = 0;
			}
			//pidX.val = dataX;
			//pidY.val = dataY;
			//dataX = ((pidX.getPIDVal()- (pidX.maxO + pidX.minO )/2)*255/(pidX.maxO-pidX.minO))-1;
			//dataY = ((pidY.getPIDVal() - (pidY.maxO + pidY.minO) / 2) * 255 / (pidY.maxO - pidY.minO))-1;
			//cout << (int)dataX << " , " << (int)dataY << " )       ( ";

			arduino.writeSerialPort(&dataX, 1/*MAX_DATA_LENGTH*/);
			arduino.writeSerialPort(&dataY, 1/*MAX_DATA_LENGTH*/);
			//y lines
			line(imgOriginal, Point(0, (Sz.height / 2) + offset[1] - Sz.height*ratio[1] / 2), Point(Sz.width, (Sz.height / 2) + offset[1] - Sz.height*ratio[1] / 2), Scalar(0, 0, 255));
			line(imgOriginal, Point(0, (Sz.height / 2) + offset[1] + Sz.height*ratio[1] / 2), Point(Sz.width, (Sz.height / 2) + offset[1] + Sz.height*ratio[1] / 2), Scalar(0, 0, 255));
			//x lines
			line(imgOriginal, Point((Sz.width / 2) + offset[0] - Sz.width*ratio[0] / 2, 0), Point((Sz.width / 2) + offset[0] - Sz.width*ratio[0] / 2, Sz.height), Scalar(255, 100, 0));
			line(imgOriginal, Point((Sz.width / 2) + offset[0] + Sz.width*ratio[0] / 2, 0), Point((Sz.width / 2) + offset[0] + Sz.width*ratio[0] / 2, Sz.height), Scalar(255, 100, 0));
			//credits
			//putText(imgOriginal, "*** Code implemented by - Abhishek Khurana ***", Point(0, 25), FONT_HERSHEY_SIMPLEX, 0.79, Scalar(0, 0, 0));



			namedWindow("imgOriginal", CV_WINDOW_AUTOSIZE);
			namedWindow("imgThresh", CV_WINDOW_AUTOSIZE);
			imshow("imgOriginal", imgOriginal);
			imshow("imgThresh", imgThresh);
			char output;
			arduino.readSerialPort(&output, 1/*MAX_DATA_LENGTH*/);
			//cout << (int)output<<" , ";
			arduino.readSerialPort(&output, 1/*MAX_DATA_LENGTH*/);
			//cout << (int)output <<" ) "<< endl;
			v3fCircles.resize(0);
			checkForEscKey = waitKey(1);
			switch (checkForEscKey)
			{
			case 'a':
				ratio[0] -= 0.01;
				break;
			case 'd':
				ratio[0] += 0.01;
				break;
			case 'A':
				offset[0] -= 5;
				break;
			case 'D':
				offset[0] += 5;
				break;
			case 'w':
				ratio[1] -= 0.01;
				break;
			case 's':
				ratio[1] += 0.01;
				break;
			case 'W':
				offset[1] -= 5;
				break;
			case 'S':
				offset[1] += 5;
				break;
			case 'N':
				currentStep++;
				break;
			case 'R':
				goto start;
				break;
			}
			//cout << currentStep << endl;
		}
	}
	return 0;
}
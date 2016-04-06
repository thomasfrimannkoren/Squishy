#include <QApplication>
#include <QtCore>

#include <opencv2/opencv.hpp>

#include <iostream>

#include "image.h"

using namespace std;

int main(int argc, char* argv[]){
//	QCoreApplication app(argc, argv);

	CARImage* img = new CARImage("test.png");
	cout << "Before calulations" << endl;
	img->calculateWeights();
	img->calculatePaths();
	cout << "After calculation" << endl;
	img->printWeights();
	img->printPaths();
	cout << "Now saving shit" << endl;
	cv::Mat result = img->getWeights();

	cv::imwrite("weights.jpeg", result);
	delete img;
	return 0;
//	return app.exec();
}

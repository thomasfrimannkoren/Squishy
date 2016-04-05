#include <QApplication>
#include <QtCore>

#include <opencv2/opencv.hpp>

#include "image.h"

int main(int argc, char* argv[]){
	QApplication app(argc, argv);

	CARImage* img = new CARImage("cross.jpeg");
	img->calculateWeights();
	cv::Mat result = img->getWeights();

	cv::imwrite("weights.jpeg", result);
	delete img;
	return app.exec();
}

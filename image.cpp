#include "image.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

using namespace cv;
using namespace std;

typedef struct _weightedPoint weightedPoint;

typedef struct _weightedPoint{
	weightedPoint* previous;
	int weight;
}:

CARImage::CARImage(const string& filename, QObject* parent) : QObject(parent){
	this->originalImage = imread(filename, CV_LOAD_IMAGE_COLOR);
}

void CARImage::calculateWeights(void){
	Mat imageGray, gradX, gradY, gradXTemp, gradYTemp, gradXf, gradYf;
	int depth = CV_16S;
	int kernelSize = 1;
	cvtColor(this->originalImage, imageGray, CV_BGR2GRAY);

	//Gradient X
	Sobel(imageGray, gradX, depth, 1, 0, kernelSize, 1, 0);
	convertScaleAbs(gradX, gradXTemp);
	gradXTemp.convertTo(gradXf, CV_32F);

	//Gradient Y
	Sobel(imageGray, gradY, depth, 0, 1, kernelSize, 1, 0);
	convertScaleAbs(gradY, gradYTemp);
	gradYTemp.convertTo(gradYf, CV_32F);

	addWeighted(gradXf, 0.5, gradYf, 0.5, 0, this->weights);
//	Mat gradXSquared, gradYSquared;
//	pow(gradXf, 2, gradXSquared);
//	pow(gradYf, 2, gradYSquared);
//
//	Mat gradSquared;
//	add(gradXSquared, gradYSquared, gradSquared);
//
//	sqrt(gradSquared, this->weights);
}

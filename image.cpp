#include "image.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <iomanip>

using namespace cv;
using namespace std;


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

void CARImage::calculatePaths(void){
	int rows = this->weights.rows;
	int cols = this->weights.cols;
	
	verticalPaths.resize(cols);
	horizontalPaths.resize(rows);

	//uchar* weights;
	weightedPoint v[rows*cols];

	float* weights = new float[rows*cols];
	for(int i = 0; i < rows; ++i){
		float* r = this->weights.ptr<float>(i);
		for(int j = 0; j < cols; ++j){
			weights[i*cols+j] = *r;
			r++;
		}
	}

	for(int i = 0; i < rows; ++i){
		for(int j = 0; j < cols; ++j){
			v[i*cols+j].pos.x = i;
			v[i*cols+j].pos.y = j;
		}
	}

	
	this->calculateVerticalPaths(weights, v, rows, cols);
	this->calculateHorizontalPaths(weights, v, rows, cols);
}

void CARImage::calculateVerticalPaths(float* weights, weightedPoint* v, int rows, int cols){
	
	for(int i = 0; i < rows; ++i){
		for(int j = 0; j < cols; ++j){
			//Set the weights of the first row
			if ( i == 0 ) {
				v[i*cols+j].weight = weights[i*cols+j];
				v[i*cols+j].previous = 0;
				continue;
			}

			weightedPoint* prev = &(v[(i-1)*cols+j]);
			if ( j != 0 && (v[(i-1)*cols+(j-1)].weight < prev->weight) ){
				prev = &(v[(i-1)*cols+(j-1)]);
			}
			if ( j != (cols-1) && (v[(i-1)*cols+(j+1)].weight < prev->weight) ){
				prev = &(v[(i-1)*cols+(j+1)]);
			}
			cout << "Point: [" << j << "," << i << "]" << "points to [" << prev->pos.x << "," << prev->pos.y << "] with weight: " << prev->weight << endl;
//			//Find the start and end values to consider in the above row
//			int k_start = j != 0 ? j-1 : 0;
//			int k_end = j!= (cols-1) ? j+1 : (cols-1);
//				
//			//Find the best possible path to one of three above points
//			weightedPoint* prev = &(v[(i-1)*cols+k_start]);
//			for (int k = k_start+1; k <= k_end; ++k){
//				if ( v[(i-1)*cols+k].weight < prev->weight ){
//					prev = &(v[(i-1)*cols+k]);
//				}
//			}
			v[i*cols+j].previous = prev;
			v[i*cols+j].weight = prev->weight + weights[i*cols+j];
		}
	}
	for(int i = 0; i < cols; ++i){
		this->verticalPaths[i].path.clear();
		weightedPoint* current = &(v[cols*(rows-1)+i]);
		this->verticalPaths[i].weight = current->weight;
		while(current){
			this->verticalPaths[i].path.push_back(current->pos);
			current = current->previous;
		}
	}
}

void CARImage::calculateHorizontalPaths(float* weights, weightedPoint* v, int rows, int cols){
	for(int i = 0; i < cols; ++i){
		for(int j = 0; j < rows; ++j){
			if (i == 0){
				v[j*cols+i].weight = weights[j*cols+i];
				v[j*cols+i].previous = 0;
				continue;
			}
//			int k_start = j != 0 ? j-1 : 0;
//			int k_end = j != (rows-1) ? j+1 : (rows-1);

			weightedPoint* prev = &(v[j*cols+(i-1)]);
			if ( j != 0 && (v[(j-1)*cols+(i-1)].weight < prev->weight) ){
				prev = &(v[(j-1)*cols+(i-1)]);
			}
			if ( j != (rows-1) && (v[(j+1)*cols+(i-1)].weight < prev->weight) ){
				prev = &(v[(j+1)*cols+(i-1)]);
			}
			cout << "Point: [" << j << "," << i << "]" << "points to [" << prev->pos.x << "," << prev->pos.y << "] with weight: " << prev->weight << endl;

//			weightedPoint* prev = &(v[(j-1)*cols+k_start]);
//			for(int k = k_start+1; k <= k_end; ++k){
//				if ( v[(j-1)*cols+k].weight < prev->weight ){
//					prev = &(v[(i-1)*cols+k]);
//				}
//			}
			v[j*cols+i].previous = prev;
			v[j*cols+i].weight = prev->weight + weights[j*cols+i];
		}
	}
	for(int i = 0; i < rows; ++i){
		this->horizontalPaths[i].path.clear();
		weightedPoint* current = &(v[i*cols+(cols-1)]);
		this->horizontalPaths[i].weight = current->weight;
		while(current){
			this->horizontalPaths[i].path.push_back(current->pos);
			current = current->previous;
		}
	}
}


void CARImage::printWeights(void){
	int cols = this->weights.cols;
	int rows = this->weights.rows;

	cout << "=====WEIGHTS=====" << endl;
//	for(int i = 0; i < rows; ++i){
//		cout << '|';
//		for(int j = 0; j < cols; ++j){
//			cout << setfill(' ') << setw(5) << this->weights.at<uchar>(i, j);
//			cout << '|';
//		}
//		cout << endl;
//	}
	cout << this->weights << endl;
}

void CARImage::printPaths(void){
	vector<path_t>::iterator it;
	cout << "=====Vertical Paths=====" << endl;
	for(it = verticalPaths.begin(); it != verticalPaths.end(); ++it){
		list<point_t>::iterator jt;
		cout << "W:" << it->weight;
		for(jt = it->path.begin(); jt != it->path.end(); ++jt){
			cout << "->[" << jt->x << ',' << jt->y << ']';
		}
		cout << endl;
	}
	cout << "=====Horizontal Paths=====" << endl;
	for(it = horizontalPaths.begin(); it != horizontalPaths.end(); ++it){
		list<point_t>::iterator jt;
		cout << "W:" << it->weight;
		for(jt = it->path.begin(); jt != it->path.end(); ++jt){
			cout << "->[" << jt->x << ',' << jt->y << ']';
		}
		cout << endl;
	}
}
			

				




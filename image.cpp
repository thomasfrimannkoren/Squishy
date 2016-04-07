#include "image.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <iomanip>
#include <algorithm>

using namespace cv;
using namespace std;

QImage Mat2QImage(const cv::Mat3b &src) {
        QImage dest(src.cols, src.rows, QImage::Format_ARGB32);
        for (int y = 0; y < src.rows; ++y) {
                const cv::Vec3b *srcrow = src[y];
                QRgb *destrow = (QRgb*)dest.scanLine(y);
                for (int x = 0; x < src.cols; ++x) {
                        destrow[x] = qRgba(srcrow[x][2], srcrow[x][1], srcrow[x][0], 255);
                }
        }
        return dest;
}

//QImage Mat2QImage(const cv::Mat_<double> &src)
//{
//	double scale = 255.0;
//	QImage dest(src.cols, src.rows, QImage::Format_ARGB32);
//	for (int y = 0; y < src.rows; ++y) {
//		const double *srcrow = src[y];
//		QRgb *destrow = (QRgb*)dest.scanLine(y);
//		for (int x = 0; x < src.cols; ++x) {
//			unsigned int color = srcrow[x] * scale;
//			destrow[x] = qRgba(color, color, color, 255);
//		}
//	}
//	return dest;
//}

CARImage::CARImage(const string& filename, QObject* parent) : QObject(parent){
	this->originalImage = imread(filename, CV_LOAD_IMAGE_COLOR);

	int cols = this->originalImage.cols;
	int rows = this->originalImage.rows;

	if (cols > 400 && cols >= rows){
		float aspectRatio = (float)rows / (float)cols;
		Size size(400,(int)(400.0*aspectRatio));
		cv::resize(this->originalImage, this->originalImage, size, 0, 0, CV_INTER_LANCZOS4);
	}
	else if (rows > 400 && rows >= cols){
		float aspectRatio = (float)cols / (float)rows;
		Size size((int)(400.0*aspectRatio), 400);
		cv::resize(this->originalImage, this->originalImage, size, 0, 0, CV_INTER_LANCZOS4);
	}

	this->originalImage.copyTo(editedImage);
}

void CARImage::calculateWeights(Mat* image){
	Mat imageGray, gradX, gradY, gradXTemp, gradYTemp, gradXf, gradYf;
	int depth = CV_16S;
	int kernelSize = 1;
	cvtColor(*image, imageGray, CV_BGR2GRAY);

	//Gradient X
	Sobel(imageGray, gradX, depth, 1, 0, kernelSize, 1, 0);
	convertScaleAbs(gradX, gradXTemp);
	gradXTemp.convertTo(gradXf, CV_32F);

	//Gradient Y
	Sobel(imageGray, gradY, depth, 0, 1, kernelSize, 1, 0);
	convertScaleAbs(gradY, gradYTemp);
	gradYTemp.convertTo(gradYf, CV_32F);

	addWeighted(gradXf, 0.5, gradYf, 0.5, 0, this->weights);
}

void CARImage::calculatePaths(int dir){
	int rows = this->weights.rows;
	int cols = this->weights.cols;
    cout << "Rows: " << rows << " Cols: " << cols << endl;

	verticalPaths.resize(cols);
	horizontalPaths.resize(rows);

	//uchar* weights;
	weightedPoint v[rows*cols];

    float* w;
    w = new float[rows*cols];
    cout << w << endl;
	for(int i = 0; i < rows; ++i){
        float* r = this->weights.ptr<float>(i);
		for(int j = 0; j < cols; ++j){
            w[i*cols+j] = r[j];
		}
    }

	for(int i = 0; i < rows; ++i){
		for(int j = 0; j < cols; ++j){
			v[i*cols+j].pos.x = i;
			v[i*cols+j].pos.y = j;
		}
	}

	if(!dir){
        this->calculateVerticalPaths(w, v, rows, cols);
	}
	else{
        this->calculateHorizontalPaths(w, v, rows, cols);
	}
    delete[] w;
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


			weightedPoint* prev = &(v[j*cols+(i-1)]);
			if ( j != 0 && (v[(j-1)*cols+(i-1)].weight < prev->weight) ){
				prev = &(v[(j-1)*cols+(i-1)]);
			}
			if ( j != (rows-1) && (v[(j+1)*cols+(i-1)].weight < prev->weight) ){
				prev = &(v[(j+1)*cols+(i-1)]);
			}

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

	cout << "=====WEIGHTS=====" << endl;

	cout << setfill(' ') << setw(5) <<this->weights << endl;
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

void CARImage::shrinkWidthBy1Px(void){
	this->calculateWeights(&editedImage);
	this->calculatePaths(0);
	int cols = this->weights.cols;
	int rows = this->weights.rows;
	int rmPix[rows];
	path_t* shortestPath = &(verticalPaths[1]);
	vector<path_t>::iterator it;
	for(it = verticalPaths.begin(); it != verticalPaths.end(); ++it){
		if (it->weight < shortestPath->weight){
			shortestPath = &(*it);
		}
	}
	removedVPaths.push_back(*shortestPath);
	list<point_t>::iterator lit;
	cout << "Will delete the following path: ";
	for(lit = shortestPath->path.begin(); lit != shortestPath->path.end(); ++lit){
		rmPix[lit->x] = lit->y;
		cout << "->[" << lit->x << ',' << lit->y << ']';
	}
	cout << endl;
    Mat newImage;
    editedImage.copyTo(newImage);
	for (int i = 0; i < rows; ++i){
        Vec3b* pixel = newImage.ptr<Vec3b>(i);
		for(int j = 0; j < (cols-1); ++j){
			if( j >= rmPix[i] ){
				pixel[j][0] = pixel[j+1][0];	
				pixel[j][1] = pixel[j+1][1];	
				pixel[j][2] = pixel[j+1][2];	
			}
		}
	}
    newImage(Rect(0,0,cols-1, rows)).copyTo(editedImage);
}

void CARImage::shrinkHeightBy1Px(void){
    this->calculateWeights(&editedImage);
    this->calculatePaths(1);
	int cols = this->weights.cols;
	int rows = this->weights.rows;
	int rmPix[cols];

	path_t* shortestPath = &(horizontalPaths[1]);
	vector<path_t>::iterator it;
	for(it = horizontalPaths.begin(); it != horizontalPaths.end(); ++it){
		if (it->weight < shortestPath->weight){
			shortestPath = &(*it);
		}
	}

	list<point_t>::iterator lit;
	for(lit = shortestPath->path.begin(); lit != shortestPath->path.end(); ++lit){
		rmPix[lit->y] = lit->x;
	}

	Mat newImage;
	editedImage.copyTo(newImage);
	for (int i = 0; i < cols; ++i){
		for(int j = 0; j < (rows-1); ++j){
			if( j >= rmPix[j] ){
				Vec3b* tPixel = newImage.ptr<Vec3b>(j);
				Vec3b* nPixel = newImage.ptr<Vec3b>(j+1);
				tPixel[i][0] = nPixel[i][0];
				tPixel[i][1] = nPixel[i][1];
				tPixel[i][2] = nPixel[i][2];
			}
		}
	}
	newImage(Rect(0,0,cols, rows-1)).copyTo(editedImage);
}

void CARImage::saveEdited(const std::string& filename){
	imwrite(filename, this->editedImage);
}

void CARImage::saveGradient(const std::string& filename){
	calculateWeights(&originalImage);
	imwrite(filename, this->weights);
}

QImage CARImage::getOriginal(void){
	return Mat2QImage(this->originalImage);
}

QImage CARImage::getEdited(void){
	return Mat2QImage(this->editedImage);
}
QImage CARImage::getGradient(void){
	calculateWeights(&originalImage);
	Mat temp;
	cvtColor(weights, temp, CV_GRAY2BGR);
	return Mat2QImage(temp);
}

void CARImage::resize(int width, int height){
	if(width > editedImage.cols || height > editedImage.rows){
		originalImage.copyTo(editedImage);
	}

	int widthIter = editedImage.cols - width;
	for(int i = 0; i < widthIter; ++i){
		shrinkWidthBy1Px();
	}

	int heightIter = editedImage.rows - height;
	for(int i = 0; i < heightIter; ++i){
		shrinkHeightBy1Px();
	}
}

void CARImage::restore(void){
	originalImage.copyTo(editedImage);
}

QImage CARImage::getVertPaths(void){
	calculateWeights(&originalImage);
	calculatePaths(0);

	//sort(removedVPaths.begin(), removedVPaths.end());

	int nPaths = removedVPaths.size();

	Mat retVal;
	originalImage.copyTo(retVal);

	path_t* e;
	for(int i = 0; i < nPaths; ++i){
		e = &(removedVPaths[i]);
		list<point_t>::iterator it;
		for(it = e->path.begin(); it != e->path.end(); ++it){
			Vec3b* pix = retVal.ptr<Vec3b>(it->x, it->y);
			pix[0] = 0;
			pix[1] = 0;
			pix[2] = 0;
		}
	}
	return Mat2QImage(retVal);
}

	

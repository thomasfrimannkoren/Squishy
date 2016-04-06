#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <string>
#include <opencv2/opencv.hpp>

#include <QObject>

typedef struct _point_t{
	int x;
	int y;
}point_t;

typedef struct _path_t{
	std::list<point_t> path;
	float weight;
}path_t;

typedef struct _weightedPoint weightedPoint;

struct _weightedPoint{
	weightedPoint* previous;
	float weight;
	point_t pos;
};

class CARImage : public QObject {
Q_OBJECT
	private:
		cv::Mat originalImage;
		cv::Mat editedImage;
		cv::Mat weights;
		std::vector<path_t> verticalPaths;
		std::vector<path_t> horizontalPaths;
	public:
		void calculateWeights(void);
		void calculatePaths(void);
		void calculateVerticalPaths(float* weights, weightedPoint* v, int rows, int cols);
		void calculateHorizontalPaths(float* weights, weightedPoint* v, int rows, int cols);
	public:
		CARImage(const std::string& filename, QObject* parent = 0);
		cv::Mat getEdited(void);
		cv::Mat getWeights(void){return this->weights;};
		cv::Mat getWeightsAsImage(void);
		cv::Mat getPaths(void);

		void printWeights(void);
		void printPaths(void);
};

#endif //_IMAGE_H_

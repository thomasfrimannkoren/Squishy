#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <string>
#include <opencv2/opencv.hpp>

#include <QObject>
#include <QImage>

typedef struct _point_t{
	int x;
	int y;
}point_t;

typedef struct _path_t path_t;
struct _path_t{
	std::list<point_t> path;
	float weight;

	bool operator<(const path_t& a){
		return weight < a.weight;
	}
};

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
		std::vector<path_t> removedVPaths;
	public:
		void calculateWeights(cv::Mat* image);
		void calculatePaths(int dir);
		void calculateVerticalPaths(float* weights, weightedPoint* v, int rows, int cols);
		void calculateHorizontalPaths(float* weights, weightedPoint* v, int rows, int cols);
		void shrinkWidthBy1Px(void);
		void shrinkHeightBy1Px(void);
	public:
		CARImage(const std::string& filename, QObject* parent = 0);
		void saveEdited(const std::string& filename);
		void saveGradient(const std::string& filename);
		void saveVertPaths(const std::string& filename);
		void saveHorPaths(const std::string& filename);
		QImage getOriginal(void);
		QImage getEdited(void);
		QImage getGradient(void);
		QImage getVertPaths(void);
		QImage getHorPaths(void);

		void resize(int width, int height);
		void restore(void);

		int getWidth(void){return originalImage.cols;};
		int getHeight(void){return originalImage.rows;};

		void shrinkWidth(int newWidth);
		void shrinkHeigh(int newHeight);

		void printWeights(void);
		void printPaths(void);
};

#endif //_IMAGE_H_

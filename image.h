#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <string>
#include <opencv2/opencv.hpp>

#include <QObject>


class CARImage : public QObject {
Q_OBJECT
	private:
		cv::Mat originalImage;
		cv::Mat editedImage;
		cv::Mat weights;
	public:
		void calculateWeights(void);
		void calculatePaths(void);
	public:
		CARImage(const std::string& filename, QObject* parent = 0);
		cv::Mat getEdited(void);
		cv::Mat getWeights(void){return this->weights;};
		cv::Mat getWeightsAsImage(void);
		cv::Mat getPaths(void);
};

#endif //_IMAGE_H_

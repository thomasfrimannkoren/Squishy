#include <QApplication>
#include <QtCore>

#include <opencv2/opencv.hpp>

#include <iostream>

#include "image.h"
#include "mainwindow.h"

using namespace std;

int main(int argc, char* argv[]){
	QApplication app(argc, argv);

	MainWindow wnd;
	wnd.show();

	return app.exec();
}

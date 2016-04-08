#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QGraphicsScene>
#include <QPixmap>

#include "image.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

	CARImage* image;

	QImage original;
	QPixmap originalPixmap;
	QGraphicsScene *originalScene;

	QImage edited;
	QPixmap editedPixmap;
	QGraphicsScene *editedScene;

	QImage gradient;
	QPixmap gradientPixmap;
	QGraphicsScene* gradientScene;

	QImage vp;
	QPixmap vpPixmap;
	QGraphicsScene* vpScene;

	QImage hp;
	QPixmap hpPixmap;
	QGraphicsScene* hpScene;
private slots:
	void on_transform_clicked();
	void on_restore_clicked();
	void on_openNew_clicked();
	void on_saveEdited_clicked();
	void on_saveGradient_clicked();
	void on_saveVPaths_clicked();
	void on_saveHPaths_clicked();
};

#endif // MAINWINDOW_H

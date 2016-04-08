#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

	image = 0;

	ui->selectX->setRange(0,0);
	ui->selectY->setRange(0,0);

}

MainWindow::~MainWindow()
{
    delete ui;
	delete image;
}
	

void MainWindow::on_transform_clicked(){
	if (image == 0) return;
	int width = ui->selectX->value();
	int height = ui->selectY->value();

	image->resize(width, height);

	delete editedScene;
//	delete vpScene;

	edited = image->getEdited();
	
	editedPixmap = QPixmap::fromImage(edited);

	editedScene = new QGraphicsScene(this);
	editedScene->addPixmap(editedPixmap);
	editedScene->setSceneRect(editedPixmap.rect());
	ui->editedView->setScene(editedScene);
	ui->editedView->fitInView(ui->editedView->sceneRect(), Qt::KeepAspectRatio);

//	vp = image->getVertPaths();
//
//	vpPixmap = QPixmap::fromImage(vp);
//
//	vpScene = new QGraphicsScene(this);
//	vpScene->addPixmap(vpPixmap);
//	vpScene->setSceneRect(vpPixmap.rect());
//	ui->verPathsView->setScene(vpScene);
//	ui->verPathsView->fitInView(ui->verPathsView->sceneRect(), Qt::KeepAspectRatio);
}

void MainWindow::on_restore_clicked(){
	if (image == 0) return;
	
	image->restore();

	ui->selectX->setValue(image->getWidth());
	ui->selectY->setValue(image->getHeight());

	delete editedScene;
//	delete vpScene;

	edited = image->getEdited();
	
	editedPixmap = QPixmap::fromImage(edited);

	editedScene = new QGraphicsScene(this);
	editedScene->addPixmap(editedPixmap);
	editedScene->setSceneRect(editedPixmap.rect());
	ui->editedView->setScene(editedScene);
	ui->editedView->fitInView(ui->editedView->sceneRect(), Qt::KeepAspectRatio);
//
//	vp = image->getVertPaths();
//
//	vpPixmap = QPixmap::fromImage(vp);
//
//	vpScene = new QGraphicsScene(this);
//	vpScene->addPixmap(vpPixmap);
//	vpScene->setSceneRect(vpPixmap.rect());
//	ui->verPathsView->setScene(vpScene);
//	ui->verPathsView->fitInView(ui->verPathsView->sceneRect(), Qt::KeepAspectRatio);
}
void MainWindow::on_openNew_clicked(){
	if(image != 0){
		delete image;

		delete originalScene;

		delete editedScene;

		delete gradientScene;

		delete vpScene;

		delete hpScene;
	}
	QString file = QFileDialog::getOpenFileName(this, tr("Open file!"), "./", tr("Images (*.png *.jpg *.jpeg *.bmp)"));
	if (file ==0) return;

	image = new CARImage(file.toUtf8().constData(), this);

	ui->selectX->setMaximum(image->getWidth());
	ui->selectY->setMaximum(image->getHeight());
	ui->selectX->setValue(image->getWidth());
	ui->selectY->setValue(image->getHeight());

	original = image->getOriginal();

	originalPixmap = QPixmap::fromImage(original);

	originalScene = new QGraphicsScene(this);
	originalScene->addPixmap(originalPixmap);
	originalScene->setSceneRect(originalPixmap.rect());
	ui->originalView->setScene(originalScene);
	ui->originalView->fitInView(ui->originalView->sceneRect(), Qt::KeepAspectRatio);

	gradient = image->getGradient();

	gradientPixmap = QPixmap::fromImage(gradient);

	gradientScene = new QGraphicsScene(this);
	gradientScene->addPixmap(gradientPixmap);
	gradientScene->setSceneRect(gradientPixmap.rect());
	ui->weightView->setScene(gradientScene);
	ui->weightView->fitInView(ui->weightView->sceneRect(), Qt::KeepAspectRatio);

	edited = image->getEdited();
	
	editedPixmap = QPixmap::fromImage(edited);

	editedScene = new QGraphicsScene(this);
	editedScene->addPixmap(editedPixmap);
	editedScene->setSceneRect(editedPixmap.rect());
	ui->editedView->setScene(editedScene);
	ui->editedView->fitInView(ui->editedView->sceneRect(), Qt::KeepAspectRatio);

	vp = image->getVertPaths();

	vpPixmap = QPixmap::fromImage(vp);

	vpScene = new QGraphicsScene(this);
	vpScene->addPixmap(vpPixmap);
	vpScene->setSceneRect(vpPixmap.rect());
	ui->vpView->setScene(vpScene);
	ui->vpView->fitInView(ui->vpView->sceneRect(), Qt::KeepAspectRatio);

	hp = image->getHorPaths();

	hpPixmap = QPixmap::fromImage(hp);

	hpScene = new QGraphicsScene(this);
	hpScene->addPixmap(hpPixmap);
	hpScene->setSceneRect(hpPixmap.rect());
	ui->hpView->setScene(hpScene);
	ui->hpView->fitInView(ui->hpView->sceneRect(), Qt::KeepAspectRatio);
}
void MainWindow::on_saveEdited_clicked(){
	if (image == 0) return;
	QString file = QFileDialog::getSaveFileName(this, tr("Save file!"), "./", tr("Images (*.png *.jpg *.jpeg *.bmp)"));
	if (file ==0) return;
	image->saveEdited(file.toUtf8().constData());
}

void MainWindow::on_saveGradient_clicked(){
	if (image == 0) return;
	QString file = QFileDialog::getSaveFileName(this, tr("Save file!"), "./", tr("Images (*.png *.jpg *.jpeg *.bmp)"));
	if (file ==0) return;
	image->saveGradient(file.toUtf8().constData());
}

void MainWindow::on_saveVPaths_clicked(){
	if (image == 0) return;
	QString file = QFileDialog::getSaveFileName(this, tr("Save file!"), "./", tr("Images (*.png *.jpg *.jpeg *.bmp)"));
	if (file ==0) return;
	image->saveVertPaths(file.toUtf8().constData());
}

void MainWindow::on_saveHPaths_clicked(){
	if (image == 0) return;
	QString file = QFileDialog::getSaveFileName(this, tr("Save file!"), "./", tr("Images (*.png *.jpg *.jpeg *.bmp)"));
	if (file ==0) return;
	image->saveHorPaths(file.toUtf8().constData());
}

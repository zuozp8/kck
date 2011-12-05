#include "widget.hpp"
#include <QTcpSocket>
#include <QApplication>

#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.h>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
	s = new QTcpSocket(this);
	QString host = QApplication::argc()>1
				   ?QApplication::arguments().at(1)
				   :"127.0.0.1";
	s->connectToHost(host,1234);
	stream.setDevice(s);
	stream<<"C 0 255 255"<<endl;
	stream<<"C 255 0 255"<<endl;


	QGridLayout* layout= new QGridLayout(this);

	layout->addWidget(new QLabel("Kierunek",this),0,0);
	setGeometry(0,0,600,140);

	slider = new QSlider(Qt::Horizontal,this);
	slider->setRange(-310,310);
	layout->addWidget(slider,0,1,1,2);

	layout->addWidget(new QLabel("Treshold",this),1,0);

	tresholdSlider = new QSlider(Qt::Horizontal,this);
	tresholdSlider->setRange(50,180);
	tresholdSlider->setSliderPosition(140);
	layout->addWidget(tresholdSlider,1,1,1,2);

	fpsCounter = new QLabel("FPS",this);
	layout->addWidget(fpsCounter,2,0);

	huButton = new QPushButton("HU",this);
	layout->addWidget(huButton,2,1);

	normalButton = new QPushButton("Normalne",this);
	layout->addWidget(normalButton,2,2);

	cvNamedWindow("Widok");
	cvNamedWindow("Widok2");
	cameraThread = new CameraThread(this);
	cameraThread->setTreshold(tresholdSlider->value());
	connect(cameraThread,SIGNAL(showFPS(int)),SLOT(showFPS(int)),Qt::QueuedConnection);
	connect(cameraThread,SIGNAL(setAngle(double)),SLOT(setAngle(double)),Qt::QueuedConnection);
	connect(cameraThread,SIGNAL(showFrame(IplImage*,IplImage*)),SLOT(showFrame(IplImage*,IplImage*)),Qt::QueuedConnection);
	connect(cameraThread,SIGNAL(changeTreshold()),SLOT(changeTreshold()),Qt::QueuedConnection);
	connect(huButton,SIGNAL(clicked()),cameraThread,SLOT(makeHu()),Qt::QueuedConnection);
	connect(normalButton,SIGNAL(clicked()),cameraThread,SLOT(makeNormal()),Qt::QueuedConnection);
	connect(tresholdSlider,SIGNAL(valueChanged(int)),cameraThread,SLOT(setTreshold(int)),Qt::QueuedConnection);
	cameraThread->start();
}

Widget::~Widget()
{

}

void Widget::setAngle(double a) {
	stream<<"A "<<a<<endl;
	slider->setValue(a*100);
}

void Widget::showFrame(IplImage * image, IplImage* image3) {
	cvShowImage("Widok", image);
	cvReleaseImage(&image);
	cvShowImage("Widok2", image3);
	cvReleaseImage(&image3);
}

void Widget::showFPS(int fps) {
	fpsCounter->setText(QString("FPS: %1").arg(fps));
}

void Widget::changeTreshold() {
	tresholdSlider->setValue(tresholdSlider->value()+(qrand()%2?10:-10));
}

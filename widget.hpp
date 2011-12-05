#ifndef WIDGET_HPP
#define WIDGET_HPP

#include <QtGui>

class QTcpSocket;
#include <QTextStream>
#include "camerathread.hpp"

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
	~Widget();
private:
	QTcpSocket* s;
	QTextStream stream;
	QSlider* slider,* tresholdSlider;
	CameraThread* cameraThread;
	QPushButton* huButton,* normalButton;
	QLabel* fpsCounter;
public slots:
	void setAngle(double);
	void showFrame(IplImage*,IplImage*);
	void showFPS(int);
	void changeTreshold();
};

#endif // WIDGET_HPP

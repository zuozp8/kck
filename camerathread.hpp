#ifndef CAMERATHREAD_HPP
#define CAMERATHREAD_HPP

#include <QThread>
#include <opencv/cv.h>

class CameraThread : public QThread
{
    Q_OBJECT
public:
    explicit CameraThread(QObject *parent = 0);
	void run();

signals:
	void showFrame(IplImage*,IplImage*);
	void showFPS(int);
	void setAngle(double);
	void changeTreshold();
public slots:
	void setTreshold(int);
	void makeHu();
	void makeNormal();
private:
	int treshold;
	bool hu, normal;
};

#endif // CAMERATHREAD_HPP

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/highgui.h>
#include <opencv/cv.h>

#include "camerathread.hpp"

#include <QDebug>

#include <ctime>

#define CHECK 0.2

CameraThread::CameraThread(QObject *parent) :
    QThread(parent)
{
	hu=0;
	normal=0;
}


void CameraThread::run() {

	CvCapture * camera = cvCreateCameraCapture(0);
	IplImage * image=cvQueryFrame(camera);

	int currentTime=time(0), framesCount=0;

	forever {
		if (time(0)>currentTime) {
			currentTime=time(0);
			emit showFPS(framesCount);
			framesCount = 0;
		}
		framesCount++;

		image=cvQueryFrame(camera);
		if (!image) {
			qWarning()<<"noImage";
			continue;
		}
		int w = image->width;
		int h = image->height;

		IplImage* image2 = cvCreateImage(cvSize(w,h), image->depth, 1);
		IplImage* image3 = cvCreateImage(cvSize(w,h), image->depth, 1);
		cvSet(image3, CV_RGB(255,255,255));

		//cvConvertImage(image, image, CV_CVTIMG_FLIP);

		cvCvtColor(image,image2, CV_RGB2GRAY );
		cvSmooth(image2, image2,CV_MEDIAN, 5, 5, 0, 0);

		cvThreshold(image2,image3,treshold,255,0);
		cvNot(image3,image3);

		CvFont font;
		cvInitFont(&font,CV_FONT_HERSHEY_SCRIPT_COMPLEX, 3.0, 3.0, 6);

		CvMemStorage* storage = cvCreateMemStorage(0);
		CvSeq* contour = 0;
		cvFindContours(image3, storage, &contour,sizeof(CvContour),CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
		int found=0;
		for(int i=0 ; contour != 0; contour = contour->h_next, i++ ) {

			/* replace CV_FILLED with 1 to see the outlines */
			//cvDrawContours(image3, contour, CV_RGB( 255, 255, 255 ), CV_RGB( 255, 255, 255 ), -1, CV_FILLED, 8 );
			static CvMoments* moments=new CvMoments();
			cvMoments(contour,moments);
			static CvHuMoments* huMoments=new CvHuMoments();
			cvGetHuMoments(moments,huMoments);

			CvRect r = cvBoundingRect(contour,1);
			CvPoint point = cvPoint(r.x,r.y);
			CvPoint point2 = cvPoint(r.x+r.width,r.y+r.height);
			CvPoint pointS = cvPoint(r.x+r.width/2,r.y+r.height/2);

			cvPutText(image3,QString("%1").arg(i).toStdString().c_str(),point,&font,cvScalar(150,150,150));

			double check=0;
			//check+= abs(huMoments->hu1-0.161)*10;
			//check+= abs(log10(huMoments->hu4)+7.1)/10;

			//check+= abs(huMoments->hu1-0.185)*10;
			//check+= abs(log10(huMoments->hu2)+4.7)/20;
			//check+= abs(log10(huMoments->hu3)+2.92);

			check+= abs(huMoments->hu1-0.20)*10;
			check+= abs(huMoments->hu2-0.004)*10;
			check+= abs(huMoments->hu3-0.0041)*100;

			if (r.width<100)
				check+=(100-r.width)/40;
			if (r.height<100)
				check+=(100-r.height)/40;
			if (r.height>h*3/4)
				check+=0.1;
			if (r.width>w*3/4)
				check+=0.1;
			if (check < CHECK) {
				found++;
				cvDrawContours(image2, contour, CV_RGB( 0,0,0 ), CV_RGB( 0,0,0 ), -1, CV_FILLED, 8 );
				cvRectangle(image3, point, point2, CV_RGB(120, 120, 120), 3);
				double length = sqrt(moments->mu30*moments->mu30 + moments->mu03*moments->mu03);
				double dx = moments->mu30/length, dy=moments->mu03/length;
				double angle = asin(dx);
				if (dy>0) {
					if (angle>0)
						angle = 3.14159 - angle;
					else
						angle = -3.14159 - angle;
				}
				// Angle hint
				CvPoint pointV = cvPoint(pointS.x-100*dx,pointS.y+100*dy);
				cvLine(image3, pointS, pointV, CV_RGB(200,200,200), 2);

				// Check hint
				pointV = pointS;
				pointV.x+=100*CHECK;
				cvLine(image3, pointS, pointV, CV_RGB(255,255,255), 2);
				pointS.y+=4;
				pointV = pointS;
				pointV.x+=100*(huMoments->hu1-0.20)*10;
				cvLine(image3, pointS, pointV, CV_RGB(200,200,200), 2);
				pointS.y+=4;
				pointV = pointS;
				pointV.x+=100*(huMoments->hu2-0.004)*10;
				cvLine(image3, pointS, pointV, CV_RGB(200,200,200), 2);
				pointS.y+=4;
				pointV = pointS;
				pointV.x+=100*(huMoments->hu3-0.0041)*100;;
				cvLine(image3, pointS, pointV, CV_RGB(200,200,200), 2);

				if (hu) {
					qDebug()<< huMoments->hu1
							<< huMoments->hu2
							<< huMoments->hu3
							<< huMoments->hu4
							<< huMoments->hu5
							<< huMoments->hu6
							<< huMoments->hu7;
					hu=0;
				}
				if (normal) {
					qDebug()<< moments->mu03
							<< moments->mu30
							<< angle;
					normal=0;
				}
				emit setAngle(angle);
			} else if (check < 2*CHECK) {
				cvDrawContours(image2, contour, CV_RGB( 100, 100, 100 ), CV_RGB( 100, 100, 100 ), -1, CV_FILLED, 8 );
			}

		}

		if (found!=1) {
			emit changeTreshold();
		}

		emit showFrame(image2,image3);

	}
}

void CameraThread::setTreshold(int t) {
	treshold = t;
}

void CameraThread::makeHu() {
	hu=1;
}

void CameraThread::makeNormal() {
	normal=1;
}

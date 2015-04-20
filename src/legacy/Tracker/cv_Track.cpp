//#include <highgui.h>
//#include <cv.h>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <stdlib.h>

IplImage* GetThresholdedImage(IplImage* img)
{
	// Convert the image into an HSV image
	IplImage* imgHSV = cvCreateImage(cvGetSize(img), 8, 3);
	cvCvtColor(img, imgHSV, CV_BGR2HSV);

	IplImage* imgThreshed = cvCreateImage(cvGetSize(img), 8, 1);

	// Values 20,100,100 to 30,255,255 for Yellow
	cvInRangeS(imgHSV, cvScalar(15, 100, 100), cvScalar(35, 255, 255), 
    //cvInRangeS(imgHSV, cvScalar(20, 100, 100), cvScalar(40, 255, 255), 
        imgThreshed);

	cvReleaseImage(&imgHSV);

	return imgThreshed;
}

int main()
{
	// Initialize capturing live feed from the camera
	CvCapture* capture = 0;
	capture = cvCaptureFromCAM(0);	

	// Couldn't get a device? Throw an error and quit
	if(!capture)
    {
        printf("Could not initialize capturing...\n");
        return -1;
    }
    cvNamedWindow("video", CV_WINDOW_NORMAL);
	cvNamedWindow("thresh", CV_WINDOW_NORMAL);
  
	IplImage* imgScribble = NULL;

	//infinite loop
	while(true)
    {
		// Will hold a frame captured from the camera
		IplImage* frame = 0;
		frame = cvQueryFrame(capture);
        cvShowImage("video", frame);
		// If we couldn't grab a frame... quit
        if(!frame)
            break;

        
		// If this is the first frame, we need to initialize it
		if(imgScribble == NULL)
		{
			imgScribble = cvCreateImage(cvGetSize(frame), 8, 3);
		}

		IplImage* imgYellowThresh = GetThresholdedImage(frame);
        cvShowImage("thresh", imgYellowThresh);
        
		// Calculate the moments to estimate the position of the ball
		CvMoments *moments = (CvMoments*)malloc(sizeof(CvMoments));
		cvMoments(imgYellowThresh, moments, 1);

		// The moment values
		double moment10 = cvGetSpatialMoment(moments, 1, 0);
		double moment01 = cvGetSpatialMoment(moments, 0, 1);
		double area = cvGetCentralMoment(moments, 0, 0);

		// Holding the last and current ball positions
		static int posX = 0;
		static int posY = 0;

		int lastX = posX;
		int lastY = posY;

		posX = moment10/area;
		posY = moment01/area;

		printf("position (%d,%d)\n", posX, posY);

		// Wait for a keypress
		int c = cvWaitKey(10);
		if(c!=-1)
		{
            break;
		}

		cvReleaseImage(&imgYellowThresh);

		delete moments;
    }

	cvReleaseCapture(&capture);
    return 0;
}

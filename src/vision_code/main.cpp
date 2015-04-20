/*
 * Author Idan Warsawski, Peter Galvin
 */

#include <string>
#include <vector>
#include <cv.h>
#include <highgui.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>


#include "hiredis/hiredis.h"
#include "../capture_blackmagic.h"
#include "../image_conversion.h"

#include "movement_logic.h"

redisContext *c;

struct movement_logic * ml;

IplImage * blackmagicImgIn;
uint8_t * rgb_tmp_buf;
struct capture_blackmagic cbm;
int picture_counter;

class FrameProcessor 
{

public:
  //processing method
  virtual void process(cv:: Mat &input, cv:: Mat &output)= 0;
};

class VideoProcessor 
{

private:

  cv::VideoCapture capture;
  void (*process)(cv::Mat&, cv::Mat&);
  FrameProcessor *frameProcessor;
  bool callIt;
  std::string windowNameInput;
  std::string windowNameOutput;
  int delay;
  long fnumber;
  long frameToStop;
  bool stop;

  //vector of image filename to be used as input
  std::vector<std::string> images; 
  //image vector iterator
  std::vector<std::string>::const_iterator itImg;

  cv::VideoWriter writer;
  // output filename
  std::string outputFile;
  // current index for output images
  int currentIndex;
  // number of digits in output image filename
  int digits;
  // extension of output images
  std::string extension;

  //holds the encoded png image we will push to the redis server
  std::vector<unsigned char> encoded_img;

public:

  //constructor setting the default values
  VideoProcessor() : callIt(false), delay(-1), 
		     fnumber(0), stop(false), digits(0), frameToStop(-1), 
		     process(0), frameProcessor(0) {}


  void setCaptureDevice(cv::VideoCapture & capDevIn)
  {
    capture = capDevIn;
  }

  //set the camera ID
  bool setInput(int id) 
  {

    fnumber= 0;
    capture.release();
    images.clear();

    //open the video file
    return capture.open(id);
  }

  bool setOutput(const std::string &filename, // filename prefix
		 const std::string &ext, // image file extension 
		 int numberOfDigits=3,   // number of digits
		 int startIndex=0) 
  {     // start index

    if (numberOfDigits<0)
      return false;

    outputFile= filename;
    extension= ext;

    digits= numberOfDigits;
    currentIndex= startIndex;

    return true;
  }

  // set the callback function that will be called for each frame
  void setFrameProcessor(void (*frameProcessingCallback)(cv::Mat&, cv::Mat&)) 
  {
    // invalidate frame processor class instance
    frameProcessor= 0;
    // this is the frame processor function that will be called
    process= frameProcessingCallback;
    callProcess();
  }

  // set the instance of the class that implements the FrameProcessor interface
  void setFrameProcessor(FrameProcessor* frameProcessorPtr) 
  {

    // invalidate callback function
    process= 0;
    // this is the frame processor instance that will be called
    frameProcessor= frameProcessorPtr;
    callProcess();
  }

  // process callback to be called
  void callProcess() 
  {

    callIt= true;
  }

  // do not call process callback
  void dontCallProcess() 
  {

    callIt= false;
  }

  // to display the processed frames
  void displayInput(std::string wn) 
  {
	    
    windowNameInput= wn;
    cv::namedWindow(windowNameInput);
  }

  // to display the processed frames
  void displayOutput(std::string wn) 
  {
	    
    windowNameOutput= wn;
    cv::namedWindow(windowNameOutput);
  }
  // a count is kept of the processed frames
  long getNumberOfProcessedFrames() 
  {
	  
    return fnumber;
  }

	
  //stop the processing
  void stopIt() 
  {

    stop= true;
  }

  bool isStopped() 
  {

    return stop;
  }

  bool isOpened() 
  {

    return capture.isOpened() || !images.empty();
  }
	  
  void run() 
  {

    //IplImage* iplImage = cvLoadImage("file path");
    //cv::Mat image4(iplImage,false);
    // current frame
    //IplImage* iplImage = cvQueryFrame(capture);
    cv::Mat frame; //(iplImage,false);
    // output frame
    cv::Mat output;
		

    // if no capture device has been set
    //if (!isOpened())
    //return;

    stop= false;

    while (!isStopped())
      {

#ifdef BLACKMAGIC_CAPTURE
	capture_blackmagic_block(&cbm);
	frame = blackmagicImgIn;
#else
	capture >> frame;
#endif


	//display input frame
	if (windowNameInput.length()!=0) 
	  cv::imshow(windowNameInput,frame);

	//calling the process function or method
	if (callIt) 
	  {
				  
	    //process the frame
	    if (process)
	      process(frame, output);
	    else if (frameProcessor) 
	      frameProcessor->process(frame,output);
	    //increment frame number
	    fnumber++;

	  } else {

	  output= frame;
	}

	//display output frame
	//if (windowNameOutput.length()!=0) 
	//cv::imshow(windowNameOutput,output);

	if(++picture_counter == 5)

	  {
            std::vector<int> p;
            p.push_back(CV_IMWRITE_JPEG_QUALITY);
            p.push_back(75);
            unsigned char * img_ptr;
	    //cv::imwrite("/var/www/status_images/input.png", output);
            cv::imencode(".jpg", output, encoded_img, p);
            img_ptr = (unsigned char *) &(encoded_img[0]);
            redisCommand(c, "SET vision:image %b", img_ptr, encoded_img.size());

	    //cvSaveImage("/var/www/status_images/skinfilter.png", skinFilter);

                                    

	    //IplImage paintimage = painting.getImage();

	    //cvSaveImage("/var/www/status_images/painting.png", &paintimage);

	    picture_counter = 0;

	  }
			
	/*if (delay>=0 && cv::waitKey(delay)>=0)
	  stopIt(); */

                   

	//check if we should stop
	/* if (frameToStop>=0 && getFrameNumber()==frameToStop)
	   stopIt();*/

	cv::waitKey(100);
      }
  }
};

class FeatureTracker : public FrameProcessor 
{
	
  cv::Mat gray;			// current gray-level image
  cv::Mat gray_prev;		//previous gray-level image
  std::vector<cv::Point2f> points[2]; // tracked features from 0->1
  std::vector<cv::Point2f> initial;   //initial position of tracked points
  std::vector<cv::Point2f> features;  //detected features
  int max_count;	  // maximum number of features to detect
  double qlevel;    // quality level for feature detection
  double minDist;   //minimum distance between two feature points
  std::vector<uchar> status; // status of tracked features
  std::vector<float> err;    // error in tracking
	

public:
	
  static int slowdown_counter;
  static int target_pan, target_tilt;

  FeatureTracker() : max_count(500), qlevel(0.01), minDist(10.) {}
	

  // processing method
  void process(cv:: Mat &frame, cv:: Mat &output) 
  {
			
	
    cv::cvtColor(frame, gray, CV_BGR2GRAY); 
    frame.copyTo(output);

    if(addNewPoints())
      {
	// detect feature points
	detectFeaturePoints();
	points[0].insert(points[0].end(),features.begin(),features.end());
	initial.insert(initial.end(),features.begin(),features.end());
      }
		
    if(gray_prev.empty())
      gray.copyTo(gray_prev);
            
    cv::calcOpticalFlowPyrLK(gray_prev, gray, 
			     points[0],
			     points[1], 
			     status,  
			     err);      // tracking error
           
    int k=0;
    cv::Point2f average;
    for(int i= 0; i < points[1].size(); i++ ) 
      {

	//do we keep this point?
	if (acceptTrackedPoint(i)) 
	  {

	    initial[k]= initial[i];
	    points[1][k++] = points[1][i];
	    average += points[1][i];
				
	  }
      }

    //eliminate unsuccesful points
    points[1].resize(k);
    initial.resize(k);
    average *= 1.0f / (float)points[1].size();
		
    //std::cout << "The average of point locations is: " << average.x << " " << average.y << std::endl;
    // set the values
    char buf[256];
    static short int boole = 0;

    //NaN bypass    
    if(average.x == average.x)
      movement_logic_new_point(ml, average.x, average.y);

#if 0		
    if(slowdown_counter++ == 10)
      {
	char buf[256];
	if(average.x > 510)
	  {
	    target_pan += 10;
	    snprintf(buf, 256, "MSET visca:camera:pan %i visca:camera:tilt %i", target_pan, target_tilt);
	    redisCommand(c, buf);
	    redisCommand(c, "lpush visca:command m");
	    redisCommand(c, "expire visca:command 5");
	  }
	else if(average.x < 90)
	  {
	    target_pan -= 10;
	    snprintf(buf, 256, "MSET visca:camera:pan %i visca:camera:tilt %i", target_pan, target_tilt);
	    redisCommand(c, buf);
	    redisCommand(c, "lpush visca:command m");
	    redisCommand(c, "expire visca:command 5");                      
	  }

	slowdown_counter = 0;

      }
#endif
    /*
      snprintf(buf, 256, "MSET vision:x %i vision:y %i, vision:boole %i", (int)average.x, (int)average.y, boole);
      redisCommand(c, buf);
      // execute the redis Command to move the camera
      */

    cv::circle(output,average,10,cv::Scalar(255,0,0),-1);

    handleTrackedPoints(frame, output);

    std::swap(points[1], points[0]);
    cv::swap(gray_prev, gray);
  }

  // feature point detection
  void detectFeaturePoints() 
  {
			
    // detect the features
    cv::goodFeaturesToTrack(gray, 
			    features,   //the output detected features
			    max_count,  //the maximum number of features 
			    qlevel,     
			    minDist);   //min distance between two features
  }

  // determine if new points should be added
  bool addNewPoints() 
  {
    return points[0].size()<=10;
  }

  // determine which tracked point should be accepted
  bool acceptTrackedPoint(int i) 
  {

    return status[i] &&
      // if point has moved
      (abs(points[0][i].x-points[1][i].x)+
       (abs(points[0][i].y-points[1][i].y))>2);
  }

  void handleTrackedPoints(cv:: Mat &frame, cv:: Mat &output) 
  {

    // for all tracked points
    for(int i= 0; i < points[1].size(); i++ ) 
      {

	//draw line and circle
	cv::line(output, initial[i], points[1][i], cv::Scalar(255,255,255));
	cv::circle(output, points[1][i], 3, cv::Scalar(255,255,255),-1);
      }
  }
};



		
#ifdef BLACKMAGIC_CAPTURE

void callback(uint8_t * bytes, uint32_t size, uint64_t timestamp, void * priv)
{
  uyvy_to_bgr(rgb_tmp_buf, bytes, 720, 486);
  cvSetData(blackmagicImgIn, rgb_tmp_buf, 720*3);
  capture_blackmagic_unblock(&cbm);
}

#endif

int FeatureTracker::slowdown_counter = 0;
int FeatureTracker::target_pan = 0, FeatureTracker::target_tilt = 0;

//IplImage* iplImage = cvLoadImage("file path");
//cv::Mat image4(iplImage,false);
int main(int argc, char ** argv)
{


  c = redisConnect("127.0.0.1", 6379);
  if (c->err) 
    {
      printf("Error Connecting to Redis Server: %s\n", c->errstr);
      redisFree(c);
      return 1;
    }


  picture_counter = 0;

  //init video proc
  VideoProcessor processor;

  //init feature tracker
  FeatureTracker tracker;
	
  //open video file
  //processor.setInput(0);


#ifdef BLACKMAGIC_CAPTURE
  if(argc == 2) 
    cbm.video_source = BLACKMAGIC_COMPOSITE;
  else
    cbm.video_source = BLACKMAGIC_SVIDEO;

  //initialize movement logic
  ml = movement_logic_init(720, 486, 2, 25);
  //movement_logic_init_sample_fsm(ml);
  movement_logic_init_fsm(ml, "vision:movement:fsm");
  blackmagicImgIn = cvCreateImageHeader(cvSize(720, 486), IPL_DEPTH_8U, 3);
  cbm.priv = blackmagicImgIn;
  rgb_tmp_buf = (uint8_t *) malloc(720*486*3); //resolution * 3bbp
  cbm.video_frame_callback = callback;
  capture_blackmagic_init(&cbm);
  capture_blackmagic_start(&cbm);

#else
  //initialize movement logic
  ml = movement_logic_init(640, 480, 2, 25);
  //movement_logic_init_sample_fsm(ml);
  movement_logic_init_fsm(ml, "vision:movement:fsm");
  cv::VideoCapture capture(0);
  if (!capture.isOpened())
    return 1;
  processor.setCaptureDevice(capture);	
#endif


  /*
    CvCapture* capture = 0;

    capture = cvCaptureFromCAM(0);

    IplImage* framea = 0;

  

    framea = cvQueryFrame(capture);
  */

	

  //set frame processor
  processor.setFrameProcessor(&tracker);

  //declare a window to display the video
  //processor.displayOutput("Tracked Features");
  //processor.displayInput("");
	

  //start process
  processor.run();
}

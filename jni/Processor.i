/*
 * include the headers required by the generated cpp code
 */
%{
#include "Processor.h"
#include "image_pool.h"
using namespace cv;
%}


/**
 * some constants, see Processor.h
 */
#define DETECT_FAST 0
#define DETECT_STAR 1
#define DETECT_SURF 2
#define DETECT_MSER 3

//import the android-cv.i file so that swig is aware of all that has been previous defined
//notice that it is not an include....
%import "android-cv.i"

//make sure to import the image_pool as it is 
//referenced by the Processor java generated
//class
%typemap(javaimports) Processor "
import com.opencv.jni.image_pool;// import the image_pool interface for playing nice with
								 // android-opencv

/** Processor - for processing images that are stored in an image pool
*/"

class Processor {
public:
	Processor();
	virtual ~Processor();

	
	void MeasureAndDisplay(int input_idx, image_pool* pool, int ReferenceLocationX, int ReferenceLocationY, int ObjectToMeasureX, int ObjectToMeasureY);
	
	//void detectAndDrawFeatures(int idx, image_pool* pool, int feature_type);

	//bool detectAndDrawChessboard(int idx,image_pool* pool);
	
	//void resetChess();
	
	//int getNumberDetectedChessboards();
	
	//void calibrate(const char* filename);
	
	//void SetPointsLocation(int x1, int y1, int x2, int y2);
	//void drawText(int idx, image_pool* pool, const char* text);

};

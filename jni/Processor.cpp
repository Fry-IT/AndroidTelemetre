/*
 * Processor.cpp
 *
 *  Created on: Jun 13, 2010
 *      Author: ethan
 */

#include "Processor.h"

#include <sys/stat.h>
#include <iostream>
#include <math.h>
#include <stdio.h>

using namespace cv;
using namespace std;

Processor::Processor() :
      stard(20/*max_size*/, 8/*response_threshold*/, 15/*line_threshold_projected*/, 8/*line_threshold_binarized*/, 5/*suppress_nonmax_size*/),
      fastd(50/*threshold*/, true/*nonmax_suppression*/),
      surfd(1600/*hessian_threshold*/, 3/*octaves*/, 1/*octave_layers*/),
      mserd(10/*delta*/, 10/*min_area*/, 1000/*max_area*/, 0.2/*max_variation*/, 0.7/*min_diversity*/, 2/*max_evolution*/, 1/*area_threshold*/, 5/*min_margin*/, 2/*edge_blur_size*/)
      // fastd(20/*threshold*/, true/*nonmax_suppression*/),
      // surfd(100./*hessian_threshold*/, 1/*octaves*/, 2/*octave_layers*/)

{

}

Processor::~Processor()
{
  // TODO Auto-generated destructor stub
}

void drawRows(Mat img, Point PointA, Point PointB, Point PointC)
    {
		int TriangleSpace = 10;

		Point PointA1Lefttriangle = PointA;
		Point PointA2Lefttriangle = Point(PointA1Lefttriangle.x + TriangleSpace, PointA1Lefttriangle.y - TriangleSpace);
		Point PointA3Lefttriangle = Point(PointA1Lefttriangle.x + TriangleSpace, PointA1Lefttriangle.y + TriangleSpace);

		Point PointA1Uptriangle = Point(PointA.x - 10, PointA.y);
		Point PointA2Uptriangle = Point(PointA1Uptriangle.x - TriangleSpace, PointA1Uptriangle.y + TriangleSpace);
		Point PointA3Uptriangle = Point(PointA1Uptriangle.x + TriangleSpace, PointA1Uptriangle.y + TriangleSpace);

		Point PointB1triangle = PointB;
		Point PointB2triangle = Point(PointB1triangle.x - TriangleSpace, PointB1triangle.y - TriangleSpace);
		Point PointB3triangle = Point(PointB1triangle.x - TriangleSpace, PointB1triangle.y + TriangleSpace);

		Point PointC1triangle = Point(PointC.x - 10, PointC.y);
		Point PointC2triangle = Point(PointC1triangle.x - TriangleSpace, PointC1triangle.y - TriangleSpace);
		Point PointC3triangle = Point(PointC1triangle.x + TriangleSpace, PointC1triangle.y - TriangleSpace);

		const Point ArrayPointsLeft [] = {PointA1Lefttriangle, PointA2Lefttriangle, PointA3Lefttriangle};
		const Point ArrayPointsUp [] = {PointA1Uptriangle, PointA2Uptriangle, PointA3Uptriangle};
		const Point ArrayPointsRight [] = {PointB1triangle, PointB2triangle, PointB3triangle};
		const Point ArrayPointsDown [] = {PointC1triangle, PointC2triangle, PointC3triangle};

		const int npts = 3;

		//Draw measure lines of object to measure
		line(img, PointA, PointB, cvScalar(0 , 0, 255, 255), 2,8, 0);
		line(img, Point(PointA.x - 10, PointA.y) , Point(PointC.x - 10, PointC.y), cvScalar(0, 0, 255, 255), 2,8, 0);

		fillConvexPoly(img, ArrayPointsLeft, npts, cvScalar(0, 0, 255, 255), 8, 0); //Draw Triangle "Top Left" LEFT
		fillConvexPoly(img, ArrayPointsUp, npts, cvScalar(0 , 0, 255, 255), 8, 0); //Draw Triangle "Top Left" UP
		fillConvexPoly(img, ArrayPointsRight, npts, cvScalar(0 , 0, 255, 255), 8, 0); //Draw Triangle "Top Right" Right
		fillConvexPoly(img, ArrayPointsDown, npts, cvScalar(0 , 0, 255, 255), 8, 0); //Draw Triangle "Bottom" DOWN

    }

void Processor::MeasureAndDisplay(int input_idx, image_pool* pool, int ReferenceLocationX, int ReferenceLocationY, int ObjectToMeasureX, int ObjectToMeasureY)
{
	  Mat img = pool->getImage(input_idx);

//	  if (img.empty())
//	    return; //no image at input_idx!

	  //////FLOOD AND FILL

	  	Point ReferenceLocation = Point(ReferenceLocationX, ReferenceLocationY);
	  	Point ObjectToMeasure = Point(ObjectToMeasureX, ObjectToMeasureY);
		Rect rect1; //reference rectangle generated by floodandfill method
		Rect rect2; //object to measure rectangle generated by floodandfill method
		int Postit_Width = 76; // mm
		int Postit_Height = 76; // mm
		int Box_Width; // mm
		int Box_Height; // mm
		int Postit_Captured_Width; //px
		int Postit_Captured_Height; //px
		int Box_Captured_Width; //px
		int Box_Captured_Height; //px
		string Text_Box_Width;
		string Text_Box_Height;
		Point Text_Location_Box_Width =  Point(100,100); // TODO CHANGE IT
		Point Text_Location_Box_Height =  Point(100,100); // TODO CHANGE IT
		Point middleRect2Width;
		Point middleRect2Height;
		Point PointA; // Point top left of rectangle of object to measure
		Point PointB; // Point top right of rectangle of object to measure
		Point PointC; // Point bottom left of rectangle of object to measure
		char buffer1 [33];
		char buffer2 [33];
		char buffer3 [33];

		//Flood and fill reference object and object to measure
		floodFill(img, ReferenceLocation, cvScalar(255, 0, 0, 0), &rect1, CV_RGB(5,5,5), CV_RGB(3,3,3),8);
		floodFill(img, ObjectToMeasure, cvScalar(255, 0, 0, 0), &rect2, CV_RGB(5,5,5), CV_RGB(3,3,3),8);

		//Draw a point of locations touch of reference and object to measure
		circle(img,ReferenceLocation, 5, CV_RGB(255,0,0), -1); //Point of fist object
		circle(img,ObjectToMeasure, 5, CV_RGB(255,0,0), -1); //Point of second object

		//Draw rectangle of reference and object to measure
		rectangle(img, rect1, cvScalar(255, 0, 0, 100),1); //TODO Fill Rectangle with opacity 50% Red
		//rectangle(img, rect2, cvScalar(255, 0, 0, 100),1);

		//Capture Measure of reference and object to measure
		Postit_Captured_Width = rect1.width;
		Postit_Captured_Height = rect1.height;
		Box_Captured_Width = rect2.width;
		Box_Captured_Height = rect2.height;

		//Define measure of object to measure
		Box_Width = ((Box_Captured_Width * Postit_Width)/Postit_Captured_Width);
		Box_Height = ((Box_Captured_Height * Postit_Height)/Postit_Captured_Height);

		//Define position of A, B and C
		PointA = Point(rect2.x, rect2.y);
		PointB = Point((rect2.x + rect2.width), rect2.y);
		PointC = Point(rect2.x, (rect2.y + rect2.height));

		drawRows(img, PointA, PointB, PointC); //Draw measure lines of object to measure

		//Define middle of width and height of the object to measure
		middleRect2Width =  Point(rect2.x +(rect2.width/2), rect2.y);
		middleRect2Height =  Point(rect2.x, rect2.y +(rect2.height/2));

		//Write measure of object to measure
		sprintf(buffer1,"%d mm",Box_Width); //TODO OPTIMISE IT
		sprintf(buffer2,"%d",Box_Height); //TODO OPTIMISE IT
		putText(img, buffer1, Point((middleRect2Width.x - 50)  , (middleRect2Width.y - 20)), 1, 2, cvScalar(0, 0, 255, 100), 2, 4, false);
		putText(img, buffer2, Point((middleRect2Height.x - 70) , middleRect2Height.y), 1, 2, cvScalar(0, 0, 255, 100), 2, 4, false);
		putText(img, "mm", Point((middleRect2Height.x - 70) , (middleRect2Height.y + 30)), 1, 2, cvScalar(0, 0, 255, 100), 2, 4, false);
}

void saveCameraParams(const string& filename, Size imageSize, Size boardSize, float squareSize, float aspectRatio,
                      int flags, const Mat& cameraMatrix, const Mat& distCoeffs, const vector<Mat>& rvecs,
                      const vector<Mat>& tvecs, const vector<float>& reprojErrs,
                      const vector<vector<Point2f> >& imagePoints, double totalAvgErr)
{
  FileStorage fs(filename, FileStorage::WRITE);

  time_t t;
  time(&t);
  struct tm *t2 = localtime(&t);
  char buf[1024];
  strftime(buf, sizeof(buf) - 1, "%c", t2);

  fs << "calibration_time" << buf;

  if (!rvecs.empty() || !reprojErrs.empty())
    fs << "nframes" << (int)std::max(rvecs.size(), reprojErrs.size());
  fs << "image_width" << imageSize.width;
  fs << "image_height" << imageSize.height;
  fs << "board_width" << boardSize.width;
  fs << "board_height" << boardSize.height;
  fs << "squareSize" << squareSize;

  if (flags & CV_CALIB_FIX_ASPECT_RATIO)
    fs << "aspectRatio" << aspectRatio;

  if (flags != 0)
  {
    sprintf(buf, "flags: %s%s%s%s", flags & CV_CALIB_USE_INTRINSIC_GUESS ? "+use_intrinsic_guess" : "", flags
        & CV_CALIB_FIX_ASPECT_RATIO ? "+fix_aspectRatio" : "", flags & CV_CALIB_FIX_PRINCIPAL_POINT
        ? "+fix_principal_point" : "", flags & CV_CALIB_ZERO_TANGENT_DIST ? "+zero_tangent_dist" : "");
    cvWriteComment(*fs, buf, 0);
  }

  fs << "flags" << flags;

  fs << "camera_matrix" << cameraMatrix;
  fs << "distortion_coefficients" << distCoeffs;

  fs << "avg_reprojection_error" << totalAvgErr;
  if (!reprojErrs.empty())
    fs << "per_view_reprojection_errors" << Mat(reprojErrs);

  if (!rvecs.empty() && !tvecs.empty())
  {
    Mat bigmat(rvecs.size(), 6, CV_32F);
    for (size_t i = 0; i < rvecs.size(); i++)
    {
      Mat r = bigmat(Range(i, i + 1), Range(0, 3));
      Mat t = bigmat(Range(i, i + 1), Range(3, 6));
      rvecs[i].copyTo(r);
      tvecs[i].copyTo(t);
    }
    cvWriteComment(*fs, "a set of 6-tuples (rotation vector + translation vector) for each view", 0);
    fs << "extrinsic_parameters" << bigmat;
  }

  if (!imagePoints.empty())
  {
    Mat imagePtMat(imagePoints.size(), imagePoints[0].size(), CV_32FC2);
    for (size_t i = 0; i < imagePoints.size(); i++)
    {
      Mat r = imagePtMat.row(i).reshape(2, imagePtMat.cols);
      Mat(imagePoints[i]).copyTo(r);
    }
    fs << "image_points" << imagePtMat;
  }
}

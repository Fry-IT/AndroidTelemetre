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

void Processor::MeasureAndDisplay(int input_idx, image_pool* pool, int ReferenceLocationX, int ReferenceLocationY, int ObjectToMeasureX, int ObjectToMeasureY)
{
	  Mat img = pool->getImage(input_idx);

//	  if (img.empty())
//	    return; //no image at input_idx!

	  //////FLOOD AND FILL

	  Point ReferenceLocation = Point(ReferenceLocationX, ReferenceLocationY);
	  	Point ObjectToMeasure = Point(ObjectToMeasureX, ObjectToMeasureY);
		Rect rect1;
		Rect rect2;
		int Postit_Width = 76; //76 mm
		int Postit_Height = 76; //76 mm
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
		char buffer1 [33];
		char buffer2 [33];


		floodFill(img, ReferenceLocation, CV_RGB(255,255,255), &rect1, CV_RGB(5,5,5), CV_RGB(3,3,3)); //flood and fill object in center
		floodFill(img, ObjectToMeasure, CV_RGB(255,255,255), &rect2, CV_RGB(5,5,5), CV_RGB(3,3,3)); //flood and fill second object

		circle(img,ReferenceLocation, 5, CV_RGB(255,0,0), -1); //Point of fist object
		circle(img,ObjectToMeasure, 5, CV_RGB(255,0,0), -1); //Point of second object

		rectangle(img, rect1, CV_RGB(255,0,0),1); //Rectangle Object 1
		rectangle(img, rect2, CV_RGB(255,0,0),1); //Rectangle Object 2

		Postit_Captured_Width = rect1.width;
		Postit_Captured_Height = rect1.height;
		Box_Captured_Width = rect2.width;
		Box_Captured_Height = rect2.height;

		Box_Width = ((Box_Captured_Width * Postit_Width)/Postit_Captured_Width);
		Box_Height = ((Box_Captured_Height * Postit_Height)/Postit_Captured_Height);

		sprintf(buffer1,"%d",Box_Width); //TODO OPTIMISE IT
		sprintf(buffer2,"%d",Box_Height); //TODO OPTIMISE IT

		middleRect2Width =  Point(rect2.x +(rect2.width/2), rect2.y);
		middleRect2Height =  Point(rect2.x, rect2.y +(rect2.height/2));

		putText(img, buffer1, middleRect2Width, 1, 2, cvScalar(255, 0, 0, 255), 3, 8, false);
		putText(img, buffer2, middleRect2Height, 1, 2, cvScalar(255, 0, 0, 255), 3, 8, false);


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

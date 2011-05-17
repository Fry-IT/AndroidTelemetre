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

void Processor::SetPointsLocation(int x1, int y1, int x2, int y2){
	LocationFirstPoint = Point(x1,y1);
	LocationSecondPoint = Point(x2,y2);
}

void Processor::detectAndDrawFeatures(int input_idx, image_pool* pool, int feature_type)
{
  FeatureDetector* fd = 0;

  switch (feature_type)
  {
    case DETECT_SURF:
      fd = &surfd;
      break;
    case DETECT_FAST:
      fd = &fastd;
      break;
    case DETECT_STAR:
      fd = &stard;
      break;
    case DETECT_MSER:
    	fd = &mserd;
    	break;
  }

  Mat greyimage = pool->getGrey(input_idx);

  Mat img = pool->getImage(input_idx);

  if (img.empty() || greyimage.empty() || fd == 0)
    return; //no image at input_idx!


  keypoints.clear();

  //if(grayimage->step1() > sizeof(uchar)) return;
  //cvtColor(*img,*grayimage,CV_RGB2GRAY);

  //fd->detect(greyimage, keypoints);

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //TEST

  /////////RECTANGLE
  /*std::vector<cv::KeyPoint> newkeypointsorganised;

    for (vector<KeyPoint>::const_iterator it = keypoints.begin(); it != keypoints.end(); ++it)
      {
  	  Point pt1 = it->pt;

  	  for(vector<KeyPoint>::const_iterator it2 = newkeypointsorganised.begin(); it2 != newkeypointsorganised.end(); ++it2)
  	    {l
  		  Point pt2 = it2->pt;

  		  if(pt2.y >= pt1.y)
  		  {
  			  //Insert After
  		  }
  		  else
  		  {
  			  //Insert Before
  		  }
  	    }
      }*/


  //////COLOR SELECTION
  /*char threshold= 60;
  char BG_threshold = 175;
  char BG_diff = 60;
  	IplImage* img_result_threshold, *img_morph, *img_temp;
  	int x,y;
  	char red,green,blue;

  	//CvMemStorage* storage = cvCreateMemStorage(0);
          CvSeq* contour = 0;
          vector<vector<Point> > contours;
          vector<Vec4i> hierarchy;
          int contourIdx = -1;

  	//Create image result for threshold
  	img_result_threshold= cvCreateImage(cvSize(img.cols, img.rows), IPL_DEPTH_8U, 1);
  	img_morph= cvCreateImage(cvSize(img.cols, img.rows), IPL_DEPTH_8U, 1);
  	img_temp= cvCreateImage(cvSize(img.cols, img.rows), IPL_DEPTH_8U, 1);

  	Mat Mat_img_result_threshold= Mat(img_result_threshold, false);
  	Mat Mat_img_morph = Mat(img_morph, false);
  	Mat Mat_img_temp= Mat(img_temp, false);

  	for(x=0; x<img.cols; x++)
  	{
  		for(y=0;y<img.rows; y++)
  		{
  			Vec3b intensity = img.at<Vec3b>(x, y);
  			blue = intensity.val[0];
  			green = intensity.val[1];
  			red = intensity.val[2];

  			//Vec3b black;
  			//Vec3b white;

  			if((red>threshold)&&(green < BG_threshold) && (blue < BG_threshold) && (abs(green-blue)< BG_diff)){
  				Mat_img_result_threshold.at<uchar>(x, y) = 255;
  			}else{
  				Mat_img_result_threshold.at<uchar>(x, y) = 0;
  			}
  		}
  	}

  	//img = Mat_img_result_threshold;
  	morphologyEx(img_result_threshold, Mat_img_morph, 0,img_temp, Point(-1, -1), 1, BORDER_CONSTANT, morphologyDefaultBorderValue());
  	findContours(Mat_img_morph, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, Point());
  	drawContours(img, contours, contourIdx, cvScalar(0, 255, 0), 1, 8, vector<Vec4i>(),INT_MAX, Point());*/
  	///////

  	//////FLOOD AND FILL

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

  	char buffer [33];


  	floodFill(img, LocationFirstPoint, CV_RGB(255,255,255), &rect1, CV_RGB(5,5,5), CV_RGB(3,3,3)); //flood and fill object in center
  	floodFill(img, LocationSecondPoint, CV_RGB(255,255,255), &rect2, CV_RGB(5,5,5), CV_RGB(3,3,3)); //flood and fill second object

  	circle(img,LocationFirstPoint, 5, CV_RGB(255,0,0), -1); //Point of fist object
  	circle(img,LocationSecondPoint, 5, CV_RGB(255,0,0), -1); //Point of second object

  	rectangle(img, rect1, CV_RGB(255,0,0),1); //Rectangle Object 1
  	rectangle(img, rect2, CV_RGB(255,0,0),1); //Rectangle Object 2

  	Postit_Captured_Width = rect1.width;
  	Postit_Captured_Height = rect1.height;
  	Box_Captured_Width = rect2.width;
  	Box_Captured_Height = rect2.height;

  	Box_Width = ((Box_Captured_Width * Postit_Width)/Postit_Captured_Width);
  	Box_Height = ((Box_Captured_Height * Postit_Height)/Postit_Captured_Height);

  	sprintf(buffer,"%d,%d",Box_Width, Box_Height);

  	putText(img, buffer, Text_Location_Box_Width, 1, 4, cvScalar(0, 255, 0, 255), 3, 8, false);

  //
  //
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  	Point2f pvpt = keypoints.begin()->pt; //Stock Previous point to draw lines
  /*for (vector<KeyPoint>::const_iterator it = keypoints.begin(); it != keypoints.end(); ++it)
  {
    // circle(img, it->pt, 3, cvScalar(255, 0, 255, 0));
    
    Point2f pt = it->pt;
    pt.x -= 2;
    pt.y -= 2;

    Point2f newpt = it->pt;
    newpt.x += 2;
    newpt.y += 2;

    if (feature_type == DETECT_SURF)
    {
    	circle(img, pt, it->size*0.5, cvScalar(0, 0, 0, 0), 2);
    }
    else if (feature_type == DETECT_FAST)
    {
    	circle(img, pt, it->size, cvScalar(0, 0, 0, 0), 2);
    }
    else if (feature_type == DETECT_MSER)
    {
    	//circle(img, pt, it->size, cvScalar(0, 0, 0, 0), 2);
    	if(newpt != keypoints.begin()->pt)
		{
			Point2f centerline = it->pt;
			float distance = 0;

			centerline.x = (newpt.x + pvpt.x) / 2;
			centerline.y = (newpt.y + pvpt.y) / 2;

			distance = sqrt((newpt.x-pvpt.x)+(newpt.y-pvpt.y));
			string str;
			str += distance;

			line(img,pvpt,newpt,cvScalar(0, 0, 0, 0),3,8,0);
			putText(img, str, centerline, 1, 4, cvScalar(0, 255, 0, 255), 3, 8, false);
		}
    	}
    else
   	{
   		//circle(img, pt, 5, cvScalar(0, 0, 0, 0), 2);
    	line(img,pvpt,newpt,cvScalar(0, 0, 0, 0),3,8,0);
    }
    
    pt.x += 2;
    pt.y += 2;
    if (feature_type == DETECT_SURF)
    {
    	circle(img, pt, it->size*0.5, cvScalar(255, 255, 0, 0), 2);
    }
    else if (feature_type == DETECT_FAST)
    {
	    circle(img, pt, it->size, cvScalar(255, 255, 0, 0), 2);

	}
	else if (feature_type == DETECT_MSER)
    {
		//circle(img, pt, it->size, cvScalar(0, 0, 0, 0), 2);
		if(newpt != keypoints.begin()->pt)
		{
			Point2f centerline = it->pt;
			int distance = 0;

			centerline.x = (newpt.x + pvpt.x) / 2;
			centerline.y = (newpt.y + pvpt.y) / 2;

			distance = sqrt((newpt.x-pvpt.x)+(newpt.y-pvpt.y));
			string str;
			str += distance;

			line(img,pvpt,newpt,cvScalar(0, 0, 0, 0),3,8,0);
			putText(img, str, centerline, 1, 4, cvScalar(0, 255, 0, 255), 3, 8, false);
		}
	}
    else
    {
	    circle(img, pt, 5, cvScalar(255, 255, 0, 0), 2);
	    //line(img,pvpt,newpt,cvScalar(0, 0, 0, 0),3,8,0);
	}

    pvpt.x = newpt.x;
	pvpt.y= newpt.y;
  }*/

  //line(img,keypoints.end()->pt,keypoints.begin()->pt,cvScalar(0, 0, 0, 0),3,8,0);
  //pool->addImage(output_idx,outimage);

}
static double computeReprojectionErrors(const vector<vector<Point3f> >& objectPoints,
                                        const vector<vector<Point2f> >& imagePoints, const vector<Mat>& rvecs,
                                        const vector<Mat>& tvecs, const Mat& cameraMatrix, const Mat& distCoeffs,
                                        vector<float>& perViewErrors)
{
  vector<Point2f> imagePoints2;
  int i, totalPoints = 0;
  double totalErr = 0, err;
  perViewErrors.resize(objectPoints.size());

  for (i = 0; i < (int)objectPoints.size(); i++)
  {
    projectPoints(Mat(objectPoints[i]), rvecs[i], tvecs[i], cameraMatrix, distCoeffs, imagePoints2);
    err = norm(Mat(imagePoints[i]), Mat(imagePoints2), CV_L1);
    int n = (int)objectPoints[i].size();
    perViewErrors[i] = err / n;
    totalErr += err;
    totalPoints += n;
  }

  return totalErr / totalPoints;
}

static void calcChessboardCorners(Size boardSize, float squareSize, vector<Point3f>& corners)
{
  corners.resize(0);

  for (int i = 0; i < boardSize.height; i++)
    for (int j = 0; j < boardSize.width; j++)
      corners.push_back(Point3f(float(j * squareSize), float(i * squareSize), 0));
}

/**from opencv/samples/cpp/calibration.cpp
 *
 */
static bool runCalibration(vector<vector<Point2f> > imagePoints, Size imageSize, Size boardSize, float squareSize,
                           float aspectRatio, int flags, Mat& cameraMatrix, Mat& distCoeffs, vector<Mat>& rvecs,
                           vector<Mat>& tvecs, vector<float>& reprojErrs, double& totalAvgErr)
{
  cameraMatrix = Mat::eye(3, 3, CV_64F);
  if (flags & CV_CALIB_FIX_ASPECT_RATIO)
    cameraMatrix.at<double> (0, 0) = aspectRatio;

  distCoeffs = Mat::zeros(5, 1, CV_64F);

  vector<vector<Point3f> > objectPoints(1);
  calcChessboardCorners(boardSize, squareSize, objectPoints[0]);
  for (size_t i = 1; i < imagePoints.size(); i++)
    objectPoints.push_back(objectPoints[0]);

  calibrateCamera(objectPoints, imagePoints, imageSize, cameraMatrix, distCoeffs, rvecs, tvecs, flags);

  bool ok = checkRange(cameraMatrix, CV_CHECK_QUIET) && checkRange(distCoeffs, CV_CHECK_QUIET);

  totalAvgErr
      = computeReprojectionErrors(objectPoints, imagePoints, rvecs, tvecs, cameraMatrix, distCoeffs, reprojErrs);

  return ok;
}

bool Processor::detectAndDrawChessboard(int idx, image_pool* pool)
{
  Mat grey = pool->getGrey(idx);
  if (grey.empty())
    return false;
  vector<Point2f> corners;

  IplImage iplgrey = grey;
  if (!cvCheckChessboard(&iplgrey, Size(6, 8)))
    return false;
  bool patternfound = findChessboardCorners(grey, Size(6, 8), corners);

  Mat img = pool->getImage(idx);

  if (corners.size() < 1)
    return false;

  cornerSubPix(grey, corners, Size(11, 11), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));

  if (patternfound)
    imagepoints.push_back(corners);

  drawChessboardCorners(img, Size(6, 8), Mat(corners), patternfound);

  imgsize = grey.size();

  return patternfound;

}

void Processor::drawText(int i, image_pool* pool, const char* ctext)
{
  // Use "y" to show that the baseLine is about
  string text = ctext;
  int fontFace = FONT_HERSHEY_COMPLEX_SMALL;
  double fontScale = .8;
  int thickness = .5;

  Mat img = pool->getImage(i);

  int baseline = 0;
  Size textSize = getTextSize(text, fontFace, fontScale, thickness, &baseline);
  baseline += thickness;

  // center the text
  Point textOrg((img.cols - textSize.width) / 2, (img.rows - textSize.height * 2));

  // draw the box
  rectangle(img, textOrg + Point(0, baseline), textOrg + Point(textSize.width, -textSize.height), Scalar(0, 0, 255),
            CV_FILLED);
  // ... and the baseline first
  line(img, textOrg + Point(0, thickness), textOrg + Point(textSize.width, thickness), Scalar(0, 0, 255));

  // then put the text itself
  putText(img, text, textOrg, fontFace, fontScale, Scalar::all(255), thickness, 8);
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
void Processor::resetChess()
{

  imagepoints.clear();
}

void Processor::calibrate(const char* filename)
{

  vector<Mat> rvecs, tvecs;
  vector<float> reprojErrs;
  double totalAvgErr = 0;
  int flags = 0;
  bool writeExtrinsics = true;
  bool writePoints = true;

  bool ok = runCalibration(imagepoints, imgsize, Size(6, 8), 1.f, 1.f, flags, K, distortion, rvecs, tvecs, reprojErrs,
                           totalAvgErr);

  if (ok)
  {

    saveCameraParams(filename, imgsize, Size(6, 8), 1.f, 1.f, flags, K, distortion, writeExtrinsics ? rvecs : vector<
        Mat> (), writeExtrinsics ? tvecs : vector<Mat> (), writeExtrinsics ? reprojErrs : vector<float> (), writePoints
        ? imagepoints : vector<vector<Point2f> > (), totalAvgErr);
  }

}

int Processor::getNumberDetectedChessboards()
{
  return imagepoints.size();
}

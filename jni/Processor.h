/*
 * Processor.h
 *
 *  Created on: Jun 13, 2010
 *      Author: ethan
 */

#ifndef PROCESSOR_H_
#define PROCESSOR_H_

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include <vector>

#include "image_pool.h"

#define DETECT_FAST 0
#define DETECT_STAR 1
#define DETECT_SURF 2
#define DETECT_MSER 3

class Processor
{
public:

  Processor();
  virtual ~Processor();

  void MeasureAndDisplay(int input_idx, image_pool* pool, int ReferenceLocationX, int ReferenceLocationY, int ObjectToMeasureX, int ObjectToMeasureY);

private:

  cv::StarFeatureDetector stard; // CLEAN IT
  cv::FastFeatureDetector fastd; // CLEAN IT
  cv::SurfFeatureDetector surfd; // CLEAN IT
  cv::MserFeatureDetector mserd; // CLEAN IT

};

#endif /* PROCESSOR_H_ */

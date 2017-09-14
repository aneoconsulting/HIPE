/*
* LabelOCR.h
*
*  Created on: May 1, 2014
*      Author: chd
*/

#ifndef LABELOCR_H_
#define LABELOCR_H_


#include <iostream>
#include <math.h>
#include <string.h>
#include <sstream>


#include <vector>
#include <opencv2/core/mat.hpp>
#include "opencv2/text.hpp" 
#include "opencv2/core/utility.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"


using namespace cv;
using namespace std;
using namespace cv::text;

class LabelOCR {

public:
	LabelOCR();
	virtual ~LabelOCR();
	vector<string> runRecognition(const vector<cv::Mat> &labelImage, int labelType);
	Ptr<OCRTesseract> tesseOCR_;
	bool showImages;
	void preProcess(const cv::Mat &InputImage, cv::Mat &binImage);

private:
	
	string runPrediction1(const cv::Mat &labelImage, int i);
	string runPrediction2(const cv::Mat &labelImage, int i);
	void skeletonize(cv::Mat& im);
	void thinningIteration(cv::Mat& im, int iter);
	void filterUndesiredChars(string &str);

};

#endif /* LABELOCR_H_ */
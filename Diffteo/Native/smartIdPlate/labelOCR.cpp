/*
* LabelOCR.cpp
*
*  Created on: May 1, 2014
*      Author: chd
*/

#include "LabelOCR.h"
#include <iso646.h>
#include "diffteo.h"
#include "tools.h"
#include "ImageProcessing.h"


LabelOCR::LabelOCR()
{
	// constructor
	// Pass it to Tesseract API
	tesseOCR_ = OCRTesseract::create(NULL, "eng", "ABCDEFHIJKLMNOPQRSTUVWXYZ0123456789-Code", 3, 7);
	//Init(NULL, "eng", tesseract::OEM_DEFAULT);
	//tess.SetVariable("tessedit_char_whitelist", "ABCDEFHIJKLMNOPQRSTUVWXYZ0123456789-"); //-
	//tess.SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);

	showImages = false;
}

LabelOCR::~LabelOCR()
{
	//destructor
}

void LabelOCR::filterUndesiredChars(string& str)
{
	char chars[] = "?";

	for (unsigned int i = 0; i < strlen(chars); ++i)
	{
		// you need include <algorithm> to use general algorithms like std::remove()
		str.erase(std::remove(str.begin(), str.end(), chars[i]), str.end());
	}
}

cv::Mat binarize(const cv::Mat&source)
{
	cv::Mat results;
	/*int blockDim = MIN(source.size().height, source.size().width);
	if (blockDim % 2 != 1) blockDim++;*/
	cv::threshold(source, results, 230, 255, cv::THRESH_BINARY | THRESH_OTSU);
	return results;
}

cv::Mat equalize(const cv::Mat&source)
{
	cv::Mat results;
	BrightnessAndContrastAuto(source, results);
	return results;
}

void LabelOCR::preProcess(const Mat& InputImage, Mat& binImage)
{
	Mat midImage, midImage2, dst;
	int ksize = 11;
	Mat Morph = getStructuringElement(MORPH_CROSS, Size(1, 1));
	Mat HPKernel = (Mat_<float>(5, 5) << -1.0 , -1.0 , -1.0 , -1.0 , -1.0 ,
		-1.0 , -1.0 , -1.0 , -1.0 , -1.0 ,
		-1.0 , -1.0 , 25.0 , -1.0 , -1.0 ,
		-1.0 , -1.0 , -1.0 , -1.0 , -1.0 ,
		-1.0 , -1.0 , -1.0 , -1.0 , -1.0);

	quantizeImage(InputImage, midImage, 2);
	ShowImageNoWait(midImage);

	//bilateralFilter(midImage, midImage2, ksize, ksize * 2, ksize / 2);
	cvtColor(midImage, dst, CV_RGB2GRAY);

	//bilateralFilter(midImage, dst, 3, ksize * 2, ksize / 2);
	//dst = equalize(dst);

	

	binImage = binarize(dst);

	//filter2D(dst, midImage2, InputImage.depth(), HPKernel);
	//cvtColor(midImage2, binImage, COLOR_RGB2GRAY);*/

	//bilateralFilter(InputImage, dst, 15, 15 * 2, 15 / 2);
	//filter2D(dst, midImage2, InputImage.depth(), HPKernel);



	//cvtColor(dst, binImage, CV_BGR2GRAY);

	////create a CLAHE object(Arguments are optional).
	//Ptr<CLAHE> clahe = createCLAHE(2.0);
	//clahe->apply(binImage, binImage);
	////ShowImageWait(small);
	//GaussianBlur(binImage, binImage, Size(3, 3), 0);
	//
	//Mat bw;
	//threshold(binImage, binImage, 0.0, 255.0, THRESH_BINARY | THRESH_OTSU);
	//bitwise_not(binImage, binImage);
	//
	
	//binImage = InputImage;

	ShowImageNoWait(binImage);
	//threshold(midImage, binImage, 60, 255, CV_THRESH_BINARY);
	//threshold(binImage, binImage ,0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	//erode(binImage, binImage, 3, Point(-1, -1), 2, 1, 1);
	//morphologyEx( binImage,binImage,MORPH_CLOSE, Morph);
}

string LabelOCR::runPrediction1(const Mat& labelImage, int i)
{
	string t1;
	if (labelImage.empty())
		return (t1);

	Mat textImage;
	Mat drawImage = labelImage.clone();

	double labelROI_x = labelImage.cols * 0.10; // initial point x
	double labelROI_y = labelImage.rows * 0.76; // initial point y
	double labelROI_w = labelImage.cols * 0.6; // width
	double labelROI_h = labelImage.rows * 0.20; // heigth

	Rect labelROI(labelROI_x, labelROI_y, labelROI_w, labelROI_h);

	Mat midImage;
	preProcess(drawImage, textImage);

	//tesseOCR_->setWhiteList("W");
	t1 = tesseOCR_->run(textImage, 1, OCR_LEVEL_TEXTLINE);
	// Get the text

	/*t1 = string(text1);
	if (t1.size() > 2)
		t1.resize(t1.size() - 2);*/

	cout << "label_" << i << ": " << t1 << endl;

	if (showImages)
	{
		putText(drawImage, t1, Point(labelROI.x + 7, labelROI.y - 5), FONT_HERSHEY_PLAIN, 1.5, Scalar(0, 0, 255), 2, 8); // CV_FONT_HERSHEY_SIMPLEX
		rectangle(drawImage, labelROI, Scalar(0, 0, 255), 2, 8, 0);
		//
		stringstream ss;
		ss << i;
		string str = ss.str();

		//imshow("label_"+str, labelImage);
		imshow("textImage_1_" + str, textImage);
		imshow("letters_1_" + str, drawImage);
	}

	return (t1);
}

string LabelOCR::runPrediction2(const Mat& labelImage, int i)
{
	string t1;
	if (labelImage.empty())
		return (t1);

	Mat textImage;
	Mat drawImage = labelImage.clone();

	double labelROI_x = 0; // initial point x
	double labelROI_y = 0; // initial point y
	double labelROI_w = labelImage.cols; // width
	double labelROI_h = labelImage.rows; // heigth

	Rect labelROI(labelROI_x, labelROI_y, labelROI_w, labelROI_h);

	Mat midImage;
	preProcess(drawImage, textImage);
	drawImage = Mat::zeros(textImage.size(), CV_8UC3);
	
	//textImage = drawImage;

	t1 = tesseOCR_->run(textImage, 30);
	//tess.TesseractRect(textImage.data, 1, textImage.step1(), labelROI.x, labelROI.y, labelROI.width, labelROI.height);
	// Get the text
	//char* text1 = tess.GetUTF8Text();
	//t1 = string(text1);
	ShowImageWait(textImage);


	cout << "label_" << i << ": " << t1 << endl;

	if (showImages)
	{
		putText(drawImage, t1, Point(labelROI.x + 7, labelROI.y + 5), FONT_HERSHEY_PLAIN, 1.5, Scalar(0, 0, 255), 2, 8); // CV_FONT_HERSHEY_SIMPLEX
		rectangle(drawImage, labelROI, Scalar(0, 0, 255), 2, 8, 0);
		//
		stringstream ss;
		ss << i;
		string str = ss.str();

		//imshow("label_"+str, labelImage);
		imshow("textImage_2_" + str, textImage);
		imshow("letters_2_" + str, drawImage);
	}

	return (t1);
}

vector<string> LabelOCR::runRecognition(const vector<Mat>& labelImage, int labelType)
{
	vector<string> output;

	output.resize(labelImage.size());

	for (size_t i = 0; i < labelImage.size(); i++)
	{
		if (!labelImage[i].empty() && labelType == 1)
			output[i] = runPrediction1(labelImage[i], i);
		if (!labelImage[i].empty() && labelType == 2)
			output[i] = runPrediction2(labelImage[i], i);
	}
	return (output);
}

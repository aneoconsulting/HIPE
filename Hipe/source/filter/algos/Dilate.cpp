#include "algos/Dilate.h"

int filter::algos::Dilate::convertMorphType(const std::string & name)
{
	if (name == "BLACKHAT")
	{
		return cv::MorphTypes::MORPH_BLACKHAT;
	}
	else if (name == "CLOSE")
	{
		return cv::MorphTypes::MORPH_CLOSE;
	}
	else if (name == "DILATE")
	{
		return cv::MorphTypes::MORPH_DILATE;
	}
	else if (name == "ERODE")
	{
		return cv::MorphTypes::MORPH_ERODE;
	}
	else if (name == "GRADIENT")
	{
		return cv::MorphTypes::MORPH_GRADIENT;
	}
	else if (name == "HITMISS")
	{
		return cv::MorphTypes::MORPH_HITMISS;
	}
	else if (name == "OPEN")
	{
		return cv::MorphTypes::MORPH_OPEN;
	}
	else if (name == "TOPHAT")
	{
		return cv::MorphTypes::MORPH_TOPHAT;
	}
	else
	{
		throw HipeException("Error in Dilate::convertMorphType - Unknown morph type argument: " + name + ".\nMorphType name must not conatain the \"MORPH_\" prefix (i.e. ERODE for MORPH_ERODE).");
	}
}

int filter::algos::Dilate::convertMorphShape(const std::string & name)
{
	if (name == "CROSS")
	{
		return cv::MorphShapes::MORPH_CROSS;
	}
	else if (name == "ELLIPSE")
	{
		return cv::MorphShapes::MORPH_ELLIPSE;
	}
	else if (name == "RECT")
	{
		return cv::MorphShapes::MORPH_RECT;
	}
	else
	{
		throw HipeException("Error in Dilate::convertMorphShape - Unknown morph shape argument: " + name + ".\nMorphShape name must not conatain the \"MORPH_\" prefix (i.e. RECT for MORPH_RECT).");
	}
}

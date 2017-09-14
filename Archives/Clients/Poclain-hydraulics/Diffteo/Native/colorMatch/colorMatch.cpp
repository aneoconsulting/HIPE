// This project aims at recognizing the average pixel color 
// in a patch provided as an argument

// It first looks at the percentage of red, blue and green
// in the average pixel color, and then find the closest
// color from the list defined at the beginning of this file

#include "misc.h"

/*
Scalar _white = Scalar(255, 255, 255);
Scalar _red = Scalar(255, 0, 0);
Scalar _green = Scalar(0, 255, 0);
Scalar _blue = Scalar(0, 0, 255);
Scalar _yellow = Scalar(255, 255, 0);
Scalar _cyan = Scalar(0, 255, 255);
Scalar _magenta = Scalar(255, 0, 255);
Scalar _grey = Scalar(128, 128, 128);
*/

// BGR format %
Scalar _red = Scalar(0, 0, 100);
Scalar _green = Scalar(0, 100, 0);
Scalar _blue = Scalar(100, 0, 0);
Scalar _yellow = Scalar(0, 50, 50);
Scalar _cyan = Scalar(50, 50, 0);
Scalar _magenta = Scalar(50, 0, 50);
Scalar _gray = Scalar(33, 33,33);

vector<Scalar> colorList = { _red, _blue, _green, _yellow, _cyan, _magenta, _gray };
vector<string> colorstrings = {"red", "blue", "green", "yellow", "cyan", "magenta", "gray" };

double eucl_dist(Scalar query, Scalar ref)
{
	double eucl_dist = 0;
	for (int i = 0; i < 3; i++)
		eucl_dist += (query[i] - ref[i])*(query[i] - ref[i]);
	return eucl_dist;
}

string getClosestColor(Scalar query)
{
	double norm = (query[0] + query[1] + query[2])/100;
	Scalar queryNormed = Scalar(query[0]/norm, query[1]/norm, query[2]/norm);

	Scalar closest_color = colorList[0];
	double min_delta = eucl_dist(queryNormed, closest_color);

	int colorIndex = 0, i = 0;
	for (auto color : colorList)
	{
		double delta = eucl_dist(queryNormed, color);
		if (delta < min_delta)
		{
			min_delta = delta;
			closest_color = color;
			colorIndex = i;
		}
		i++;
	}

	double total = 1;
	cout << "Closest color BGR% \t " << (int)(closest_color[0] / total) << ", " << (int)(closest_color[1] / total)
		<< ", " << (int)(closest_color[2] / total) << endl;

	if (closest_color != _gray)
		return colorstrings[colorIndex];
	else //Find shade of gray / black / white
	{
		int avgChannel = (query[0] + query[1] + query[2])/3;
		if (avgChannel > 200)
			return "white";
		if (avgChannel > 150)
			return "light gray";
		if (avgChannel > 90)
			return "gray";
		if (avgChannel> 30)
			return "dark gray";
		else
			return "black";
	}
}

int main(int argc, char* argv[])
{
	// Check number of arguments
	if (argc != 2)
	{
		cout << "ERROR : program expects exactly 1 argument as input (an image)" << endl;
		return -1;
	}

	// Load image
	Mat roi = getImg(argv[1]);
	// Compute average BGR color of ROI
	Scalar mean = cv::mean(roi);
	cout << "Average color BGR \t" << (int)mean[0] << ", " << (int)mean[1] << ", " << (int)mean[2] << endl;
	double total = (mean[0] + mean[1] + mean[2])/100;
	cout << "Average color BGR% \t " << (int)(mean[0]/total) << ", " 
			<< (int)(mean[1]/total) << ", " << (int)(mean[2]/total) << endl;

	// Visualize patch of average color and original patch
	Mat meanColor = roi.clone();
	meanColor = Scalar(mean[0], mean[1], mean[2]);
	ShowImage(roi);
	ShowImage(meanColor);

	// Find closest color in the color list provided
	string color = getClosestColor(mean);
	cout << "Closest color is " << color << endl;

	waitKey(0);
	return 0;
}
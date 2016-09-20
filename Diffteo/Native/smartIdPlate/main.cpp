#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>


#include <vector>
#include "drawRect.h"
#include "tools.h"
#include "perspective.h"
#include "diffteo.h"
#include "DetectTextArea.h"
#include "labelOCR.h"

using namespace std;
using namespace cv;

void clearUserData(UserData& user_data)
{
	user_data.areaInfos.clear();
	user_data.img = Mat();
	user_data.clicked = false;
}

int main(int argc, char **argv)
{
	const String dir = argv[1];
	std::vector<String> filenames;
	cv::glob(dir, filenames);
	UserData userData;
	LabelOCR lblOCR;

	for (size_t i = 0; i<filenames.size(); i++)
	{
		clearUserData(userData);

		Mat srcRaw = getImage(filenames[i], 1);
		Mat planSrcRaw;

		//TODO this an equivalent of the function to find a rectangle in a perspective
		//Need to implement a proper function to do it automaticaly
		//objectAreaDrawer(srcRaw, userData);

		//Disable get plate directly cleaned
		//findperspective(srcRaw, planSrcRaw, userData);
		planSrcRaw = srcRaw;
		ShowImageNoWait(planSrcRaw);
		FileInfo fInfo = getFileName(filenames[i]);
		//imwrite(fInfo.addPrefix("plan_"), planSrcRaw);


		//vector<Mat> textAreaImgs = detectTextArea(planSrcRaw);
		vector<Mat> textAreaImgs;

		textAreaImgs.push_back(planSrcRaw);

		/*for (int i = 0; i < textAreaImgs.size(); i++)
		{
			String nb = "label_" + i;
			
			while (1) {
				ShowImageNoWait(textAreaImgs[i]);
				char ret = waitKey(300);

				if ((ret > 'a' && ret < 'z') || (ret > '0' && ret < '9') || (ret > 'A' && ret < 'Z') || ret == '-')
				{
					std::string path = fInfo.addPrefix("/letters/" + nb + "_" + ret + "_");
					imwrite(path, textAreaImgs[i]);
					break;
				}
				else if (ret > 0)
				{
					break;
				}
			}
			
		}*/
		lblOCR.runRecognition(textAreaImgs, 2);

	
	}


	//testFFT(argc, argv);
	//function(argc, argv);
	return 0;
}

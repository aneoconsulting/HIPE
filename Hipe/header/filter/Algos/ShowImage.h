#pragma once
#include <filter/tools/RegisterTools.hpp>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>
#include <opencv2/highgui/highgui.hpp>

namespace filter
{
	namespace algos
	{
		class ShowImage : public IFilter
		{

			//data::ConnexData<data::ImageArrayData, data::ImageArrayData> _connexData;
			CONNECTOR(data::ImageArrayData, data::ImageArrayData);

			REGISTER(ShowImage, ()), _connexData(data::INOUT)
			{
				wait = false;
				wait_ms = 0;
			}

			~ShowImage()
			{
				cv::destroyWindow(_name);
			}

			REGISTER_P(int, waitkey);
			REGISTER_P(bool, wait);
			REGISTER_P(int, wait_ms);

			virtual std::string resultAsString() { return std::string("TODO"); };

		public:
			HipeStatus process()
			{
				cv::namedWindow(_name);

				while (!_connexData.empty()) // While i've parent data
				{
					data::ImageArrayData images = _connexData.pop();


					//Resize all images coming from the same parent
					for (auto &myImage : images.Array())
					{

						::cv::imshow(_name, myImage);
						char c;

						if (wait_ms <= 0 && wait)
						{
							std::cout << "Waiting for key..." << std::endl;
							cv::waitKey(0);
						}
						else if (wait_ms > 0)
						{
							cv::waitKey(wait_ms);
						}
					}
				}


				return OK;
			}

			void dispose()
			{
				Model::dispose();
				cv::destroyWindow(_name);
			}

		};

		ADD_CLASS(ShowImage, waitkey, wait, wait_ms);
	}
}

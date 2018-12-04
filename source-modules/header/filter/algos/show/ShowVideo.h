//@HIPE_LICENSE@
#pragma once
#include <corefilter/tools/RegisterTools.hpp>
#include <corefilter/tools/RegisterTable.h>

#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>
#include <data/ImageArrayData.h>
#include <opencv2/highgui.hpp>


namespace filter
{
	namespace algos
	{
		/**
		 * \var ShowVideo::waitkey
		 * The time a frame should be shown. A null value will let the frame be shown permanently and wait for user input.
		 *
		 * \var ShowVideo::fullscreen
		 * Should the video be played in exclusive fullscreen mode or in a dedicated window?
		 */

		/**
		 *\todo
		 * \brief The ShowVideo filter is used to show a video in a dedicated window.
		 */
		class FILTER_EXPORT ShowVideo : public IFilter
		{
			//data::ConnexData<data::ImageArrayData, data::ImageArrayData> _connexData;
			CONNECTOR(data::ImageArrayData, data::ImageArrayData);

			REGISTER(ShowVideo, ()), _connexData(data::INOUT)
			{
				waitkey = 10;
				fullscreen = false;
				hasWindow = false;
			}

			REGISTER_P(int, waitkey);
			REGISTER_P(bool, fullscreen);

			std::atomic<bool> hasWindow;		//<! [TODO] Boolean used to keep track of the video's dedicated window


			~ShowVideo()
			{
			}

			virtual std::string resultAsString()
			{
				return std::string("TODO");
			};

		public:
			HipeStatus process() override;

			void dispose()
			{
				Model::dispose();

				if (hasWindow.exchange(false))
				{
					cv::destroyWindow(_name);
				}
			}
		};

		ADD_CLASS(ShowVideo, waitkey, fullscreen);
	}
}

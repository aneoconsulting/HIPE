#pragma once
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>
#include <data/ImageData.h>
#include <data/DirPatternData.h>
#include <data/MatcherData.h>
#include <atomic>


namespace filter
{
	namespace algos
	{
		/**
		 * \var DirLatch::inlier_threshold
		 * The inliers2_const distance threshold used to match them between images. [TODO]
		 *
		 * \var DirLatch::nn_match_ratio
		 * The nearest neighbor matching ratio. [TODO]
		 *
		 * \var DirLatch::hessianThreshold
		 * The hessian threshold used to find keypoints with the SURF detector. Only features whose hessian is larger than hessianThreshold are retained by the detector. \see cv::xfeatures2d::SURF
		 *
		 * \var DirLatch::skip_frame
		 * The number of frames to skip between each detection.
		 *
		 * \var DirLatch::wait
		 * Should we wait and show the output image?
		 */

		 /**
		   *\todo
		   * \brief The DirLatch filter will match similarities into two images.
		   *
		   * It is used to find an object from an image in another one using keypoints by using the DirLatch algorithm.
		   * It awaits a PatternData object in input and will output an image containing the contoured computed matching simimarities.
		   * The filter will work as a separate thread to be able to find objects in videos.
		   * For an alternative: \see Akaze
		  */
		class FILTER_EXPORT DirLatch : public filter::IFilter
		{
			std::atomic<bool> isStart;		//<! [TODO] Is the thread running?
			boost::thread *thr_server;		//<! [TODO] Pointer to the DirLatch matching task.
			core::queue::ConcurrentQueue<data::DirPatternData> imagesStack;	//<! [TODO] The queue containing the frames to process.

			core::queue::ConcurrentQueue<data::ImageData> result;	//<! [TODO] The queue containing the results of all the detections to process
			data::MatcherData tosend;									//<! The current frame's matching result to output
			int good_matches;
			int count_frame;										//<! The number of frames already processed 

			CONNECTOR(data::DirPatternData, data::ImageData);

			REGISTER(DirLatch, ()), _connexData(data::INDATA)
			{
				wait_time = 5000;
				hessianThreshold = 100;
				skip_frame = -1;
				isStart = false;
				wait = false;
				thr_server = nullptr;
				good_matches = 30;
			}

			REGISTER_P(float, inlier_threshold);
			REGISTER_P(float, nn_match_ratio);

			REGISTER_P(int, hessianThreshold);

			REGISTER_P(int, skip_frame);

			REGISTER_P(bool, wait);
			REGISTER_P(int, wait_time);

			virtual std::string resultAsString() { return std::string("TODO"); };

		public:

			/**
			 * \brief  Detects patterns in an image. Runs as a separate thread.
			 * Fetch its images from the imagesStack queue then feed the detectObjet method.
			 */
			void startDetectObject();

			HipeStatus process();

			virtual void dispose()
			{
				isStart = false;

				if (thr_server != nullptr) {
					thr_server->join();
					delete thr_server;
					thr_server = nullptr;
				}
			}
		};

		ADD_CLASS(DirLatch, inlier_threshold, nn_match_ratio, hessianThreshold, wait, wait_time);
	}
}

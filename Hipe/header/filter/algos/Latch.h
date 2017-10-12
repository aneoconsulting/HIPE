#pragma once
#include <filter/tools/RegisterClass.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>
#include <data/ImageData.h>
#include <data/PatternData.h>
#include <opencv2/xfeatures2d.hpp>
#include <atomic>


namespace filter
{
	namespace algos
	{
		/**
		 * \var Latch::inlier_threshold
		 * The inliers distance threshold used to match them between images. [TODO]
		 *
		 * \var Latch::nn_match_ratio
		 * The nearest neighbor matching ratio. [TODO]
		 *
		 * \var Latch::hessianThreshold
		 * The hessian threshold used to find keypoints with the SURF detector. Only features whose hessian is larger than hessianThreshold are retained by the detector. \see cv::xfeatures2d::SURF
		 *
		 * \var Latch::skip_frame
		 * The number of frames to skip between each detection.
		 *
		 * \var Latch::wait
		 * Should we wait and show the output image?
		 */

		/**
		  *\todo
		  * \brief The Latch filter will match similarities into two images.
		  *
		  * It is used to find an object from an image in another one using keypoints by using the LATCH algorithm.
		  * It awaits a PatternData object in input and will output an image containing the contoured computed matching simimarities.
		  * The filter will work as a separate thread to be able to find objects in videos.
		  * For an alternative: \see Akaze
		 */
		class FILTER_EXPORT Latch : public filter::IFilter
		{
			std::atomic<bool> isStart;		//<! [TODO] Is the thread running?
			boost::thread *thr_server;		//<! [TODO] Pointer to the latch matching task.
			core::queue::ConcurrentQueue<data::PatternData> imagesStack;	//<! [TODO] The queue containing the frames to process.

			/**
			 * \brief The MatchContainer class is a data structure used to regroup all the detection information needed to match two images.
			 */
			class MatchContainer
			{
			public:
				cv::Mat patternImage;					//<! [TODO] The pattern image to find on the request image.
				std::vector<cv::KeyPoint> inliers1;		//<! The inliers (the pertinent information) computed from the patternImage.

				cv::Mat requestImage;					//<! [TODO] The request image on which we would like to find the patternImage.
				std::vector<cv::KeyPoint> inliers2;		//<! The inliers (the pertinent information) computed from the request image.

				std::vector<cv::DMatch> goodMatches;	//<! The inliers1 and inliers2 that match (i.e. they were found on the patternImage and the requestImage).


			};
			core::queue::ConcurrentQueue<MatchContainer> result;	//<! [TODO] The queue containing the results of all the detections to process
			MatchContainer tosend;									//<! The current frame's matching result to output

			int count_frame;										//<! The number of frames already processed 

			CONNECTOR(data::PatternData, data::ImageData);

			REGISTER(Latch, ()), _connexData(data::INDATA)
			{
				hessianThreshold = 100;
				skip_frame = -1;
				isStart = false;
				wait = false;
			}

			REGISTER_P(float, inlier_threshold);
			REGISTER_P(float, nn_match_ratio);

			REGISTER_P(int, hessianThreshold);

			REGISTER_P(int, skip_frame);

			REGISTER_P(bool, wait);

			virtual std::string resultAsString() { return std::string("TODO"); };

		private:
			/**
			 * \brief Detects patterns in an image.
			 * \param patternData The PatternData object containing the regions of interest and the request image. \see PatternData
			 * \return Returns the result of the detection as a MatchContainer object. \see MatchContainer
			 */
			MatchContainer detectObject(data::PatternData & patternData);

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

		ADD_CLASS(Latch, inlier_threshold, nn_match_ratio, hessianThreshold, wait);
	}
}

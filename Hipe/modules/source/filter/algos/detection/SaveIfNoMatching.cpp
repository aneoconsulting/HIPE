#include <filter/algos/detection/SaveIfNoMatching.h>
#include <filter/algos/detection/Latch.h>

#pragma warning(push, 0)
#include <opencv2/xfeatures2d/nonfree.hpp>
#include "algos/ResultFilter.h"
#include "algos/preprocessing/Homography.h"
#include "algos/utils/CVUtils.h"
#pragma warning(pop)

namespace filter
{
	namespace algos
	{
		HipeStatus SaveIfNoMatching::savePaternPicture(const cv::Mat& picture)
		{
			createDirectory(store_directory);
			std::stringstream build_path;
			build_path << store_directory << PathSeparator() << "match_" << id << ".jpg";

			cv::imwrite(build_path.str(), picture);
			id++;
			return OK;
		}

	

		HipeStatus SaveIfNoMatching::process()
		{
			typedef boost::chrono::duration<double> sec;
			data::MatcherData best_match = _connexData.pop();

			if (armed)
			{
				boost::timer::cpu_times elapsed = timer.elapsed();
				sec seconds = boost::chrono::nanoseconds(timer.elapsed().user);
				if (seconds.count() < resend_best_during_ms / 1000.0)
				{
					saved_matchingImage = saved_matchingImage.clone();
					cv::Mat best = CVUtils::writeTextOnImage(saved_matchingImage, "BEST MATCH FOUND !!!");

					PUSH_DATA(data::ImageData(best));
					return OK;
				}
				else
				{
					armed = false;
					timer.stop();
					std::cout << "Time elapse to show the best one" << seconds.count() << std::endl;
				}

			}

			if (best_match.requestImage().empty())
			{
				cv::Mat toPush = CVUtils::writeTextOnImage(saved_matchingImage, "PREVIOUS SEARCH");
				PUSH_DATA(data::ImageData(toPush));

				return OK;
			}

			//There is no more matching and the DirLatch or Latch return an empty answer
			// Then store the requestimage as new pattern
			if (best_match.patternImage().empty())
			{
				savePaternPicture(best_match.requestImage_const());
				cv::Mat save = cv::Mat::zeros(best_match.requestImage().size().height, 2 * best_match.requestImage().size().width, CV_8UC3);
				cv::Rect ROI = cv::Rect(0, 0, best_match.requestImage().size().width, best_match.requestImage().size().height);
				best_match.requestImage().copyTo(save(ROI));

				saved_matchingImage = save;

				cv::Mat result = CVUtils::writeTextOnImage(best_match.requestImage(), "No MATCHING...");

				PUSH_DATA(data::ImageData(result));
				return OK;
			}

			Homography homography;
			homography.setName(this->getName() + "__homography");
			homography.setLevel(1);
			data::ConnexDataBase& connectorHomography = homography.getCast().getConnector();
			data::DataPort& portHomography = static_cast<data::DataPort &>(connectorHomography.getPort());
			portHomography.push(best_match);

			HipeStatus hipe_status = homography.process();
			if (hipe_status != HipeStatus::OK)
			{
				cv::Mat save = CVUtils::writeTextOnImage(saved_matchingImage, "WAIT FOR RESEARCH...");

				PUSH_DATA(data::ImageData(save));
				return hipe_status;
			}

			/*ResultFilter resultForHomography;
			resultForHomography.setName(this->getName() + "__resultForHomography");
			resultForHomography.setLevel(1);
			resultForHomography.addDependenciesName(homography.getName());
			resultForHomography.addDependencies(&homography);

			data::DataPort & resultPort = static_cast<data::DataPort &>(resultForHomography.getCast().getConnector().getPort());
			data::Data pop = resultPort.pop();
			data::ShapeData & shape = static_cast<data::ShapeData & >(pop);
			std::vector<cv::Point2f> rectangle = shape.QuadrilatereArray()[0];*/

			//TODO Check if this form is a rectangle or square with 98° angle

			cv::Mat matchingImage;
			cv::drawMatches(best_match.requestImage(), best_match.inliers1(), best_match.patternImage(), best_match.inliers2(), best_match.goodMatches(), matchingImage);

			if (best_match.IsBest())
			{
				saved_matchingImage = matchingImage.clone();
				matchingImage = CVUtils::writeTextOnImage(matchingImage, "BEST MATCH FOUND !!!");
				timer.start();
				armed = true;
			}
			else
			{
				saved_matchingImage = matchingImage.clone();
				matchingImage = CVUtils::writeTextOnImage(matchingImage, "SEARCHING THE BEST MATCH...");
			}

			PUSH_DATA(data::ImageData(matchingImage));
			return OK;
		}
	}
}

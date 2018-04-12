#include <filter/algos/detection/DirLatch.h>
#include <filter/algos/detection/Latch.h>

#pragma warning(push, 0)
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <opencv2/xfeatures2d.hpp>
#include "algos/ResultFilter.h"
#include "algos/preprocessing/Homography.h"
#pragma warning(pop)

namespace filter
{
	namespace algos
	{
		void DirLatch::startDetectObject()
		{
			DirLatch* This = this;
			thr_server = new boost::thread([This]
			{
				while (This->isStart)
				{
					data::DirPatternData pattern;
					if (!This->imagesStack.trypop_until(pattern, 30))
						continue;

					


					if (This->result.size() != 0)
						This->result.clear();

					This->result.push(data::ImageData());


				}
			});
		}

		HipeStatus DirLatch::process()
		{
			

			//TODO Manage multi latch from here depends on the number of core and the number of Task to ask
			filter::algos::Latch latch;
			latch.set_hessianThreshold(hessianThreshold);
			latch.set_inlier_threshold(inlier_threshold);
			latch.set_nn_match_ratio(nn_match_ratio);
			latch.set_skip_frame(skip_frame);
			latch.set_wait(wait);
			latch.set_wait_time(wait_time);
			latch.setName(this->getCast().getName() + "__subLatch");
			latch.setLevel(1);

			data::ConnexDataBase & connector = latch.getCast().getConnector();
			data::DataPort & port = static_cast<data::DataPort &>(connector.getPort());

			data::DirPatternData dirPattern = _connexData.pop();
			data::DirectoryImgData directory_img = dirPattern.DirectoryImg(); //Bug fix Return the reference from the method
			data::ImageData source = dirPattern.imageSource(); //Bug fix Return the reference from the method
			
			directory_img.refreshDirectory();
			data::ImageData cropImage = directory_img.nextImageFile();

			ResultFilter resultFilter;
			resultFilter.setName(this->getName() + "__resultFilter");
			resultFilter.setLevel(1);
			resultFilter.addDependenciesName(latch.getName());
			resultFilter.addDependencies(&latch);
			
			int l_good_matches = good_matches;
			data::MatcherData best_match;
			data::SquareCrop best_squarecrop;

			while (!cropImage.empty())
			{
				cropImage = directory_img.nextImageFile();
				std::vector<int> crop = { 0,0, cropImage.getMat().size().width, cropImage.getMat().size().height };
				data::SquareCrop squarecrop;
				squarecrop << cropImage;
				squarecrop << crop;
				data::PatternData pattern(source, squarecrop);

				port.push(pattern);
				HipeStatus hipe_status = latch.process();
				if (hipe_status != HipeStatus::OK)
					return hipe_status;
				


				data::DataPort & resultPort = static_cast<data::DataPort &>(resultFilter.getCast().getConnector().getPort());
				data::Data pop = resultPort.pop();
				data::MatcherData & matcher = static_cast<data::MatcherData& >(pop);

				if(good_matches <= (int)matcher.goodMatches().size())
				{
					best_match = matcher;
					best_squarecrop = squarecrop;
				}

			}

			if (best_match.goodMatches_const().size() < good_matches)
			{
				PUSH_DATA(data::ImageData());
				return OK;
			}

			Homography homography;
			homography.setName(this->getName() + "__homography");
			homography.setLevel(1);
			data::ConnexDataBase & connectorHomography = homography.getCast().getConnector();
			data::DataPort & portHomography = static_cast<data::DataPort &>(connectorHomography.getPort());
			portHomography.push(best_match);
			
			HipeStatus hipe_status = homography.process();
			if (hipe_status != HipeStatus::OK)
			{
				PUSH_DATA(data::ImageData());
				return hipe_status;
			}
			ResultFilter resultForHomography;
			resultForHomography.setName(this->getName() + "__resultForHomography");
			resultForHomography.setLevel(1);
			resultForHomography.addDependenciesName(latch.getName());
			resultForHomography.addDependencies(&homography);
			data::DataPort & resultPort = static_cast<data::DataPort &>(resultFilter.getCast().getConnector().getPort());
			data::Data pop = resultPort.pop();
			data::ShapeData & shape = static_cast<data::ShapeData & >(pop);
			std::vector<cv::Point2f> rectangle = shape.QuadrilatereArray()[0];

			//TODO Check if this form is a rectangle or square with 98° angle

			cv::Mat matchingImage;
			cv::drawMatches(source.getMat(), best_match.inliers1(), best_squarecrop.getPicture().getMat(), best_match.inliers2(), best_match.goodMatches(), matchingImage);

			PUSH_DATA(data::ImageData(matchingImage));
			return OK;
			//if (!isStart.exchange(true))
			//{
			//	startDetectObject();
			//}

			//data::PatternData patternData = _connexData.pop();
			//if (patternData.crops().getSquareCrop().empty())
			//{
			//	data::MatcherData output;
			//	PUSH_DATA(output);
			//	return DATA_EMPTY;
			//}

			//if (skip_frame <= 0 || count_frame % skip_frame == 0)
			//{
			//	if (imagesStack.size() != 0)
			//		imagesStack.clear();
			//	imagesStack.push(patternData);
			//}
			//count_frame++;
		
			//data::MatcherData md_result;
			//cv::Mat res;

			//if (patternData.imageRequest().getMat().empty())
			//{
			//	data::MatcherData output;
			//	PUSH_DATA(output);
			//}
			//else if (wait == true && result.trypop_until(md_result, wait_time)) // wait 5 sec it's like infinite but allow to kill thread
			//{
			//	tosend = md_result;
			//	PUSH_DATA(tosend);
			//}
			//else if (result.trypop_until(md_result, 30)) // wait 30ms no more
			//{
			//	tosend = md_result;
			//	PUSH_DATA(tosend);
			//}
			//
			//else if (tosend.requestImage_const().empty())
			//{
			//	data::MatcherData output;
			//	PUSH_DATA(output);
			//}
			//else
			//{
			//	md_result = tosend; //Use backup because the algorithme is too late

			//	PUSH_DATA(tosend);
			//}

			return OK;
		}
	}
}

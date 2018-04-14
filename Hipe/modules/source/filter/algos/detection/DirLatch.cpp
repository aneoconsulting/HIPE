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
		inline void cleanFilter(IFilter * filter)
		{
			filter->cleanUp();

			data::ConnexDataBase& outRes = filter->getConnector();
			data::DataPort & data_port = static_cast<data::DataPort &>(outRes.getPort());

			while (!data_port.empty())
			{
				data_port.pop();
			}
		}

		inline void cleanAndDisposeFilter(IFilter * filter)
		{
			filter->cleanUp();
			filter->dispose();

			data::ConnexDataBase& outRes = filter->getConnector();
			data::DataPort & data_port = static_cast<data::DataPort &>(outRes.getPort());

			while (!data_port.empty())
			{
				data_port.pop();
			}
		}

		HipeStatus DirLatch::detectObject(const data::DirPatternData& dirPattern)
		{
			data::ConnexDataBase & connector = latch->getCast().getConnector();
			data::DataPort & port = static_cast<data::DataPort &>(connector.getPort());


			data::DirectoryImgData directory_img = dirPattern.DirectoryImg(); //Bug fix Return the reference from the method
			data::ImageData source = dirPattern.imageSource(); //Bug fix Return the reference from the method

			directory_img.refreshDirectory();
			data::ImageData cropImage = directory_img.nextImageFile();
			if (cropImage.empty())
			{
				cleanFilter(latch.get());

				data::MatcherData matcher;
				matcher.setPatternImage(source.getMat());

				this->result.push(matcher);

				return OK;
			}

			int l_good_matches = good_matches;
			data::MatcherData best_match;


			while (!cropImage.empty())
			{

				std::vector<int> crop = { 0,0, cropImage.getMat().size().width, cropImage.getMat().size().height };
				data::SquareCrop squarecrop;
				squarecrop << cropImage;
				squarecrop << crop;
				data::PatternData pattern(source, squarecrop);

				port.push(pattern);
				HipeStatus hipe_status = latch->process();
				if (hipe_status != HipeStatus::OK)
				{
					cleanFilter(latch.get());
					return hipe_status;
				}



				data::DataPort & resultPort = static_cast<data::DataPort &>(resultLatch->getCast().getConnector().getPort());
				data::Data pop = resultPort.pop();
				data::MatcherData & matcher = static_cast<data::MatcherData& >(pop);

				if (l_good_matches <= (int)matcher.goodMatches().size())
				{
					best_match = matcher;
					l_good_matches = (int)matcher.goodMatches().size();
					
				}
				this->result.push(matcher);
				cropImage = directory_img.nextImageFile();

			}
			if (best_match.goodMatches_const().size() >= good_matches)
			{
				best_match.setBest();
				this->result.clear();
				this->result.push(best_match);
				return OK;
			}

			cleanFilter(latch.get());
			cleanFilter(resultLatch.get());

			data::MatcherData empty_match;
			empty_match.setRequestImage(source.getMat());
			this->result.clear();
			this->result.push(empty_match);

			return OK;
		}

		void DirLatch::startDetectObject()
		{
			DirLatch* This = this;
			thr_server = new boost::thread([This]
			{
				while (This->isStart)
				{
					data::DirPatternData pattern;
					if (!This->imagesStack.trypop_until(pattern, 10))
						continue;

					data::MatcherData res;
					HipeStatus status = This->detectObject(pattern);
					if (status != HipeStatus::OK)
					{
						throw HipeException("Issue with Latch in DirLatch filter");
					}
				}
			});
		}

		HipeStatus DirLatch::process()
		{
			
			data::DirectoryImgData dirData;
			data::ImageData source;
			
			//Part of code is to avoid bug in Web application remove it and receive a dirPatternData later
			while (!_connexData.empty())
			{
				data::Data data1 = _connexData.pop();
				if (data::DataTypeMapper::isSequenceDirectory(data1.getType()))
				{
					dirData = static_cast<data::DirectoryImgData &>(data1);
				}
				else if (data::DataTypeMapper::isImage(data1.getType()))
				{
					source = static_cast<data::ImageData>(data1);
				}
				else
				{
					
					//Ignore other type of input data ATM
				}
			}
			dirData.refreshDirectory();
			if (source.empty())
			{
				PUSH_DATA(data::MatcherData());
				return OK;
			}

			if (!dirData.hasFiles())
			{
				data::MatcherData matcherResult;
				matcherResult.setRequestImage(source.getMat());
				PUSH_DATA(matcherResult);

				return OK;
			}

			
			
			if (!this->isStart.exchange(true))
			{
				latch = std::make_shared<filter::algos::Latch>();
				resultLatch = std::make_shared<ResultFilter>();
				latch->set_hessianThreshold(hessianThreshold);
				latch->set_inlier_threshold(inlier_threshold);
				latch->set_nn_match_ratio(nn_match_ratio);
				latch->set_skip_frame(skip_frame);
				bool value = true;
				latch->set_wait(value);
				latch->set_wait_time(wait_time);
				latch->setName(this->getCast().getName() + "__subLatch");
				latch->setLevel(1);

				resultLatch->setName(this->getName() + "__resultFilter");
				resultLatch->setLevel(1);
				resultLatch->addDependenciesName(latch->getName());
				resultLatch->addDependencies(latch.get());

				startDetectObject();
			}
	
			data::DirPatternData dirPattern(source, dirData);
		
			if (skip_frame <= 0 || count_frame % skip_frame == 0)
			{
				if (imagesStack.size() != 0)
					imagesStack.clear();
				imagesStack.push(dirPattern);
				count_frame = 0;
			}
			count_frame++;
		
			data::MatcherData md_result;

			if (result.trypop_until(md_result, 10)) // wait 10ms no more
			{
				tosend = md_result;
				
			}
			
			PUSH_DATA(tosend);
		

			return OK;
		}

		void DirLatch::dispose()
		{
			isStart = false;

			if (thr_server != nullptr)
			{
				thr_server->join();
				delete thr_server;
				thr_server = nullptr;
			}

			if (latch)
			{
				cleanAndDisposeFilter(latch.get());
				latch.reset();
			}
			if (resultLatch)
			{
				cleanAndDisposeFilter(resultLatch.get());
				resultLatch.reset();

			}
		}
	}
}

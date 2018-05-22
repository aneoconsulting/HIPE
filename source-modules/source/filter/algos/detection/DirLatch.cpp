//READ LICENSE BEFORE ANY USAGE
/* Copyright (C) 2018  Damien DUBUC ddubuc@aneo.fr (ANEO S.A.S)
 *  Team Contact : hipe@aneo.fr
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  In addition, we kindly ask you to acknowledge ANEO and its authors in any program 
 *  or publication in which you use HIPE. You are not required to do so; it is up to your 
 *  common sense to decide whether you want to comply with this request or not.
 *  
 *  Non-free versions of HIPE are available under terms different from those of the General 
 *  Public License. e.g. they do not require you to accompany any object code using HIPE 
 *  with the corresponding source code. Following the new licensing any change request from 
 *  contributors to ANEO must accept terms of re-license by a general announcement. 
 *  For these alternative terms you must request a license from ANEO S.A.S Company 
 *  Licensing Office. Users and or developers interested in such a license should 
 *  contact us (hipe@aneo.fr) for more information.
 */

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
				matcher.setRequestImage(source.getMat());

				this->result.push(matcher);
				if (matcher.patternImage().empty())
				{
					std::cout << "Pattern Image is empty in latch thread of DirLatch" << std::endl;
				}
				return OK;
			}

			int l_good_matches = 6;
			data::MatcherData best_match;

			int nbFile = 0;
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
				data::MatcherData matcher = static_cast<const data::MatcherData &>(pop);

				if (l_good_matches <= (int)matcher.goodMatches().size())
				{
					best_match = matcher;
					l_good_matches = (int)matcher.goodMatches().size();
					if (matcher.patternImage().empty())
					{
						std::cout << "Pattern Image is empty in latch thread of DirLatch" << std::endl;
					}
					
				}
				this->result.push(matcher);
				cropImage = directory_img.nextImageFile();
				nbFile++;
			}
			std::cout << "Nb File Read : " << nbFile << std::endl;

			if (best_match.goodMatches_const().size() >= good_matches)
			{
				best_match.setBest();
				this->result.clear();
				this->result.push(best_match);
				if (best_match.patternImage().empty())
				{
					std::cout << "Pattern Image is empty in latch thread of DirLatch" << std::endl;
				}
				return OK;
			}

			cleanFilter(latch.get());
			cleanFilter(resultLatch.get());

			data::MatcherData empty_match;
			empty_match.setRequestImage(source.getMat());
			this->result.clear();
			this->result.push(empty_match);
			if (best_match.patternImage().empty())
			{
				std::cout << "Pattern Image is empty in latch thread of DirLatch" << std::endl;
			}
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
				if (md_result.patternImage().empty())
				{
					std::cout << "Pattern Image is empty in parent thread of DirLatch" << std::endl;
				}
				
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

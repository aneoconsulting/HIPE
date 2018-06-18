#include <algos/streaming/PushGraphToNode.h>
#include "data/FileVideoInput.h"
#include <regex>

int filter::algos::PushGraphToNode::getLastKnownIDFromDirectory()
{
	return id + 1;
}

HipeStatus filter::algos::PushGraphToNode::SendVideo(const cv::Mat& picture)
{
	if (!atomic_state.exchange(true))
	{
		if (!video)
		{
			if (host_or_ip == "192.168.6.43")
				video = std::make_shared<data::FileVideoInput>("bad_detection_1.avi", true);
			else
			{
				video = std::make_shared<data::FileVideoInput>("bad_detection_0.avi", true);
			}


		}
	}
	data::Data new_frame = video->newFrame();


	if (!new_frame.empty())
	{
		PUSH_DATA(new_frame);

		return OK;
	}

	video.reset();

	return END_OF_STREAM;

	PUSH_DATA(data::ImageData());
	return OK;
}



HipeStatus filter::algos::PushGraphToNode::process()
{
	
	while (_connexData.size() != 0)
		_connexData.pop();
	


	return OK;
}

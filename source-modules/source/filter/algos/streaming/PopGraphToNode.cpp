#include <algos/streaming/PopGraphToNode.h>
#include "data/FileVideoInput.h"
#include <regex>

int filter::algos::PopGraphToNode::getLastKnownIDFromDirectory()
{
	
	return id + 1;
}

/**
 * \brief Get the video from rtp stream
 * \return status if something is going wrong to reveive stream
 */
HipeStatus filter::algos::PopGraphToNode::receiveStream()
{
	if (sync != 0)
	{
		if (!atomic_state.exchange(true))
		{
			if (!video)
			{
				if (host_or_ip == "192.168.6.43" || host_or_ip == "127.0.0.1")
					video = std::make_shared<data::FileVideoInput>("bad_detection_1.avi", true);
				else
				{
					video = std::make_shared<data::FileVideoInput>("bad_detection_0.avi", true);
				}


			}
		}
	}
	return OK;
}



HipeStatus filter::algos::PopGraphToNode::process()
{
	while (_connexData.size() != 0)
		_connexData.pop();

	if (sync != 0)
	{
		HipeStatus receive_stream = receiveStream();
		if (receive_stream != OK)
		{
			std::stringstream error;
			error << "Cannot initialize stream for " << host_or_ip;
			
			throw HipeException(error.str());
		}

		data::Data new_frame = video->newFrame();

		PUSH_DATA(new_frame);
	}



	return OK;
}

#include <filter/datasource/VideoDataSource.h>
#include "data/FileVideoInput.h"


HipeStatus filter::datasource::VideoDataSource::process()
{
	if (!atomic_state.exchange(true))
	{
		if (!video)
		{
			video = std::make_shared<data::FileVideoInput>(url, loop);
			
		}
	}
	data::Data new_frame = video->newFrame();


	if (!new_frame.empty())
	{
		_connexData.push(new_frame);

		return OK;
	}

	return END_OF_STREAM;
	
	_connexData.push(data::ImageData());
	return END_OF_STREAM;
}

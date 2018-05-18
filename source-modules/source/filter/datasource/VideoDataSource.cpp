//@HIPE_LICENSE@
#include <filter/datasource/VideoDataSource.h>
#include <data/FileVideoInput.h>
#include <data/ImageArrayData.h>
#include <data/ImageData.h>

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
		PUSH_DATA(new_frame);

		return OK;
	}
	
	video.reset();
	
	return END_OF_STREAM;
	
	PUSH_DATA(data::ImageData());
	return END_OF_STREAM;
}

HipeStatus filter::datasource::VideoDataSource::intialize() 
{
	if (!atomic_state.exchange(true))
	{
		if (!video)
		{
			video = std::make_shared<data::FileVideoInput>(url, loop);
		}
	}
	data::Data new_frame = video->newFrame();
	video->closeFile();
	
	return OK;
}


	

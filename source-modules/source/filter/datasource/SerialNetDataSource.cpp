//@HIPE_LICENSE@
#include <filter/datasource/SerialNetDataSource.h>
#include <data/FileVideoInput.h>
#include <data/ImageData.h>
#include <corefilter/tools/cloud/SerialNetDataServer.h>
#include <boost/iostreams/stream.hpp>


HipeStatus filter::datasource::SerialNetDataSource::process()
{
	cv::Mat image;
	
	SerialNetDataClient::ptr_func func = &SerialNetDataClient::ImageHandler;
	serialNetDataClient.StartOnceAndConnect(address, port, std::bind(func, &serialNetDataClient));

	while (serialNetDataClient.isActive())
	{

		if (serialNetDataClient.imagesStack.trypop_until(image, 3000)) //
			break;
	}

	if (!serialNetDataClient.isActive())
	{
		serialNetDataClient.imagesStack.clear();
		serialNetDataClient.stop();
		return END_OF_STREAM;
	}

	serialNetDataClient.imagesStack.clear();

	data::ImageData result(image);

	PUSH_DATA(result);

	return OK;
}

void filter::datasource::SerialNetDataSource::dispose()
{
	serialNetDataClient.stop();
}

HipeStatus filter::datasource::SerialNetDataSource::intialize()
{
	
	return OK;
}




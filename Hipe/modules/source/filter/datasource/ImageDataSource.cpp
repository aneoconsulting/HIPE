#include <filter/datasource/ImageDataSource.h>

HipeStatus filter::datasource::ImageDataSource::process()
{
	if (!atomic_state.exchange(true))
	{
		imgs = static_cast<data::ImageArrayData>(data::FileImageData(url));
		_connexData.push(imgs);
		return OK;
	}
	_connexData.push(data::ImageData());
	return END_OF_STREAM;
}

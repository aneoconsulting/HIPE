//@HIPE_LICENSE@
#include <filter/datasource/ImageDataSource.h>

HipeStatus filter::datasource::ImageDataSource::process()
{
	if (!atomic_state.exchange(true))
	{
		imgs = static_cast<data::ImageArrayData>(data::FileImageData(url));
		PUSH_DATA(imgs);
		return OK;
	}
	PUSH_DATA(data::ImageData());
	return END_OF_STREAM;
}

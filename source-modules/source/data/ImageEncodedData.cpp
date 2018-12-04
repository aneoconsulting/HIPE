//@HIPE_LICENSE@
#include <data/ImageEncodedData.h>

namespace data
{
	ImageEncodedData& ImageEncodedData::operator=(const ImageEncodedData& left)
	{
		Data::registerInstance(left);
		This()._type = left.This_const().getType();
		This()._decorate = left.This_const().getDecorate();
		This()._channels = left.This_const().getChannelsCount();
		This()._width = left.This_const().getWidth();
		This()._height = left.This_const().getHeight();
		This()._compression = left.This_const().getCompression();

		_type = left.This_const().getType();
		_decorate = left.This_const().getDecorate();
		_channels = left.This_const().getChannelsCount();
		_width = left.This_const().getWidth();
		_height = left.This_const().getHeight();
		_compression = getCompression();

		return *this;
	}

	int ImageEncodedData::getChannelsCount() const
	{
		return This_const()._channels;
	}

	int ImageEncodedData::getWidth() const
	{
		return This_const()._width;
	}

	int ImageEncodedData::getHeight() const
	{
		return This_const()._height;
	}

	std::string ImageEncodedData::getCompression() const
	{
		return This_const()._compression;
	}
}

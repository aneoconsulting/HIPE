#include <pydata/pyConverterData.h>
#include <data/ImageData.h>
#include <pydata/pyImageData.h>


template <>
pyData* pydata::pyDataConverter::convertTo(data::ImageData& dataIn)
{
	pyImageData pyData();

	return nullptr;
}

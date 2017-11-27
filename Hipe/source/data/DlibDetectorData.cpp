#include <data/DlibDetectorData.h>

const std::vector<data::DlibDetectorData::detector_type>& data::DlibDetectorData::detectors_const() const
{
	return This_const()._detectors;
}

//std::shared_ptr<boost::shared_mutex> data::DlibDetectorData::mutex_ptr()
//{
//	return This()._mutex;
//}

data::DlibDetectorData& data::DlibDetectorData::operator=(const DlibDetectorData& left)
{
	data::Data::registerInstance(left);

	//_This = left._This;

	_type = left._type;
	_decorate = left._decorate;

	return *this;
}

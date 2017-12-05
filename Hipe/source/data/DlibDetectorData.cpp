#include <data/DlibDetectorData.h>

const std::vector<data::DlibDetectorData::detector_type>& data::DlibDetectorData::detectors_const() const
{
	return This_const()._detectors;
}

std::vector<data::DlibDetectorData::detector_type>& data::DlibDetectorData::detectors()
{
	return This()._detectors;
}

data::DlibDetectorData& data::DlibDetectorData::operator=(const DlibDetectorData& left)
{
	data::Data::registerInstance(left);

	_type = left._type;
	_decorate = left._decorate;
	_detectors = left._detectors;

	return *this;
}

void data::DlibDetectorData::copyTo(DlibDetectorData& left) const
{
	auto leftDetectors = left.This()._detectors;
	leftDetectors.insert(leftDetectors.end(), This_const().detectors_const().begin(), This_const().detectors_const().end());
}

bool data::DlibDetectorData::empty() const
{
	return This_const()._detectors.empty();
}

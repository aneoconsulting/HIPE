//@HIPE_LICENSE@
#include <DirPatternData.h>
#include <coredata/IODataType.h>
#include <coredata/IOData.h>
#include <data/ImageData.h>
#include <data/VideoData.h>
#include <data/DirectoryImgData.h>

namespace data
{



	/**
	* \brief Overloaded ssignment operator used to copy PatternData objects.
	* \param left The PatternData object to use as a source for the copy
	* \return Returns a reference to the copied PatternData object
	*/
	DirPatternData& DirPatternData::operator=(const DirPatternData& left)
	{
		if (_This == left._This) return *this;

		Data::registerInstance(left);
		_type = left.getType();
		_decorate = left.getDecorate();

		return *this;
	}

	/**
	* \todo
	* \brief Overloaded insersion operator used to copy PatternData objects.
	* \param left The PatternData object to use as a source for the copy
	* \return Returns a reference to the copied PatternData object
	*/
	DirPatternData& DirPatternData::operator<<(const DirPatternData& left)
	{
		if (_This == left._This) return *this;

		Data::registerInstance(left._This);

		return *this;
	}

	/**
	* \brief Overloaded insetion operator used to overwrite the PatternData object's request image with another one
	* \param left The ImageData used to overwrite The PatternData's one
	* \return Returns a reference to the copied PatternData objet
	*/
	DirPatternData& DirPatternData::operator<<(const ImageData& left)
	{
		This()._inputSource = static_cast<Data>(left);
		return *this;
	}


	Data DirPatternData::imageSource() const
	{
		return This_const()._inputSource;
	}

	DirectoryImgData DirPatternData::DirectoryImg() const
	{
		return This_const().dir;
	}

	
	void DirPatternData::copyTo(DirPatternData& left) const
	{
		left.This()._inputSource = This_const()._inputSource;
		left.This().dir = This_const().dir;
	}


	
	bool DirPatternData::empty() const
	{
		if (This_const()._inputSource.empty() && This_const().dir.empty()) return true;

		return false;
	}

	DirPatternData & DirPatternData::operator=(const Data& left)
	{
		Data::registerInstance(left);
		_type = left.getType();
		_decorate = left.getDecorate();

		return *this;
	}
}


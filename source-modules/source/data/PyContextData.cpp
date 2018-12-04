//@HIPE_LICENSE@
#include <data/PyContextData.h>



namespace data
{
	PyContextData::PyContextData(IOData::_Protection priv): IOData(IMGF)
	{
	}

	PyContextData::PyContextData(IODataType dataType): IOData(dataType)
	{
		/*Data::registerInstance(new PyContextData(IOData::_Protection()));
		This()._type = dataType;
		This()._array.resize(1);*/
	}

	PyContextData::PyContextData(): IOData(PYCTX)
	{
		Data::registerInstance(new PyContextData(IOData::_Protection()));
		This()._type = PYCTX;
		
		
		This()._init = false;
	}

	PyContextData::PyContextData(const PyContextData& ref): IOData(PYCTX)
	{
		Data::registerInstance(ref._This);
		This()._type = ref.This_const()._type;
		_decorate = ref._decorate;
	}

	PyContextData::PyContextData(const Data& ref): IOData(IMGF)
	{
		if (ref.getType() != IMGF) throw HipeException(
			"ERROR data::PyContextData::PyContextData - Only Connexdata should call this constructor.");

		Data::registerInstance(ref);
		This()._type = ref.getType();
		_decorate = ref.getDecorate();
	}

	PyContextData::~PyContextData()
	{
		IOData::release();
	}

	void PyContextData::copyTo(PyContextData& left) const
	{
		if (getType() != left.getType())
			throw HipeException("Cannot left argument in a PyContextData");
		

		PyContextData::copyTo(static_cast<PyContextData &>(left));
	}

	PyContextData& PyContextData::operator=(const PyContextData& left)
	{
		//if (left.getType() != PYCTX) throw HipeException("ERROR data::PyContextData::PyContextData - Only Connexdata should call this constructor.");

		Data::registerInstance(left);
		This()._type = left.getType();
		_decorate = left.getDecorate();

		return *this;
	}
}

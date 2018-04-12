#pragma once

#include <coredata/IODataType.h>
#include <memory>

namespace data
{
	class DATA_EXPORT Data
	{
	public:
		typedef Data _classtype;
	protected:
		IODataType _type;
		std::shared_ptr<Data> _This;
		bool _decorate = false;
		std::string _label;

	public:
		std::string getLabel() const;

		void setLabel(const std::string& cs);

	protected:
		Data(IODataType datatype);

	public:
		Data();

		Data(const Data& data);

	public:
		virtual ~Data();

		void registerInstance(const Data & childInstance);

		void registerInstance(Data* childInstance);

		void registerInstance(std::shared_ptr<Data> childInstance);


		IODataType getType() const;

		bool getDecorate() const;

		void copyTypeTo(Data& left);

		virtual void copyTo(Data& left) const;

		virtual bool empty() const;

		Data& operator=(const Data& left);

		void release();

		void setType(const IODataType io_data_type);
	};
}
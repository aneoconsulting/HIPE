#pragma once
#include <string>
#include <core/HipeStatus.h>
#include <filter/data/IOData.h>
#include <filter/data/DataAccess.h>
#include <filter/data/ConnexData.h>

namespace filter
{
	/**
	 * \brief The Model class is the foundatation of every Hipe's filter. It's the base class they should implement.
	 * [TODO]
	 */
	class Model
	{
	protected:
		std::string _name;
		std::string _constructor;
		int _level;

		Model() : _level(0)
		{
			_name = "NO_NAME";
			_constructor = "MODEL_ERROR";
		}

	public:
		virtual ~Model() {};

		const std::string & getConstructorName() const { return _constructor; }

		void setLevel(int level) { _level = level; }
		int getLevel() { return _level; }

		virtual void cleanUp()
		{

		}

		virtual void dispose() {}


		const std::string & getName() const { return _name; }

		void setName(const std::string& algoName)
		{
			_name = algoName;
		}



		virtual void addDependencies(Model *filter) = 0;
		virtual void addChildDependencies(Model *filter) = 0;

		virtual void addDependenciesName(std::string filter) = 0;
		virtual void addChildDependenciesName(std::string filter) = 0;

		virtual std::map<std::string, Model *>  getParents() const = 0;


		virtual std::map<std::string, Model *> getChildrens() const = 0;

		virtual Model * getRootFilter() = 0;

		virtual HipeStatus process() = 0;

		virtual Model &operator<<(data::Data & element) = 0;

		virtual Model &operator<<(cv::Mat & element) = 0;

		virtual data::ConnexDataBase & getConnector() = 0;
	};

}

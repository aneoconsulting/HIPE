#pragma once
#pragma once
#include <string>
namespace http
{
	class JsonTree;

	class AbstractJsonTree
	{
		std::string requestName;
	public:
		AbstractJsonTree() = default;
		virtual ~AbstractJsonTree();
		
		virtual AbstractJsonTree & Add(std::string key, std::string value) = 0;

		virtual AbstractJsonTree & AddChild(std::string key, JsonTree & value)=0;
		static AbstractJsonTree* Create();
		virtual void read_json(std::basic_istream<char> stream)=0;
		virtual size_t count()=0;
		///Add top layer for ptree method...
	};
}

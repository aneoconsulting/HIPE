#include <http/JsonTree.h>

namespace http {
	http::AbstractJsonTree::~AbstractJsonTree(){}

	AbstractJsonTree* AbstractJsonTree::Create()
	{
		auto jsonTree = reinterpret_cast<AbstractJsonTree *>(new JsonTree);
		return jsonTree;
	}
}

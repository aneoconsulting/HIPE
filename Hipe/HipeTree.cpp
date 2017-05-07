#include <boost/ptree.hpp>


class HipeTree
{
	
	public:
	HipeTree()
	{
		
	}
	HipeTree(const HipeTree & tree)
	{
		
	}
	
	HipeTree getChild()
	{
		throw HipeException("not yet implemented");
	}

	template <class Ty>
	T & get()
	{
		throw HipeException("not yet implemented");
	}
	
	
	
}
#include  <filter/algos/DemuxPattern.h>

namespace filter {
	namespace algos {
		HipeStatus DemuxPattern::process()
		{
			auto data = _connexData.pop();
			if(data.getType()!= data::DIRPATTERN)
			{
				throw HipeException("Demux needs DirPattern");
			}
			
			/*for(auto d : data.patterns())
			{
				_connexData.pushonce(d);
			}*/
			return OK;
		}
	}
}
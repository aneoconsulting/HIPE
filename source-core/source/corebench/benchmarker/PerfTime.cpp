#include <corebench/benchmarker/PerfTime.h>



HipeStatus corefilter::tools::PerfTime::process()
{
	while (_connexData.size() != 0)
		_connexData.pop();

	

	return OK;
}

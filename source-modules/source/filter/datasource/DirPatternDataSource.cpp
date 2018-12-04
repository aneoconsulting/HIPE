//@HIPE_LICENSE@
#include <filter/datasource/DirPatternDataSource.h>

HipeStatus filter::datasource::DirPatternDataSource::process()
{
	if (!atomic_state.exchange(true))
	{
		if(_connexData.size() !=2)
		{
			throw HipeException("DirPatternDataSource needs two data (DirImgDataSource and Video or image data source");
		}
		auto data1 = _connexData.pop();
		auto data2 = _connexData.pop();
		if (data1.getType() == data::VIDF|| data1.getType() == data::IMGF)
		{	
			data::DirPatternData dirpattern(data1, data2);
			PUSH_DATA(dirpattern);			
		}
		
		else //directory
		{
			data::DirPatternData dirPatterData(data2, data1);
			PUSH_DATA(dirPatterData);
		}
	return OK;
	}
	PUSH_DATA(data::DirPatternData());
	return END_OF_STREAM;
}

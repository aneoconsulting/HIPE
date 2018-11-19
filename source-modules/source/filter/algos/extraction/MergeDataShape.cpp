//@HIPE_LICENSE@
#include <algos/extraction/MergeDataShape.h>

bool filter::algos::MergeDataShape::isShapeData(const data::Data& input)
{
	if (input.getType() == data::SHAPE)
	{
		return true;
	}

	return false;
}

HipeStatus filter::algos::MergeDataShape::process()
{
	if (_connexData.empty())
		return OK;

	// Separate shapes from source image
	std::vector<data::ShapeData> shapes;
	data::ImageData image;

	while (!_connexData.empty())
	{
		data::ShapeData shape = _connexData.pop();
		if (shape.This_const().getType() == data::SHAPE)
			shapes.push_back(shape);
	}
	if (shapes.size() == 1)
	{
		PUSH_DATA(shapes[0]);
	}
	else if (shapes.size() > 1)
	{
		data::ShapeData merger = shapes[0];
		for (size_t i = 1; i < shapes.size(); i++)
		{
			//v1.insert(v1.end(), v2.begin(), v2.end());
			merger.CirclesArray().insert(merger.CirclesArray().end(), shapes[i].CirclesArray().begin(), shapes[i].CirclesArray().end());
			merger.ColorsArray().insert(merger.ColorsArray().end(), shapes[i].ColorsArray().begin(), shapes[i].ColorsArray().end());
			merger.FreeshapeArray().insert(merger.FreeshapeArray().end(), shapes[i].FreeshapeArray().begin(), shapes[i].FreeshapeArray().end());
			merger.IdsArray().insert(merger.IdsArray().end(), shapes[i].IdsArray().begin(), shapes[i].IdsArray().end());
			merger.PointsArray().insert(merger.PointsArray().end(), shapes[i].PointsArray().begin(), shapes[i].PointsArray().end());
			merger.RectsArray().insert(merger.RectsArray().end(), shapes[i].RectsArray().begin(), shapes[i].RectsArray().end());
		}
		PUSH_DATA(merger);
	}

	return OK;
}

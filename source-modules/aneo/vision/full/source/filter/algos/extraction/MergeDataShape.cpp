//READ LICENSE BEFORE ANY USAGE
/* Copyright (C) 2018  Damien DUBUC ddubuc@aneo.fr (ANEO S.A.S)
 *  Team Contact : hipe@aneo.fr
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  In addition, we kindly ask you to acknowledge ANEO and its authors in any program 
 *  or publication in which you use HIPE. You are not required to do so; it is up to your 
 *  common sense to decide whether you want to comply with this request or not.
 *  
 *  Non-free versions of HIPE are available under terms different from those of the General 
 *  Public License. e.g. they do not require you to accompany any object code using HIPE 
 *  with the corresponding source code. Following the new licensing any change request from 
 *  contributors to ANEO must accept terms of re-license by a general announcement. 
 *  For these alternative terms you must request a license from ANEO S.A.S Company 
 *  Licensing Office. Users and or developers interested in such a license should 
 *  contact us (hipe@aneo.fr) for more information.
 */

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
		data::ShapeData merger;
		shapes[0].copyTo(merger);

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

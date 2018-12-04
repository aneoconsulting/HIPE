[0]: #content
[3]: #-develop-with-hipe
[31]: #-develop-filters
[311]: #-register-macro
[312]: #-filters-and-data
[313]: #-expose-fields
[314]: #-the-process-method
[315]: #-reference-a-filter
[316]: #-example-of-an-implemented-filter
[4]: #-things-to-not-forget

[cmake_steps_vid]: path "cmake steps"
[postman_steps_vid]: path "Postman steps"
[visual_studio_path_vid]: path "Visual Studio's PATH"

How to start with HIPE
==================

# Content
3. [Develop with HIPE][3]
	* [Develop filters][31]
4. [Things to not forget][4]


# [[~][0]] Develop with HIPE
## [[..][3]] Develop filters
* [REGISTER macro][311]
* [Filters and data][312]
* [Expose fields][313]
* [The process method][314]
* [Reference a filter][315]
* [Example of an implemented filter][316]
<br></br><br></br><br></br>

In HIPE, a filter is modelized by a C++ class.    
To be interpreted as a filter a class must meet the following criteria:    

* It must implement the ``IFilter`` interface
* Its constructor must be defined by the ``REGISTER`` macro
* It must override the ``process()`` method
* It must explicitly specify the awaited input and output data types
* It must at least possess an exposed field, even if not used
* It must be referenced by the ``ADD_CLASS`` macro

**Note:** all the filters headers are located on the ``/Hipe/header/filter/algos/`` directory, and the sources in the ``/Hipe/source/filter/algos/`` one.    
**Note:** In Visual Studio, the filters are located in the _filter_ project.


### [[..][31]] REGISTER macro
It is not the developer’s task to write the constructor of each filter. It is internally done by the ``REGISTER`` macro.    
The macro awaits 2 parameters:
```c++
REGISTER(Constructor, params)
{

}
```
Where:

* ``Constructor`` is the class name
* ``params`` are the constructor parameters (mind that you must manually write the brackets around the parameters)

The macro will only handle the constructor’s signature. Its body, like any constructor or method must be defined within the two curly brackets following it.
 
As an example, for a fictive filter named "FilterExample", we’ll have the following syntax:
```c++
REGISTER(FilterExample, ())
{

}
```


### [[..][31]] Filters and data
When implementing a new filter, you must specify the type of the awaited input data, as well as the one it will output. You also need to specify the filter’s behavior regarding the handling of the data’s allocated memory. Should the input data be kept? Should it be deleted and overwritten? etc.

All those steps are done by the ``CONNECTOR(DataIn, DataOut)`` macro

Where:

* ``DataIn`` is the expected input data type
* ``Dataout`` is the expected output data type

To do this, the macro will add to the class a field of type ``ConnexData``, which, for every filter, will behave as a port and be used by HIPE to link its input to the output of its parent, so the data types must match (i.e. the ``DataOut`` type must be _compatible_ with the ``DataIn`` type of the child filter).    
The behavior regarding the handling of the data’s memory will be specified after the ``REGISTER`` macro (which internally uses an initialization list) by specializing the created ``ConnexData`` object.

It will then be possible in your code to get the input data and set the output data using these two methods
```c++
//	Get the port's stored data
DataIn ConnexData::pop()

// Set the port's stored data
void ConnexData::push(DataOut data);
```

Let’s take as an example a fictive filter which awaits an image in input, and will output an array of regions of interest. As the filter will only do that, we do not need to keep _its_ input data alive at the end of _its_ process method. 

Such a filter can be represented by using the following macros:
```c++
CONNECTOR(data::ImageData, data::PatternData);
```
and
```c++
REGISTER(ExtractROI, ()), _connexData(data::InData)
```

Here is a list of the primary data types in HIPE:    

| **Type** 	| **Description** 	|
|------	|-------------	|
|	``ImageData``	|	an image	|
|	``ImageArrayData``	|	images	|
|	``PatternData``	|	image + region(s) of interest	|    
<br></br>
    
And the primary data behaviors:    

| **Type** 	| **Description** 	|
|------	|-------------	|
|	``INDATA``	|	The Input data will be deleted from the ConnexData port when the ``pop()`` method will be called. You will need to call ``push()``  |
|	``INOUT``	|	The ``pop()`` method will return a reference to the data store in the ConnexData port. Working on it will directly affect the port. You don’t need to call ``push()``	|
|	``OUTDATA``	|	No input data is stored on the ConnexData port. You will need to call ``push()``	|


### [[..][31]] Expose fields
Any exposed field’s value can be set at runtime by a JSON request. You need to use the ``REGISTER_P`` macro to expose a parameter:

```c++
REGISTER_P(typef, params)
```

Where:

* ``typef`` is the exposed field’s type
* ``params`` is its name

An exposed field can be used like any class field, so it is possible to set it to a default value in the class constructor.

**Note:** It is mandatory to have at least one exposed field, even if never used.

    
As an example, to expose an integer, we’ll use the following macro:

```c++
REGISTER_P(int, desired_value);
```


### [[..][31]] The process method
**[TODO]**

The process method is the main method of every filter. It is the one who will be called by the scheduler.

Its signature is:
```c++
HipeStatus process();
```


### [[..][31]] Reference a filter
HIPE dynamically create a list containing all the implemented filters. To populate this list, HIPE must know which classes are filters and which are not. Implementing the IFilter interface is insufficient: you must use the ``ADD_CLASS`` macro to do so.

The macro awaits multiple parameters:

```c++
ADD_CLASS(classname, ...)
```

Where:

* ``classname`` is the name of the class
* ``...`` is the list of all the exposed fields separated by commas

    
**Note:** The macro is to be used _outside_ the class’ body.    
**Note:** If a parameter is defined using the ``REGISTER_P`` macro but is not added to the ``ADD_CLASS`` parameters list, it will not be exposed.    


### [[..][31]] Example of an implemented filter
As an example, here is the code of the Resize filter of the prior mentioned Resize JSON request:    

```c++
#include <filter/tools/RegisterClass.h>     // The RegisterClass.h header defines all the used macros
#include <filter/IFilter.h>                 // The IFilter.h is the basis interface of every filter
#include <core/HipeStatus.h>                // The HipeStatus header contains HIPE's known return values. It is used by the process method

#include <filter/data/ImageData.h>          // The ImageData.h header contains the definitions of the images handling data classes


class Resize : public filter::IFilter
{
	// The CONNECTOR macro specify the filter's awaited input and output data types and creates a communication port (a data::ConnexData object) which will handle the data between filters
	CONNECTOR(data::ImageData, data::ImageData);


	// The REGISTER handles the declaration of the class' constructor
	REGISTER(Resize, ()), _connexData(data::INOUT)	// You must specify the behavior of the communication port.
	{
		// Here is the constructor's body
		// You can here set exposed fields to default values
		ratio = 1;
	}


	// To expose a field, you must declare it with the REGISTER_P macro
	REGISTER_P(double, ratio);

public:
	// The process method is the main method of each filter. It will be called by the chosen scheduler to process the data with the filter
	HipeStatus process()
	{
		// Assert the resize ratio is positive
		if(!ratio > 0.0)	throw HipeException("resize ratio must be positive!");
		
		// Get the image data with DataIn ConnexData::pop(). Refer to the "primary data behaviors" table for more information
		data::ImageData imageData = _connexData.pop();
		cv::Mat image = _connexData.getMat();	
		
		// Downscale the image
		int w = myImage.cols;
		int h = myImage.rows;
		
		cv::Size size(w/ ratio, h/ ratio);
		cv::resize(myImage, myImage, size, 0.0, 0.0, cv::INTER_CUBIC);
		
		return OK;
	}
};


// Reference the class as a valid filter
ADD_CLASS(Resize, ratio);
```    

# [[~][0]] Things to not forget
You will find in this part the things you should always keep in mind when you will use HIPE

* HIPE is target dependent. All the libraries were compiled in Debug or Release mode. You can't use them in a Release situation if when you built HIPE you selected the Debug target. You _will_ encounter errors.
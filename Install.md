[0]: #content
[1]: #-build-hipe
[11]: #-requirements
[111]: #-cmake-372
[112]: #-nvidia-cuda-80
[113]: #-nvidia-cudnn
[114]: #-windows-platforms
[115]: #-linux-platforms
[116]: #-libraries-and-other-dependencies
[12]: #-project-building
[121]: #-windows
[122]: #-linux
[13]: #-set-up-your-environment
[131]: #-windows-1
[132]: #-linux-1
[2]: #-how-to-use-hipe
[21]: #-json-requests
[211]: #-syntax-of-a-json-request
[2111]: #-usefulness-of-the-requests-name
[2112]: #-processing-chain-and-its-filters
[2113]: #-scheduler
[2114]: #-linking-data-and-supported-data-types
[212]: #-send-requests-to-hipe
[2121]: #-postman
[21211]: #-import-requests
[21212]: #-create-and-edit-a-request
[21213]: #-save-your-request
[21214]: #-send-a-request
[2122]: #-web-interface
[3]: #-develop-with-hipe
[31]: #-develop-filters
[311]: #-register-macro
[312]: #-filters-and-data
[313]: #-expose-fields
[314]: #-the-process-method
[315]: #-reference-a-filter
[316]: #-example-of-an-implemented-filter
[4]: #-things-to-not-forget


[![License: AGPL v3](https://img.shields.io/badge/License-AGPL%20v3-blue.svg)](https://www.gnu.org/licenses/agpl-3.0)

[cmake_steps_vid]: path "cmake steps"
[postman_steps_vid]: path "Postman steps"
[visual_studio_path_vid]: path "Visual Studio's PATH"

How to start with HIPE
==================

# Content
1. [Build HIPE][1]
	* [Requirements][11]
	* [Project Building][12]
	* [Set up your environment][13]
2. [How to use HIPE][2]
	* [JSON Requests][21]
		* [Syntax of a JSON request][211]
		* [Send requests to HIPE][212]
3. [Develop with HIPE][3]
	* [Develop filters][31]
4. [Things to not forget][4]



# [[~][0]] Build Hipe


## [[..][1]] Requirements
* [CMake][111]
* [Nvidia CUDA][112]
* [Nvidia cuDNN][113]
* [Windows Specific][114]
* [Linux Specific][115]
* [Libraries and other dependencies][116]



HIPE can be built and used on a Windows or Linux platform.    
Here is a summary list of all the primary third party programs and libraries needed. It is not required to build all the dependencies separately as they were all packed in the HipeExternal archive.

-	Third party
	-	CMake 3.7.2
-	Dependencies
	-	Nvidia CUDA 8.0
	-	Nvidia CuDNN for CUDA 8.0
	-	HDF5
	-	caffe
	-	dlib
	-	boost
-	Windows platforms
	-	OpenCV 3.1.0
	-	Visual Studio 2015 Update 3 (VC140 build tools)
-	Linux platforms
	-	OpenCV 3.2.0
	-	GCC 5+
	-	Makefile

### [[..][11]] CMake 3.7.2
To build HIPE, the required version of CMake is 3.7.2 (this version only).

CMake can be [downloaded here]( https://cmake.org/download/)    


### [[..][11]] Nvidia CUDA 8.0
Some of the required dependencies (like OpenCV) were built using CUDA. It allows HIPE to use any Nidia GPU, if present, to use its power and speed up things.    
Nvidia CUDA toolkit 8.0 is needed even if no GPU is present on the hardware. In that case, you are not forced to install the display driver nor the PhysX one. You can do so by selecting the advanced installation mode when prompted, and unchecking the _Driver_ and _Other components_ marks on the components to install list.

The toolkit can be downloaded on the [Nvidia CUDA dedicated website](https://developer.nvidia.com/cuda-toolkit-archive)

**Note:** The version 8.0 must be present on your system’s _PATH_, as HIPE's building configuration files will look for it here. If you already have multiple versions of the toolkit installed, or you don’t want it to be referenced in it,  you can also fill the ``CUDA_SDK_ROOT_DIR`` and ``CUDA_TOOLKIT_ROOT_DIR`` fields when the “Could not find CUDA” CMake error will occur in the building phase.

### [[..][11]] Nvidia cuDNN    
Nvidia cuDNN library is also required. You must download a version matching the downloaded version of CUDA (the v7.0.3 for CUDA 8.0 Windows 10 library should work).    
You can download it from the [Nvidia CuDNN dedicated website](https://developer.nvidia.com/rdp/form/cudnn-download-survey), but you will need to have (or create) a Nvidia devevloper account.    
The content of the archive must be placed where you extracted and installed CUDA. In other words, put the content of the bin, lib, and include foldes in the CUDA's ones.    

### [[..][11]] Windows platforms
On Windows platforms, _Visual Studio 2015 Update 3_ was used to compile the dependencies and is the main tool to work on HIPE. The project will **_only_** work with this version. At least, only when using the VC140 build tools.    
    
If you don’t have those build tools or any Visual Studio version installed, you can find the free 2015 Community one in the [older downloads page](https://www.visualstudio.com/fr/vs/older-downloads/) on the Visual Studio website. Don't forget to install the Update 3 package, or download the full version already prepacked with the update.    

**Note:** You will need an account and join the Visual Studio dev essentials program with it to download it. You can in fact use any microsoft account.    
**Note:** The first time you will try to access the download page with your account, there's a possibility that no content will be shown. In that case you will need to go to the front page of the Visual Studio website (or the Benefits tab) to initialize your account (note that you will also find a link to join the dev essentials program on this page). When you will return to the download page, the different versions will then be displayed.    

### [[..][11]] Linux platforms  
**[TODO]**    
On Linux, you need gcc 5+, CMake and Makefile to build HIPE. Make sur your gcc version is sufficent before trying to build.

### [[..][11]] Libraries and other dependencies
As mentioned before, HIPE uses multiple libraries.    
To make things easier, we decided to regroup them in a package. The main goal of this package is portability: extract it where you want. There is no setup to be done and there will be no conflict with your own libraries.    

**Important:** Two files are missing in the HipeExternal archive (libiomp5md.dll & libiomp5md.pdb). You can find them where you downloaded it. These two files are to be placed in a _bin_ folder (you have to create one) in the _win64/intel64_win_ one.   

**Note:** The package can be extracted anywhere but it _must_ keep its hierarchy as HIPE's configuration files will look for precise locations: Don't move or rename anything in the win64 folder.    
    
## [[..][1]] Project Building
* [Windows][131] 
* [Linux][132]
<br></br><br></br><br></br>


### [[..][12]] Windows
To build HIPE's Visual Studio project files, you must use CMake.    

1.	Fill in the HIPE sources and build directories fields. You can chose the build directory you want, but remember that HIPE sources are located on the Hipe subdirectory.
2.	Click on _Configure_ and select the _Visual Studio build tools (Visual Studio 14 2015 Win64)_ build tools then confirm by clicking on the _Finish_ button. The configuration will start. If not, click again on _Configure_.
3.	An error (Could not find ``HIPE_EXTERNAL`` variable) will occur. You will have to fill the ``HIPE_EXTERNAL`` field with the path of the directory where the extracted HipeExternal archive is located. The expected directory is the one containing the _win64_ one.
4.	When the configuration is done, you must click on _Generate_ to create the Visual Studio project files. The files will be generated on the selected build directory.


### [[..][12]] Linux
**[TODO]**    
The steps are similar on a Linux platform but you'll have to select GCC as build tools instead of Visual Studio.


## [[..][1]] Set up your environment
* [Windows][131]
* [Linux][132]
<br></br><br></br><br></br>


You must go through a few configuration steps before being able to start working with HIPE.


### [[..][13]] Windows
In the Visual Studio solution, the default start up project to set is _hipe_server_.    

The compiled dependencies files (*.dll files) must be referenced in your system folder, your system’s PATH or Visual Studio's debug one. You can also copy them next to the _hipe_server.exe_ executable file.    

Here is the list of the needed dependencies. The dependencies are all coming from the HipeExternal archive.    

-	boost
-	OpenCV
-	FFmpeg
-	GStreamer
-	CUDA
-	Liblept
-	HDF5
-	Intel

**Note:** If you want to copy them next to the hipe_server.exe binary, you will find it in the _chosen_build_directory_/target/_chosen_build_target_/

where:

* _chosen_build_directory_ is the build directoy you chose in CMake
* _chosen_build_target_ is the debug or release chosen build target
 
If you want to use Visual Studio’s debug path, you can add them by completing the field _Environment_ of the _hipe_server_ Debugging properties tab (right click on hipe_server->Properties->Debugging Tab). The variable can contain multiple properties all separated by new lines.

Assuming you start with an empty PATH variable you will end up with a value similar to the following one:
```
PATH=%HipeExternal%/win64/boost_1_62_0/lib64-msvc-14.0;%HipeExternal%/win64/opencv/x64/vc14/bin;%HipeExternal%/win64/intel64_win/bin;%HipeExternal%/win64/ffmpeg/bin;%HipeExternal%/win64/gstreamer/1.0/x86_64/bin;%HipeExternal%/win64/liblept/bin;%HipeExternal%/win64/hdf5/bin;C:/NVIDIA/CUDA/v8.0/Toolkit/bin;%PATH%
```

**Note:** ``%HipeExternal%`` is to replace by the location where you extracted the HipeExternal package. You can use any text editor to do that faster.     
**Note:** The whole PATH variable **must be set on the _same_ line**!    
**Note:** Working with this debug PATH overwrites the system’s one. To still use it you must add ``%PATH%`` at the end of the value.    
**Note:** Don't forget: The whole PATH variable **must be set on the _same_ line**!    


### [[..][13]] Linux
**[TODO]**    
You will have to refer to the steps done on Windows platforms and redo them in your chosen environment.    

**Note:** There is a slight difference with the linux hipe_server binary file. It is located on the _chosen_build_directory_/_chosen_target_ folder.

# [[~][0]] How to use HIPE
## [[..][2]] JSON Requests    
* [Syntax of a JSON request][211] 
    * [Usefulness of the request name][2111] 
	* [ Processing chain and its filters][2112] 
	* [Scheduler][2113] 
	* [Linking data and supported data types][2114] 
* [Send requests to HIPE][212]
	* [Postman][2121] 
        * [Import requests][21211] 
		* [Create and edit a request][21212]
		* [Save your request][21213] 
		* [Send a request][21214]
	* [Web Interface][2122]
<br></br><br></br><br></br>


HIPE is a http server.    
The communication method we use to transfer data is to send them using the JSON syntax.    
If you never worked with it, you can quickly learn the basics by reading its documentation on [the w3schools website](https://www.w3schools.com/js/js_json_intro.asp)


### [[..][21]] Syntax of a JSON request    
* [Usefulness of the request name][2111]
* [ Processing chain and its filters][2112] 
* [Scheduler][2113] 
* [Linking data and supported data types][2114] 
<br></br><br></br><br></br>


Here is a request as an example:
```json
{
	"name": "resize",


	"filters": [
		{
			"RootFilter": { 
				"name": "root" 
			} 
		},
		{
			"Resize": {
				"name": "resize",
				"need": ["root"],
				"ratio": 4
			}
		},
		{
			"ShowImage": { 
				"name": "show",
				"need": ["resize"],
				"wait": 1
			}
		}
	],
	
	
	"orchestrator": "DefaultScheduler",
	
	
	"data": {
		"type": "IMGF",
		"path":"C:\\workspace\\data\\images\\image_01.jpg"
	}
}
```
The body of a valid JSON request contains:

* Its name (``name`` field)
* The processing chain (``filters`` array)
* The scheduler symbolizing how the chain's nodes will be processed (``orchestrator`` filed)
* The data which will be processed by the chain (``data`` field)

#### [[..][211]] Usefulness of the request name
The name _is_ the most important information on a request. As your requests graphs will grow, building then processing them will take some time. One of the key concepts of HIPE is to capitalize and reuse the more possible things, _even requests_.    
It is not currently possible to do so but in a future version you will be able to use a saved in database graph just by sending a JSON request containing its name, the desired scheduler, and the data to be processed. The filters array will not be needed anymore.

#### [[..][211]] Processing chain and its filters
The processing chain can be seen as a graph where each node is a different _filter_ which will “do something” on its input data. So basically, a _filter_ can represent an algorithm, like one which will alter an image, or another one which will find and identify an object in a video.

Let’s analyze in detail a filter extracted from the last example:
```json
"Resize": {
	"name": "resize",
	"need": ["root"],
	"ratio": 4
}
```
Each JSON filter node must contain:

* A name, required for its identification (``name`` field)
* A list of all its direct parents nodes’ names (``need`` array)
* If existing, its exposed calibration parameters (in our case the ``ratio`` field)

In the JSON request body, the order on which all the filters are written is not important. You are not forced to do it in their logical processing order, the ``need`` arrays and ``name`` fields will be used by the chosen scheduler to reorder them.

As for the exposed calibration parameters, they are variables exposed in the source code. It permits us to fine tune their behavior without having to each time recompile the whole project again.

**Note:** In the case where the processing chain is saved in HIPE's database, you don't need to write the filters array part in your JSON request. The graph will be found using the request's name.

#### [[..][211]] Scheduler
As said before, the orchestrator is the scheduler which will reorder the processing of the nodes, taking into account their dependencies and the available hardware.     
For now, only the _DefaultScheduler_ model is available, where each filter will be processed sequentially respecting the dependencies order.


#### [[..][211]] Linking data and supported data types
**[TODO]**    
HIPE can work with images, videos (or image sequences), as well as live data streams acquired from hardware like video cameras or RTP (**R**eal-time **T**ransport **P**rotocol).

Here is a table containing the primary data types and their corresponding identifier tags

|   **Type**	|   **Identifier**	|   **Note**	|
|---	|---	|---	|
|   image	|   ``IMGF``	|   path: "*image_path*"	|
|   video	|   ``VIDF``	|   path: "*video_path*"	|
|   webcam	|   ``VIDF``	|   path: "*id_cam"* (i.e. 0,1, ...)"	|
|   image sequence	|   ``SEQIMGD``	|   path: "*images_dir_path*"	|


To link some data to your request, you must add a ``data`` field. As shown in the example, the field is not part of the ``filters`` array.

The field must contain:

* The data type containing the identifier of the data type you want to use (``type`` field)
* The data path (``path`` field)

**Note:** Those data types can also be used as output. To stream the output we use GStreamer and the ``StreamResultFilter`` filter. More information will be added to the API documentation.

### [[..][21]] Send requests to HIPE
The tool we use to edit and send JSON requests to HIPE is Postman. It is not a prerequisite, so you can alternatively use any tool you want.    
Assuming you want to use Postman, you can download it [here](https://www.getpostman.com/).    
You will also find collections of tests requests in the ``/Hipe/tests/data/request/`` folder. They are in the Postman format but it internally uses text, so you can open them in any text editor you want to peek at their content.


#### [[..][212]] Postman
* [Import requests][21211]
* [Create and edit a request][21212]
* [Save your request][21213] 
* [Send a request][21214] 
<br></br><br></br><br></br>


##### [[..][2121]] Import requests
In Postman, every request must be part of a collection. To import a collection, you must simply click on the _import_ button (1.) in the upper left corner of Postman’s interface. The collection will be shown in the left side bar. If the side bar is hidden you can show it by clicking on button _Show/Hide Sidebar_ (2.) next to the _import_ button.


##### [[..][2121]] Create and edit a request
In Postman, all the opened requests are shown in different tabs. By default, if no request is already opened, a new one will be shown and it is possible to directly edit it. But, if at least one request is opened, the default empty one will be hidden and you will need to click on the plus sign button at the right of the last opened request’s tab to create a new one (3.).   
To be able to write the request, you must chose the ``POST`` mode instead of the ``GET`` mode (4.). It will unlock the _Body_ tab. It is here that you will write the content of the request. You will also have to check the _raw_ button to enable text edit, then chose the _JSON (application/json)_ instead of _text_ in the orange drop down menu to enable syntax highlighting.


##### [[..][2121]] Save your request
You can save your request by clicking on the _Save_ button at the right of the request’s tab window (5.). Remember that in Postman, every request must be linked to a collection. If no collection was already created, you will have to create a new one. You can do so by clicking on the _+ Create Collection_ button at the bottom of the opened save dialog.


##### [[..][2121]] Send a request
HIPE’s http server listen on port 9090.    
When used in a debug environment, you will probably deploy the server locally. In that case, the address will be localhost. If deployed on a remote environment, you will have to input the corresponding IP address (6.).    
A complete address will be similar to: ``localhost:9090/json``.    
To send the request you will have to click on the blue _Send_ button (7.). The server will then send its response when the processing will be done. The response is also in JSON format and will be shown just below the body of the sent request.

**Note:** HIPE internally uses the 9090 port to listen for and send JSON requests but It will change in the near future with the apparition of configuration files. You will be able to manually set the port you want to use.

#### [[..][212]] Web Interface
An alternative to postman is the web interface that can be found [here](http://dev-hipe.aneo.local:3000/).    

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
How to start with HIPE
==================

# Content
1. [Building HIPE](#building-hipe) 
	*  [Requirements](#requirements)
	* [Project Building](#project-building)
	* [Project Configuration](#project-configuration)
2. [Working with HIPE](#working-with-hipe)
	* [JSON Requests](#json-requests) 
		* [Syntax of a JSON request](#syntax-of-a-json-request) 
		* [Sending requests to HIPE](#sending-requests-to-hipe) 


# Building Hipe

## Requirements
HIPE can be built and used on a Windows or Linux platform.
Here is a summary list of all the primary third party programs and libraries needed. It is not required to build all the dependencies separately as they were all packed in an archive to make things easier.

-	Third party
	-	CMake 3.4.7+
-	Dependencies
	-	Nvidia CUDA 8.0
	-	OpenCV 3.1.0
	-	HDF5
	-	caffe
	-	dlib
	-	boost
-	Windows Specific
	-	Visual Studio 2015 (VC140 build tools)
-	Linux
	-	[TODO]

### CMake 3.4.7+
To build HIPE, the minimum required version of CMake is 3.4.7.

CMake can be [downloaded here]( https://cmake.org/download/)

### Nvidia CUDA 8.0
Some of the required dependencies (like OpenCV) were built using CUDA. It allows HIPE to use any Nidia GPU, if present, to use its power and speed up things.
Nvidia CUDA toolkit 8.0 is needed even if no GPU is present on the hardware. In that case, you are not forced to install the display driver nor the PhysX one. You can do so by selecting the advanced installation mode when prompted, and unchecking the _Driver_ and _Other components_ marks on the components to install list.

The toolkit can be downloaded on the [Nvidia CUDA dedicated website](https://developer.nvidia.com/cuda-toolkit-archive)

**Note:** The version 8.0 must be present on your system’s _PATH_, as HIPE's building configuration files will look for it here. If you already have multiple versions of the toolkit installed, or you don’t want it to be referenced in it,  you can also fill the ``CUDA_SDK_ROOT_DIR`` and ``CUDA_TOOLKIT_ROOT_DIR`` fields when the “Could not find CUDA” CMake error will occur in the building phase.

### Windows Specific
On Windows platforms, Visual Studio 2015 was used to compile the dependencies and is the main tool to work on HIPE. The project will _ONLY_ work with this version. At least, only when using the VC140 build tools.

If you don’t have those build tools or any Visual Studio version installed, you can find the free 2015 Community one in the [older downloads page](https://www.visualstudio.com/fr/vs/older-downloads/) on the Visual Studio website.

### Linux Specific
[TODO]

### Libraries and other dependencies
As mentioned before, HIPE uses multiple libraries. A package regrouping all of them can be found here : [TODO].

**Note:** The package can be extracted anywhere but it must keep its hierarchy.

## Project Building

### Windows
To build HIPE's Visual Studio project files, you must use CMake.

1.	Fill in the HIPE sources and build directories. You can chose the build directory you want, but remember that HIPE sources are located on the Hipe subdirectory.
2.	Click on _Configure_ and select the _Visual Studio build tools (Visual Studio 14 2015 Win64)_ build tools then confirm by clicking on the _Finish_ button. The configuration will start. If not click again on _Configure_
3.	An error (Could not find ``HIPE_EXTERNAL`` variable) will occur. You will have to fill the ``HIPE_EXTERNAL`` field with the path of the directory where the extracted HipeExternal archive is located. The expected directory is the one where the “win64” one is located.
4.	When the configuration is done, you must click again on _Configure_ to validate it then click on _Generate_ to create the Visual Studio project files. The files will be located on the selected build directory.

### Linux
[TODO]

## Project Configuration
A few configuration steps are also needed before HIPE can be used

### Windows
In the Visual Studio solution, the default start up project to set is _hype_server_

The compiled dependencies files must be referenced in your system folder, your system’s PATH or Visual Studio's debug one. You can also copy them next to the _hype_server.exe_ executable file.

Here is the list of the needed dependencies. The dependencies are all coming from the HipeExternal archive.

-	boost
-	OpenCV
-	FFmpeg
-	GStreamer
-	CUDA
-	Liblept
-	HDF5
-	Intel

If you want to use Visual Studio’s debug path, you can add them by completing the field _Environment_ of the _hype_server_ Debugging properties tab (right click on hype_server Properties->Debugging). The variable can contain multiple properties all separated by new lines.

Assuming you start with an empty PATH variable you will end up with a value similar to the following one:
```
“PATH=C:\workspace\libs\HipeExternal\win64\boost_1_62_0\lib64-msvc-14.0;C:\workspace\libs\HipeExternal\win64\opencv\x64\vc14\bin;C:\workspace\libs\HipeExternal\win64\intel64_win\bin;C:\workspace\libs\HipeExternal\win64\ffmpeg\bin;C:\workspace\libs\HipeExternal\win64\gstreamer\1.0\x86_64\bin;C:\workspace\libs\HipeExternal\win64\liblept\bin;C:\workspace\libs\HipeExternal\win64\hdf5\bin;C:\NVIDIA\CUDA\v8.0\Toolkit\bin;%PATH%”
```

**Note:** The whole PATH variable must be set **on the same line**!

**Note:** Working with this debug PATH overwrites the system’s one. To still use it you must add %PATH% at the end of the value.

### Linux
[TODO]

# Working with HIPE
## JSON Requests
HIPE is a http server.
The communication method we use to transfer data is to send them using the JSON syntax.
If you never worked with it, you can quickly learn the basics by reading its documentation on [the w3schools website](https://www.w3schools.com/js/js_json_intro.asp)

### Syntax of a JSON request
Here is a request as an example:
```json
{
	"name": "resize",
	"filters": [ {
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
	}],
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
* The scheduler symbolizing how the chain will be processed (``orchestrator`` filed)
* The data which will be processed by the chain (``data`` field)

#### Processing chain and its filters
The processing chain can be seen as a graph where each node is a different _filter_ which will “do something” on its input data. So basically, a _filter_ can represent an algorithm, like one which will alter an image , or another one which will find and identify a object in a video.

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

#### Scheduler
As said before, the orchestrator is the scheduler which will reorder the processing of the nodes, taking into account their dependencies and the available hardware.
For now, only the _DefaultScheduler_ model is available. The filters algorithms will be processed sequentially in their dependencies order.

#### Linking data and supported data types
HIPE can work with images, videos (or image sequences), as well as live data streams from hardware like video cameras.

Here is a table containing the primary data types and their corresponding identifier tags

|   TYPE	|   IDENTIFIER	|   NOTE	|
|---	|---	|---	|
|   image	|   ``IMGF``	|   path: "*image_path*"	|
|   video	|   ``VIDF``	|   path: "*video_path*"	|
|   webcam	|   ``VIDF``	|   path: "*id_cam"* (i.e. 0,1, ...)"	|
|   image sequence	|   ``SEQIMGD``	|   path: "*images_dir_path*"	|


To link some data to your request, you must add a ``data`` field. As shown in the example, the field is not part of the ``filters`` array.

The field must contain:

* The data type containing the identifier of the data type you want to use (``type`` field)
* The data path (``path`` field)

### Sending requests to HIPE
The tool we use to edit and send JSON requests to HIPE is Postman. It is not a prerequisite to use HIPE, so you can alternatively use any tool you want.
Assuming you want to use Postman, you can download it [here](https://www.getpostman.com/)
You will also find collections of tests requests in the ``/Hipe/tests/data/request/`` folder. They are in the Postman format but it internally uses text, so you can open them in any text editor you want to peek at their content.

#### How to use Postman
##### Import requests
In Postman, every request must be part of a collection. To import a collection, you must simply click on the _import_ button in the upper left corner of Postman’s interface. The collection will be shown in the left side bar. If the side bar is hidden you can show it by clicking on the first button _Show/Hide Sidebar_ next to the _import_ button.

##### Create and edit a request
In Postman, all the opened requests are shown in different tabs. By default, if no request is already opened, a new one will be shown and It is possible to directly edit it. But, if at least one request is opened, the default empty one will be hidden and you will need to click on the plus sign button at the right of the last opened request’s tab.
To be able to write the request, you must chose the ``POST`` mode instead of the ``GET`` mode. It will unlock the _Body_ tab. It is here that you will write the content of the request. You will also have to check the _raw_ button to enable text edit, then chose the _JSON (application/json)_ instead of _text_ in the orange drop down menu to enable syntax highlighting.

##### Save your request
You can save your request by clicking on the _Save_ button at the right of the request’s tab window. Remember that in Postman, every request must be linked to a collection. If no collection was already created, you will have to create a new one. You can do so by clicking on the _+ Create Collection_ button at the bottom of the opened save dialog.

##### Send a request
HIPE’s http server listen on port 9090.
When used in a debug environment, you will probably deploy the server locally. In that case, the address will be localhost. If deployed on a remote environment, you will have to input the corresponding IP address. A complete address will be similar to: ``localhost:9090/json``.
To send the request you will have to click on the blue _Send_ button. The server will then send its response when the processing will be done. The response is also in JSON format and will be shown just below the body of the sent request.

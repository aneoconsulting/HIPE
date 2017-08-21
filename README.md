How to start with HIPE
==================

#Building Hipe

##Requirements
HIPE can be built and used on a Windows or Linux platform.
Here is a summary list of all the primaries third party programs and libraries needed. It is not required to build all the dependencies separately as they were all packed in an archive to make things easier.

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

###CMake 3.4.7+
To build HIPE, the minimum required version of CMake is 3.4.7.
CMake can be [downloaded here]( https://cmake.org/download/)

###Nvidia CUDA 8.0
Some of the required dependencies (like OpenCV) were built using CUDA. It allows HIPE to use any Nidia GPU, if present, to use its power and speed up things.
Nvidia CUDA toolkit 8.0 is needed even if no GPU is present on the hardware. In that case, you are not forced to install the display driver nor the PhysX one. You can do so by selecting the advanced installation mode when prompted, then unchecking the _Driver_ and _Other components_ marks on the components list.
The toolkit can be downloaded on the [Nvidia CUDA dedicated website](https://developer.nvidia.com/cuda-toolkit-archive)

**Note:** The version 8.0 must be present on your system’s _PATH_, as the configuration files will look for it here. If you already have multiple versions of the toolkit installed, or you don’t want it to be referenced in it,  you can also fill the ``CUDA_SDK_ROOT_DIR`` and ``CUDA_TOOLKIT_ROOT_DIR`` fields when the “Could not find CUDA” CMake error will occur in the building phase.

###Windows Specific
On Windows platforms, Visual Studio 2015 was used to compile the dependencies and is the main tool to work on HIPE. The project will _ONLY_ work with this version. At least, only when using the VC140 build tools.
If you don’t have those build tools or any Visual Studio version installed, you can find the free 2015 Community one in the [older downloads page](https://www.visualstudio.com/fr/vs/older-downloads/) on the Visual Studio website.

###Linux Specific
[TODO]

###Libraries and other dependencies
As mentioned before, HIPE uses multiple libraries. A package regrouping all of them can be found here : [TODO].

**Note:** The package can be extracted anywhere but it must keep its hierarchy.

##Project Building

###Windows
To build HIPE's Visual Studio project files, you must use CMake.

1.	Fill in the HIPE sources and build directories. You can chose the build directory you want, but remember that HIPE sources are located on the Hipe subdirectory.
2.	Click on _Configure_ and select the _Visual Studio build tools (Visual Studio 14 2015 Win64)_ build tools then confirm by clicking on the _Finish_ button. The configuration will start. If not click again on _Configure_
3.	An error (Could not find ``HIPE_EXTERNAL`` variable) will occur. You will have to fill the ``HIPE_EXTERNAL`` field with the path of the directory where the extracted HipeExternal archive is located. The expected directory is the one where the “win64” one is located.
4.	When the configuration is done, you must click again on _Configure_ to validate it then click on _Generate_ to create the Visual Studio project files. The files will be located on the selected build directory.

##Project Configuration
A few configuration steps are also needed before HIPE can be used

###Windows
In the Visual Studio solution, the default start up project to set is _hype_server_

The compiled dependencies files must be referenced in your system, your system’s PATH or Visual Studio debug one. You can also copy them next to the _hype_server.exe_ executable file.

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
```“PATH=C:\workspace\libs\HipeExternal\win64\boost_1_62_0\lib64-msvc-14.0;C:\workspace\libs\HipeExternal\win64\opencv\x64\vc14\bin;C:\workspace\libs\HipeExternal\win64\intel64_win\bin;C:\workspace\libs\HipeExternal\win64\ffmpeg\bin;C:\workspace\libs\HipeExternal\win64\gstreamer\1.0\x86_64\bin;C:\workspace\libs\HipeExternal\win64\liblept\bin;C:\workspace\libs\HipeExternal\win64\hdf5\bin;C:\NVIDIA\CUDA\v8.0\Toolkit\bin;%PATH%”```

**Note:** Working with this debug PATH overwrites the system’s one. To still use it you must add %PATH% at the end of the value.

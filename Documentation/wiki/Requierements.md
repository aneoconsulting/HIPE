[0]: #content
[1]: #-dependencies-HIPE
[11]: #-requirements
[111]: #-cmake-311
[112]: #-nvidia-cuda-80
[113]: #-nvidia-cudnn
[114]: #-windows-platforms
[115]: #-linux-platforms
[116]: #-libraries-and-other-dependencies

[![License: AGPL v3](https://img.shields.io/badge/License-AGPL%20v3-blue.svg)](https://www.gnu.org/licenses/agpl-3.0)

How to start with HIPE
==================

# Content
1. [Dependencies HIPE][1]
	* [Requirements][11]


# [[~][0]] Dependencies-HIPE


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
	-	CMake 3.11
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

### [[..][11]] CMake 3.11
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

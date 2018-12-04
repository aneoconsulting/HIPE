[0]: #content
[1]: #-build-hipe
[11]: #-requirements
[12]: #-project-building
[121]: #-windows
[122]: #-linux
[13]: #-set-up-your-environment
[131]: #-windows-1
[132]: #-linux-1

How to start with HIPE
==================

# Content
1. [Build HIPE][1]
	* [Project Building][12]
		* [Windows][121]
		* [linux][122]
	* [Set up your environment][13]


# [[~][0]] Build Hipe

    
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
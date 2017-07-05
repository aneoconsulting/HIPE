#!/bin/bash

cd /home/ddubuc/hipe/Hipe/build/source/hype_server

/usr/bin/c++    -std=c++11 -Wall -std=c++11 -Wall   CMakeFiles/hype_server.dir/main.cpp.o  -o \
				../../target/hype_server \
				-lpthread \
				../../target/libs/libcore.a \
				../../target/libs/libfilter.a \
				../../target/libs/libhttp.a \
				../../target/libs/libjson.a \
				../../target/libs/liborchestrator.a \
				../../target/libs/libjson.a \
				../../target/libs/libfilter.a \
				/usr/lib/x86_64-linux-gnu/libopencv_videostab.so.2.4.9 /usr/lib/x86_64-linux-gnu/libopencv_ts.so.2.4.9 /usr/lib/x86_64-linux-gnu/libopencv_superres.so.2.4.9 /usr/lib/x86_64-linux-gnu/libopencv_stitching.so.2.4.9 /usr/lib/x86_64-linux-gnu/libopencv_ocl.so.2.4.9 /usr/lib/x86_64-linux-gnu/libopencv_gpu.so.2.4.9 /usr/lib/x86_64-linux-gnu/libopencv_photo.so.2.4.9 /usr/lib/x86_64-linux-gnu/libopencv_legacy.so.2.4.9 /usr/lib/x86_64-linux-gnu/libopencv_contrib.so.2.4.9 /usr/lib/x86_64-linux-gnu/libopencv_video.so.2.4.9 /usr/lib/x86_64-linux-gnu/libopencv_objdetect.so.2.4.9 /usr/lib/x86_64-linux-gnu/libopencv_ml.so.2.4.9 /usr/lib/x86_64-linux-gnu/libopencv_calib3d.so.2.4.9 /usr/lib/x86_64-linux-gnu/libopencv_features2d.so.2.4.9 /usr/lib/x86_64-linux-gnu/libopencv_highgui.so.2.4.9 /usr/lib/x86_64-linux-gnu/libopencv_imgproc.so.2.4.9 /usr/lib/x86_64-linux-gnu/libopencv_flann.so.2.4.9 /usr/lib/x86_64-linux-gnu/libopencv_core.so.2.4.9 \
				../../target/libs/libcore.a \
				/hpc/apps/boost_1_62_0/stage/lib/libboost_system.a \
				/hpc/apps/boost_1_62_0/stage/lib/libboost_thread.a \
				/hpc/apps/boost_1_62_0/stage/lib/libboost_filesystem.a \
				/hpc/apps/boost_1_62_0/stage/lib/libboost_date_time.a \
				/hpc/apps/boost_1_62_0/stage/lib/libboost_regex.a \
				/hpc/apps/boost_1_62_0/stage/lib/libboost_chrono.a \
				/hpc/apps/boost_1_62_0/stage/lib/libboost_atomic.a \
				/hpc/apps/boost_1_62_0/stage/lib/libboost_log_setup.a -lpthread
				/hpc/apps/boost_1_62_0/stage/lib/libboost_log.a \

					/usr/bin/c++    -std=c++11 -Wall -std=c++11 -Wall   CMakeFiles/hype_server.dir/main.cpp.o  -o ../../target/hype_server \
					/hpc/apps/boost_1_62_0/stage/lib/libboost_system.a /hpc/apps/boost_1_62_0/stage/lib/libboost_thread.a \
					/hpc/apps/boost_1_62_0/stage/lib/libboost_filesystem.a \
					/hpc/apps/boost_1_62_0/stage/lib/libboost_date_time.a \
					/hpc/apps/boost_1_62_0/stage/lib/libboost_regex.a \
					/hpc/apps/boost_1_62_0/stage/lib/libboost_log.a \
					/hpc/apps/boost_1_62_0/stage/lib/libboost_chrono.a \
					/hpc/apps/boost_1_62_0/stage/lib/libboost_atomic.a \
					/hpc/apps/boost_1_62_0/stage/lib/libboost_log_setup.a \
					-lpthread ../../target/libs/libcore.a \
					../../target/libs/libfilter.a \
					../../target/libs/libhttp.a \
					../../target/libs/libjson.a \
					../../target/libs/liborchestrator.a \
					../../target/libs/libjson.a \
					../../target/libs/libfilter.a \
					/usr/lib/x86_64-linux-gnu/libopencv_videostab.so.2.4.9 /usr/lib/x86_64-linux-gnu/libopencv_ts.so.2.4.9 /usr/lib/x86_64-linux-gnu/libopencv_superres.so.2.4.9 /usr/lib/x86_64-linux-gnu/libopencv_stitching.so.2.4.9 /usr/lib/x86_64-linux-gnu/libopencv_ocl.so.2.4.9 /usr/lib/x86_64-linux-gnu/libopencv_gpu.so.2.4.9 /usr/lib/x86_64-linux-gnu/libopencv_photo.so.2.4.9 /usr/lib/x86_64-linux-gnu/libopencv_legacy.so.2.4.9 /usr/lib/x86_64-linux-gnu/libopencv_contrib.so.2.4.9 /usr/lib/x86_64-linux-gnu/libopencv_video.so.2.4.9 /usr/lib/x86_64-linux-gnu/libopencv_objdetect.so.2.4.9 /usr/lib/x86_64-linux-gnu/libopencv_ml.so.2.4.9 /usr/lib/x86_64-linux-gnu/libopencv_calib3d.so.2.4.9 /usr/lib/x86_64-linux-gnu/libopencv_features2d.so.2.4.9 /usr/lib/x86_64-linux-gnu/libopencv_highgui.so.2.4.9 /usr/lib/x86_64-linux-gnu/libopencv_imgproc.so.2.4.9 /usr/lib/x86_64-linux-gnu/libopencv_flann.so.2.4.9 /usr/lib/x86_64-linux-gnu/libopencv_core.so.2.4.9 ../../target/libs/libcore.a \
					/hpc/apps/boost_1_62_0/stage/lib/libboost_system.a \
					/hpc/apps/boost_1_62_0/stage/lib/libboost_thread.a \
					/hpc/apps/boost_1_62_0/stage/lib/libboost_filesystem.a \
					/hpc/apps/boost_1_62_0/stage/lib/libboost_date_time.a \
					/hpc/apps/boost_1_62_0/stage/lib/libboost_regex.a \
					/hpc/apps/boost_1_62_0/stage/lib/libboost_log.a \
					/hpc/apps/boost_1_62_0/stage/lib/libboost_chrono.a \
					/hpc/apps/boost_1_62_0/stage/lib/libboost_atomic.a \
					/hpc/apps/boost_1_62_0/stage/lib/libboost_log_setup.a \
					-lpthread

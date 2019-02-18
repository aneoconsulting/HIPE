cmake_minimum_required (VERSION 3.7.1)


project (Hipe-module-aneo-vision LANGUAGES CXX CUDA)

option(WITH_ANEO_VISION_FORMATION "Use Aneo Formation computer vision" OFF)
option(WITH_ANEO_VISION_FULL "Use Aneo full computer vision" ON)

if (WITH_ANEO_VISION_FORMATION)
	add_subdirectory(formation)
endif()

if (WITH_ANEO_VISION_FULL)
	add_subdirectory(full)
endif()
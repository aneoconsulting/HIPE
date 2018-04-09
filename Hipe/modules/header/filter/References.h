#pragma once 
// Algorithm header to reference 
#include <filter/algos/AlgoExample_1.h>
#include <filter/algos/FilePatternFilter.h>
#include <filter/algos/OutputImage.h>
#include <filter/algos/OutputRawDataFilter.h>
#include <filter/algos/PPOC.h>
#include <filter/algos/PythonFilter.h>
#include <filter/algos/ResultFilter.h>
#include <filter/algos/RootFilter.h>
#include <filter/algos/StreamResultFilter.h>
#include <filter/algos/agegender\AgeClassification.h>
#include <filter/algos/agegender\AgeGender.h>
#include <filter/algos/agegender\GenderClassification.h>
#include <filter/algos/detection\Akaze.h>
#include <filter/algos/detection\DetectCirclesFilter.h>
#include <filter/algos/detection\FaceDetection.h>
#include <filter/algos/detection\FaceLandmark.h>
#include <filter/algos/detection\FingerPrint.h>
#include <filter/algos/detection\FingerPrintMinutia.h>
#include <filter/algos/detection\FingerprintMinutiae.h>
#include <filter/algos/detection\HOGLiveDetector.h>
#include <filter/algos/detection\HOGLiveTrainer.h>
#include <filter/algos/detection\Latch.h>
#include <filter/algos/detection\LatchDebug.h>
#include <filter/algos/detection\ObjectRecognitionYolo.h>
#include <filter/algos/detection\Surf.h>
#include <filter/algos/extraction\ExclusionZoneMaskFilter.h>
#include <filter/algos/extraction\ExctractSubImage.h>
#include <filter/algos/extraction\ExtractPatternCrops.h>
#include <filter/algos/extraction\ExtractPatternSource.h>
#include <filter/algos/extraction\ExtractShape.h>
#include <filter/algos/extraction\HideCircles.h>
#include <filter/algos/IDPlate\IDPlateCropper.h>
#include <filter/algos/IDPlate\IDPlateIdentifier.h>
#include <filter/algos/IDPlate\IDPlateRectifier.h>
#include <filter/algos/IDPlate\IDPlateTools.h>
#include <filter/algos/preprocessing\AverageColor.h>
#include <filter/algos/preprocessing\BilateralFilter.h>
#include <filter/algos/preprocessing\Binary.h>
#include <filter/algos/preprocessing\BinaryAdaptive.h>
#include <filter/algos/preprocessing\Blur.h>
#include <filter/algos/preprocessing\Brightness.h>
#include <filter/algos/preprocessing\Canny.h>
#include <filter/algos/preprocessing\ClosestColor.h>
#include <filter/algos/preprocessing\ComputeRegularGrid.h>
#include <filter/algos/preprocessing\Contrast.h>
#include <filter/algos/preprocessing\Crop.h>
#include <filter/algos/preprocessing\Dilate.h>
#include <filter/algos/preprocessing\Equalize.h>
#include <filter/algos/preprocessing\EqualizeAdaptive.h>
#include <filter/algos/preprocessing\Erode.h>
#include <filter/algos/preprocessing\Gaussian.h>
#include <filter/algos/preprocessing\Grayscale.h>
#include <filter/algos/preprocessing\Homography.h>
#include <filter/algos/preprocessing\Invert.h>
#include <filter/algos/preprocessing\Kmeans.h>
#include <filter/algos/preprocessing\Median.h>
#include <filter/algos/preprocessing\OrderCircles.h>
#include <filter/algos/preprocessing\OverlayFilter.h>
#include <filter/algos/preprocessing\OverlayMatFilter.h>
#include <filter/algos/preprocessing\ReshapeImage.h>
#include <filter/algos/preprocessing\Resize.h>
#include <filter/algos/preprocessing\ResizePattern.h>
#include <filter/algos/preprocessing\RotateImage.h>
#include <filter/algos/show\ConcatToShow.h>
#include <filter/algos/show\Cropper.h>
#include <filter/algos/show\DrawCircles.h>
#include <filter/algos/show\ShowImage.h>
#include <filter/algos/show\ShowVideo.h>
#include <filter/algos/streaming\DelegateStreamingFilter.h>
#include <filter/algos/streaming\EncodeDataFilter.h>
#include <filter/datasource/DirectoryImgDataSource.h>
#include <filter/datasource/DirPatternDataSource.h>
#include <filter/datasource/ImageDataSource.h>
#include <filter/datasource/VideoDataSource.h>

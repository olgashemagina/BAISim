find_package(PkgConfig)
pkg_check_modules(TINYXML REQUIRED IMPORTED_TARGET tinyxml)
pkg_check_modules(UUID REQUIRED IMPORTED_TARGET uuid)

add_library(awplflib STATIC
	src/LFAttrFilter.cpp
	src/LFAvgFeature.cpp
	src/LFBuffer.cpp
	src/LFBuilder.cpp
	src/LFCascade.cpp
	src/LFCluster.cpp
	src/LFColorDetector.cpp
	src/LFCommon.cpp
	src/lfcsboost.cpp
	src/LFCSFeature.cpp
	src/LFDatabase.cpp
	src/LFDetectedItem.cpp
	src/LFDetector.cpp
	src/LFDistance.cpp
	src/LFEngine.cpp
	src/LFFeatureImpl.cpp
	src/LFFeatures.cpp
	src/LFFileUtils.cpp
	src/LFGeometry.cpp
	src/LFHistogramm.cpp
	src/LFImage.cpp
	src/LFImageProcessor.cpp
	src/LFLabColor.cpp
	src/LFLandmarks.cpp
	src/LFMotion.cpp
	src/LFParameter.cpp
	src/LFPredictors.cpp
	src/LFSample.cpp
	src/LFScanners.cpp
	src/LFSemantic.cpp
	src/LFStrong.cpp
	src/LFStrongImpl.cpp
	src/LFThresholdProc.cpp
	src/LFUtils.cpp
	src/LFVector.cpp
	src/LFWeak.cpp
	src/LFWeakImpl.cpp
	src/LFZones.cpp
	include/LFAttrFilter.h
	include/LFAvgFeature.h
	include/LFBuffer.h
	include/LFBuilder.h
	include/LFCluster.h
	include/LFColorDetector.h
	include/LFCore.h
	include/LFCSBoost.h
	include/LFCSFeature.h
	include/LFDatabase.h
	include/LFDetector.h
	include/LFDistance.h
	include/LFEngine.h
	include/LFFeatures.h
	include/LFFileUtils.h
	include/LFGeometry.h
	include/_LF.h
	include/LF.h
	include/LFHistogramm.h
	include/LFImageProcessor.h
	include/LFInterface.h
	include/LFLabColor.h
	include/LFLandmarks.h
	include/LFMCDetector.h
	include/LFMotion.h
	include/LFParameter.h
	include/LFPredictors.h
	include/LFSample.h
	include/LFScanners.h
	include/LFSemantic.h
	include/LFStrong.h
	include/LFThresholdProc.h
	include/LFUtils.h
	include/LFVector.h
	include/LFWeak.h
	include/LFZone.h
	)

set_target_properties(awplflib PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(awplflib 
	PUBLIC $(CMAKE_CURRENT_SOURCE_DIR)/include
	)

target_link_libraries(awplflib
	awpipl2
	PkgConfig::TINYXML
	PkgConfig::UUID
	)

add_subdirectory(utils)



add_library(awpipl2 STATIC
	src/awpApprox.c
	src/awpBaseAnalysis.c
	src/awpBlur.c
	src/awpchannels.c
	src/_awpColor.c
	src/awpColor.c
	src/awpcommon.c
	src/awpcontour.c
	src/awpcontrast.c
	src/awpconvert.c
	src/awpcopypaste.c
	src/awpDistance.c
	src/awpdraw.c
	src/awpEdge.c
	src/awpfft.c
	src/awpframeprocess.c
	src/awpGauss.c
	src/awpgeometry.c
	src/awpget.c
	src/awpintegralfeatures.c
	src/awpInterpolation.c
	src/_awpio.c
	src/awpio.c
	src/awplbp.c
	src/awpmain.c
	src/awpmaskconvolution.c
	src/awpMedian.c
	src/awpNorm.c
	src/awpopencv.c
	src/awppoints.c
	src/awpstroke.c
	src/awpthreshold.c
	include/awperror.h
	include/awpipl.h
	include/config.h
	include/_include/_awpcolor.h
	include/_include/_awpdetect.h
	include/_include/_awpdraw.h
	include/_include/_awpHSL.h
	include/_include/_awpHSV.h
	include/_include/awpInterpolation.h
	include/_include/_awpio.h
	include/_include/_awpipl.h
	include/_include/awpLBPUnit.h
	)

set_target_properties(awpipl2 PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(awpipl2 
	PRIVATE $(CMAKE_CURRENT_SOURCE_DIR)/include/_include
	PUBLIC $(CMAKE_CURRENT_SOURCE_DIR)/include
	)


add_library(FVLCore STATIC
	src/awpeigenlib.cpp
	src/fvlcore.cpp
	src/awpeigenlib.h
	src/fvlcore.h
	)

set_target_properties(FVLCore PROPERTIES POSITION_INDEPENDENT_CODE ON)


target_link_libraries(FVLCore
	awpipl2
	)

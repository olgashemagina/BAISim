# Semantic Image Descriptor

Requires the following static libraries for compilation (they should be added in C++ Builder project):
   * awpipl2b.lib
   * awplflibb.lib
   * JPEGLIB.lib
   * tinyxml.lib

Usage:
```
<semantic_img_descriptor.exe> --det=<path_to_detector1> [--det=<path_to_detector2> ...] --db_folder=<path_to_img_directory>
	--det		Path to XML with detector (TLFDetectEngine)
	--db_path	Path to directory with images
Each detector will be executed on each image file in the directory and detected objects will be written to XML file
(which is created in the same directory with images).
```

For example:
```
semantic_img_descriptor.exe --det=rnum_cs.xml --det=rnum_haar.xml --det=rnum_lbp.xml --db_folder=imgs_to_test
```

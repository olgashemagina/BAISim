/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                  Locate Framework  Computer Vision Library
//
// Copyright (C) 2004-2018, NN-Videolab.net, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the NN-Videolab.net or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//      Locate Framework  (LF) Computer Vision Library.
//		File: LFFileUtils.h
//      CopyRight 2004-2020 (c) NN-Videolab.net
//M*/

#ifndef _lf_file_utils_h_
#define _lf_file_utils_h_ 


/** \defgroup LFFileUtils
*	Implementation of file system related routines 
*   @{
*/

#include <string>
#include <filesystem>

#include "LFCore.h"


extern "C"
{
#ifdef WIN32
	#include <Rpc.h>
	#define c_separator  "\\"
	#define w_separator  L"\\"
#else
	#include <uuid/uuid.h>
	typedef uuid_t UUID;
	#define c_separator  "/"
#endif
}

#ifdef WIN32
	const UUID c_uuidZero = { 0, 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0 } };
#else
	const UUID c_uuidZero = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
#endif

#ifdef WIN32
#define LF_UUID_CREATE(v)  UuidCreate(&v);
#define LF_NULL_UUID_CREATE(v) memset(&v, 0, sizeof(UUID));
#else
#define LF_UUID_CREATE(v)  uuid_generate(v);
#define LF_NULL_UUID_CREATE(v) memset(v, 0, sizeof(UUID));
#endif

std::wstring LFUtf8ConvertToUnicode(const std::string& str);
//functions to work with file system
bool LFGetDirFiles(const std::filesystem::path& dirPath, std::vector<std::filesystem::path>& filePaths);
bool LFIsImageFile(const char* fileName);
bool LFIsVideoFile(const char* fileName);
// functions to convert data
std::string LFIntToStr(int value);
std::string LFGUIDToString(UUID* id);
void  LFStringToUUID(const TLFString& strUUID, UUID* id);
// functions to work with time
unsigned long LFGetTickCount();
/** @} */ /*  end of LFFileUtils group */
#endif //_lf_file_utils_h_ 

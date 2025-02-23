#include "LFFileUtils.h"

#include <algorithm>
#include <filesystem>
#include <sys/stat.h>
//#if defined(WIN32) || defined(_WIN64)
#include <io.h>
//#include <direct.h>
#include <string>
//#else
//#include <dirent.h>
//#endif

#include "stdio.h"
#include "LFFileUtils.h"
//#ifndef __BCPLUSPLUS__
//#include <sys/stat.h>
//#endif
namespace fs = std::filesystem;

std::string LFGetFilePath(const std::string& strPath)
{
    //const std::string c = c_separator;
	//int len = strPath.find_last_of(c);
	//return strPath.substr(0, len);
    fs::path p(strPath);
    return p.parent_path().generic_string();
}

std::string LFGetFileExt(const std::string&  strFileName)
{
    fs::path p(strFileName);
    return p.extension().generic_string();
//	int len = strFileName.find_last_of('.');
//	if (len > 0)
//		return strFileName.substr(len, strFileName.length() - len);
//	else
//		return "";
}
std::string LFGetFileName(const std::string&  strFileName)
{
    fs::path p(strFileName);
   	return p.stem().generic_string();
//	int len = strFileName.find_last_of('\\');
//	std::string str = strFileName.substr(len, strFileName.length() - len);
//	len = str.find_last_of('.');
//	return str.substr(0, len);

}
std::string LFChangeFileExt(const std::string& strFileName, std::string strExt)
{
    fs::path p(strFileName);
    return p.replace_extension(strExt).generic_string();
//	int len = strFileName.find_last_of('.');
//	return strFileName.substr(0, len) + strExt;
}
std::string LFMakeFileName(const std::string& strPath, std::string strName, std::string strExt)
{
	if (strName.length() == 0)
		return "";

	std::string path = strPath;

	if (path.find_last_of('\\') != path.length() - 1)
		path += c_separator;
	if (strExt.find_first_of('.') != 0)
	{
		std::string tmp = ".";
		tmp += strExt;
		strExt = tmp;
	}
	return path + strName + strExt;
}
//#endif

//std::string LFUnicodeConvertToUtf8(const std::wstring& wstr)
//{
//	if (wstr.empty()) return std::string();
//	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
//	std::string strTo(size_needed, 0);
//	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
//	return strTo;
//}
//
//std::wstring LFUtf8ConvertToUnicode(const std::string& str)
//{
//	const int wchars_num = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
//	std::wstring ret;
//	ret.resize(wchars_num - 1);
//	//wchar_t* wstr = new wchar_t[wchars_num];
//	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, const_cast<wchar_t*>(ret.data()), wchars_num);
//	//delete[] wstr;
//	return ret;
//}
//
//std::wstring LFConcatPath(const std::wstring& parentPath, const std::wstring& path)
//{
//	if (parentPath.find_last_of(w_separator) == parentPath.size() - 1)
//		return parentPath + path;
//	return parentPath + w_separator + path;
//}


bool LFFileExists(const std::string& strFileName)
{
    fs::file_status s = fs::status(strFileName);
    if (fs::is_regular_file(s))
        return 1;
    return 0;
//	FILE *file;
//	if ((file = _wfopen(LFUtf8ConvertToUnicode(strFileName).c_str(), L"r")) != NULL)
//	{
//		fclose(file);
//		return 1;
//	}
//	return 0;
}

std::string LFIntToStr(int value)
{
	char buf[32];
	sprintf(buf, "%i", value);
	std::string str = buf;
	return str;
}
//functions to work with file system 
bool LFCreateDir(const char* lpPath)
{
    return fs::create_directory(lpPath);
//#if defined(WIN32) || defined(_WIN64)
//	if (_mkdir(lpPath) == 0)
//		return true;
//	return false;
//#else
//    mode_t mode = 0755;
//    if (mkdir(lpPath, mode) == 0)
//		return true;
//	return false;
//#endif
}

bool LFCreateDirs(const char* lpPath)
{
	try {
		fs::create_directories(lpPath);
		return true;
	}
	catch (...) {
		return false;
	}
}

bool LFDirExist(const char* lpPath)
{
    fs::file_status s = fs::status(lpPath);
    if (fs::is_directory(s))
        return 1;
    return 0;
//#if defined(WIN32) || defined(_WIN64)
//	if (_mkdir(lpPath) == 0)
//	{
//		_rmdir(lpPath);
//		return false;
//	}
//	else
//	{
//		if (errno == EEXIST)
//			return true;
//		else
//			return false;
//	}
//#else
//    mode_t mode = 0755;
//	if (mkdir(lpPath, mode) == 0)
//	{
//		rmdir(lpPath);
//		return false;
//	}
//	else
//	{
//		if (errno == EEXIST)
//			return true;
//		else
//			return false;
//	}
//#endif
}
/**
* @brief clear all files in the direcrory 
*/
bool LFRemoveDir(const char* lpPath)
{
//#if defined(WIN32) || defined(_WIN64)
	const fs::path cur_dir{lpPath};

	if(fs::is_empty(cur_dir))
		return false;
	else
	{
        fs::remove_all(cur_dir);
		return true;
	}

//#ifdef _USE_UNICODE_VS_
//	std::wstring dirPath = LFUtf8ConvertToUnicode(lpPath);
//	std::wstring strPath = LFConcatPath(dirPath, L"*.*");
//
//	_wfinddata_t filesInfo;
//	intptr_t handle = 0;
//
//	if ((handle = _wfindfirst(strPath.c_str(), &filesInfo)) != -1)
//	{
//		do
//		{
//			std::wstring strImageName = LFConcatPath(dirPath, filesInfo.name);
//			DeleteFileW(strImageName.c_str());
//		} while (!_wfindnext(handle, &filesInfo));
//	}
//	_findclose(handle);
//#else
//	TLFString strPath = lpPath;
//	strPath += "\\*.*";
//
//	_finddata_t filesInfo;
//	intptr_t handle = 0;
//
//	if ((handle = _findfirst((char*)strPath.c_str(), &filesInfo)) != -1)
//	{
//		do
//		{
//			TLFString s = lpPath;
//			TLFString strImageName = s + "\\" + filesInfo.name;
//			DeleteFileA(strImageName.c_str());
//		} while (!_findnext(handle, &filesInfo));
//	}
//	_findclose(handle);
//#endif
//	return true;
//#else
//	const char *com = "exec rm -r ";
//	const char *end = "/*";
//
//	int bufferSize = strlen(com) + strlen(lpPath) + strlen(end) + 1;
//	char* resCom = new char[bufferSize];
//	strcpy(resCom, com);
//	strcat(resCom, lpPath);
//	strcat(resCom, end);
//
//	int status = system(resCom);
//	delete[] resCom;
//	if (status < 0)
//		return false;
//	else return true;
//#endif
}

bool LFRemoveFilesInDir(const char* lpPath)
{
	const fs::path cur_dir{ lpPath };

	if (fs::is_empty(cur_dir))
		return false;
	else
	{
		for (const auto& f : fs::directory_iterator(cur_dir))
		{
			if (fs::is_regular_file(fs::status(f)))
			{
				std::string tmp = f.path().generic_string();
				fs::remove(tmp);
			}
		}
		return true;
	}
}

#if defined(WIN32) || defined(_WIN64)
/* 128 bit GUID to human-readable string */
static char * guid_to_str(UUID* id, char * out) {
	unsigned int i;
	char * ret = out;
	out += sprintf(out, "%.8lX-%.4hX-%.4hX-", id->Data1, id->Data2, id->Data3);
	for (i = 0; i < sizeof(id->Data4); ++i) {
		out += sprintf(out, "%.2hhX", id->Data4[i]);
		if (i == 1) *(out++) = '-';
	}
	return ret;
}



static GUID StringToGuid(const std::string& str)
{
	UUID guid;
	sscanf(str.c_str(),
		"{%8x-%4hx-%4hx-%2hhx%2hhx-%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx}",
		&guid.Data1, &guid.Data2, &guid.Data3,
		&guid.Data4[0], &guid.Data4[1], &guid.Data4[2], &guid.Data4[3],
		&guid.Data4[4], &guid.Data4[5], &guid.Data4[6], &guid.Data4[7]);

	return guid;
}

void  LFStringToUUID(const TLFString& strUUID, UUID* id)
{
	try
	{
		UUID guid = StringToGuid(strUUID);
		memcpy(id, &guid, sizeof(UUID));
	}
	catch (...)
	{
		LF_NULL_UUID_CREATE(id);
	}
}
#endif

std::string LFGUIDToString(UUID* id)
{
	std::string result;
	char uuid_buf[130];

#if defined(WIN32) || defined(_WIN64)
	guid_to_str(id, uuid_buf);
#else
	uuid_unparse(*id, uuid_buf);
#endif
	result = uuid_buf;
	return result;
}

unsigned long LFGetTickCount()
{
#if defined(WIN32) || defined(_WIN64)
	return GetTickCount();
#else
    struct timespec ts;
    unsigned theTick = 0U;
    clock_gettime( CLOCK_REALTIME, &ts );
    theTick  = ts.tv_nsec / 1000000;
    theTick += ts.tv_sec * 1000;
    return theTick;
#endif
}


//#if defined(WIN32) || defined(_WIN64)

static bool _LFGetDirNamesWindows(const std::string& lpDir, TLFStrings& names)
{

	//const std::wstring dirPath = LFUtf8ConvertToUnicode(lpDir);
	//std::wstring strPath = LFConcatPath(dirPath, L"*.*");
	const fs::path cur_dir{lpDir};
	
	if(fs::is_empty(cur_dir))
		return false;
	else
	{
		for (const auto &f : fs::directory_iterator(cur_dir))
		{
			if (fs::is_regular_file(fs::status(f)))
			{
				std::string tmp = f.path().generic_string();
				names.push_back(tmp);
			}
		}
		return true;
	}
/*	_wfinddata_t filesInfo;

#ifdef _USE_UNICODE_VS_
	const std::wstring dirPath = LFUtf8ConvertToUnicode(lpDir);
	std::wstring strPath = LFConcatPath(dirPath, L"*.*");
	_wfinddata_t filesInfo;

	intptr_t handle = 0;

	if ((handle = _wfindfirst(const_cast<wchar_t*>(strPath.c_str()), &filesInfo)) != -1)
	{
		do
		{
			std::string name = LFUnicodeConvertToUtf8(LFConcatPath(dirPath, filesInfo.name));
			names.push_back(name);

		} while (!_wfindnext(handle, &filesInfo));
		_findclose(handle);
	}
	else
		return false;


	return true;
=======
#else
	_finddata_t filesInfo;
	intptr_t handle = 0;
	std::string path = lpDir;
	path += c_separator;
	if ((handle = _findfirst((char*)((path + "*.*").c_str()), &filesInfo)) != -1)
	{
		do
		{
			std::string name = path + filesInfo.name;
			names.push_back(name);

		} while (!_findnext(handle, &filesInfo));
		_findclose(handle);
	}
	else
		return false;
#endif
	
	return true;*/

}
//#else
//static bool _LFGetDirNamesLinux(const char* lpDir, TLFStrings& names)
//{
//	//printf("enter _LFGetDirNamesLinux \n");
//    DIR *dir;
//    struct dirent *entry;
//	std::string path = lpDir;
//	path += c_separator;
//    dir = opendir(lpDir);
//    if (!dir) {
//        printf("cannot open dir %s \n", path.c_str());
//        return false;
//    };
//
//    while ( (entry = readdir(dir)) != NULL) {
// 			string name = path + entry->d_name;
//			names.push_back(name);
//			//printf("%s\n", name.c_str());
//    };
//
//    closedir(dir);
//
//	return true;
//}
//#endif

bool LFGetDirFiles(const std::string& lpDir, TLFStrings& names)
{
	names.clear();
//#if defined(WIN32) || defined(_WIN64)
	return _LFGetDirNamesWindows(lpDir, names);
//#else
//	return _LFGetDirNamesLinux(lpDir, names);
//#endif
}

bool LFDeleteFile(const char* lpName)
{
    return fs::remove(lpName) == 0;
//  int res = _wremove(LFUtf8ConvertToUnicode(lpName).c_str());
//	return res == 0;
}

bool LFIsImageFile(const char* fileName)
{
	std::string strExt = LFGetFileExt(fileName);
	std::transform(strExt.begin(), strExt.end(), strExt.begin(), ::tolower);

	if (strExt == ".awp")
		return true;
	if (strExt == ".jpg")
		return true;
	if (strExt == ".jpeg")
		return true;

	return false;
}
bool LFIsVideoFile(const char* fileName)
{
	std::string strExt = LFGetFileExt(fileName);
	std::transform(strExt.begin(), strExt.end(), strExt.begin(), ::tolower);

	if (strExt == ".avi")
		return true;
	if (strExt == ".mp4")
		return true;

	return false;
}

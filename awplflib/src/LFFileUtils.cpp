#include "_LF.h"
#include "stdio.h"
#ifndef __BCPLUSPLUS__
#include <sys/stat.h> 
#ifndef WIN32
	#include <unistd.h>
#endif 

std::string LFGetFilePath(const std::string& strPath)
{
    const std::string c = c_separator;
	int len = strPath.find_last_of(c);
	return strPath.substr(0, len);
}

std::string LFGetFileExt(const std::string&  strFileName)
{
	int len = strFileName.find_last_of('.');
	if (len > 0)
		return strFileName.substr(len, strFileName.length() - len);
	else
		return "";
}
std::string LFGetFileName(const std::string&  strFileName)
{
	int len = strFileName.find_last_of('\\');
	std::string str = strFileName.substr(len, strFileName.length() - len);
	len = str.find_last_of('.');
	return str.substr(0, len);

}
std::string LFChangeFileExt(std::string& strFileName, std::string strExt)
{
	int len = strFileName.find_last_of('.');
	return strFileName.substr(0, len) + strExt;
}
std::string LFMakeFileName(std::string& strPath, std::string strName, std::string strExt)
{
	if (strName.length() == 0)
		return "";
	if (strPath.find_last_of('\\') != strPath.length() - 1)
		strPath += c_separator;
	if (strExt.find_first_of('.') != 0)
	{
		std::string tmp = ".";
		tmp += strExt;
		strExt = tmp;
	}
	return strPath + strName + strExt;
}
#endif


bool LFFileExists(const std::string& strFileName)
{
	FILE *file;
	if ((file = fopen(strFileName.c_str(), "r")) != NULL)
	{
		fclose(file);
		return 1;
	}
	return 0;
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
#ifdef WIN32
	if (_mkdir(lpPath) == 0)
		return true;
	return false;
#else
    mode_t mode = 0755;
    if (mkdir(lpPath, mode) == 0)
		return true;
	return false;
#endif
}
bool LFDirExist(const char* lpPath)
{
#ifdef WIN32
	if (_mkdir(lpPath) == 0)
	{
		_rmdir(lpPath);
		return false;
	}
	else
	{
		if (errno == EEXIST)
			return true;
		else
			return false;
	}
#else
    mode_t mode = 0755;
	if (mkdir(lpPath, mode) == 0)
	{
		rmdir(lpPath);
		return false;
	}
	else
	{
		if (errno == EEXIST)
			return true;
		else
			return false;
	}
#endif
}
/**
* @brief clear all files in the direcrory 
*/
bool LFRemoveDir(const char* lpPath)
{
#ifdef WIN32
	TLFString strPath = lpPath;
	strPath += "\\*.*";

	_finddata_t filesInfo;
	intptr_t handle = 0;

	if ((handle = _findfirst((char*)strPath.c_str(), &filesInfo)) != -1)
	{
		do
		{
			TLFString s = lpPath;
			TLFString strImageName = s + "\\" + filesInfo.name;
			DeleteFileA(strImageName.c_str());
		} while (!_findnext(handle, &filesInfo));
	}
	_findclose(handle);
	return true;
#else
	const char *com = "exec rm -r ";
	const char *end = "/*";

	int bufferSize = strlen(com) + strlen(lpPath) + strlen(end) + 1;
	char* resCom = new char[bufferSize];
	strcpy(resCom, com);
	strcat(resCom, lpPath);
	strcat(resCom, end);

	int status = system(resCom);
	delete[] resCom;
	if (status < 0)
		return false;
	else return true;
#endif
}

#ifdef WIN32
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

#ifdef WIN32
	guid_to_str(id, uuid_buf);
#else
	uuid_unparse(*id, uuid_buf);
#endif
	result = uuid_buf;
	return result;
}

unsigned long LFGetTickCount()
{
#ifdef WIN32
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


#ifdef WIN32 
static bool _LFGetDirNamesWindows(const char* lpDir, TLFStrings& names)
{
	_finddata_t filesInfo;
	intptr_t handle = 0;
	std::string path = lpDir;
	path += c_separator;
	if ((handle = _findfirst((char*)((path + "*.*").c_str()), &filesInfo)) != -1)
	{
		do
		{
			string name = path + filesInfo.name;
			names.push_back(name);

		} while (!_findnext(handle, &filesInfo));
		_findclose(handle);
	}
	else
		return false;
	
	return true;
}
#else
static bool _LFGetDirNamesLinux(const char* lpDir, TLFStrings& names)
{
    //printf("enter _LFGetDirNamesLinux \n");
    DIR *dir;
    struct dirent *entry;
	std::string path = lpDir;
	path += c_separator;
    dir = opendir(lpDir);
    if (!dir) {
        printf("cannot open dir %s \n", path.c_str());
        return false;
    };

    while ( (entry = readdir(dir)) != NULL) {
 			string name = path + entry->d_name;
			names.push_back(name);
			//printf("%s\n", name.c_str());
    };

    closedir(dir);
	
	return true;
}
#endif

bool LFGetDirFiles(const char* lpDir, TLFStrings& names)
{
	names.clear();
#ifdef WIN32
	return _LFGetDirNamesWindows(lpDir, names);
#else
	return _LFGetDirNamesLinux(lpDir, names);
#endif
}

bool LFDeleteFile(const char* lpName)
{
  int res = remove(lpName);
  return res == 0;
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

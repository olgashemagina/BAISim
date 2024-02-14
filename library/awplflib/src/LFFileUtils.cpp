#include "LFFileUtils.h"

#include <algorithm>
#include "utils.h"

#ifdef WIN32
#include <io.h>
#include <direct.h>
#include <string>
#else
#include <dirent.h>
#endif

#include "stdio.h"
#ifndef __BCPLUSPLUS__
#include <sys/stat.h> 
#ifndef WIN32
	#include <unistd.h>
#endif 

#endif

std::wstring LFUtf8ConvertToUnicode(const std::string& str)
{
	const int wchars_num = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
	std::wstring ret;
	ret.resize(wchars_num - 1);
	//wchar_t* wstr = new wchar_t[wchars_num];
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, const_cast<wchar_t*>(ret.data()), wchars_num);
	//delete[] wstr;
	return ret;
}



std::string LFIntToStr(int value)
{
	char buf[32];
	sprintf(buf, "%i", value);
	std::string str = buf;
	return str;
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

bool LFGetDirFiles(const std::filesystem::path& dirPath, std::vector<std::filesystem::path>& filePaths)
{
	filePaths.clear();
	if (!std::filesystem::exists(dirPath))
		return false;
	for (auto const& dir_entry : std::filesystem::directory_iterator{ dirPath }) {
		filePaths.push_back(dir_entry.path());
	}
	return true;
}

bool LFIsImageFile(const char* fileName)
{
	return utils::IsImageFile(fileName);
}

bool LFIsVideoFile(const char* fileName)
{
	return utils::IsVideoFile(fileName);
}

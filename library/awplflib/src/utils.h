#pragma once
#include <filesystem>

namespace utils
{
	inline bool IsImageFile(const std::filesystem::path& filePath)
	{
		std::string strExt = filePath.extension().string();
		std::transform(strExt.begin(), strExt.end(), strExt.begin(), ::tolower);

		if (strExt == ".awp")
			return true;
		if (strExt == ".jpg")
			return true;
		if (strExt == ".jpeg")
			return true;
		if (strExt == ".png")
			return true;
		if (strExt == ".bmp")
			return true;
		return false;
	}

	inline bool IsVideoFile(const std::filesystem::path& filePath)
	{
		std::string strExt = filePath.extension().string();
		std::transform(strExt.begin(), strExt.end(), strExt.begin(), ::tolower);

		if (strExt == ".avi")
			return true;
		if (strExt == ".mp4")
			return true;

		return false;
	}
}
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
//		File: _LFMediaSource.cpp
//		Purpose: implementation of ILFMediaSource interface 
//
//      CopyRight 2004-2018 (c) NN-Videolab.net
//M*/
#include "LFCore.h"
#include "LFInterface.h"
#include "LFFileUtils.h"

// single image as media source 
// support images in the awp and jpeg file format 
class TLFImageSource : public TLFObject, ILFMediaSource
{
protected: 
	TLFImage m_image;
public: 
	TLFImageSource(OnMediaNewFrame OnNewFrame);
	virtual ~TLFImageSource();

	virtual bool Open(const char* name);
	virtual void Close();

	virtual unsigned int NumFrames();
	virtual unsigned int CurrentFrame();
	virtual bool SetCurrentFrame(unsigned int frame);

	virtual bool First();
	virtual bool Prev();
	virtual bool Next();
	virtual bool Last();

	//virtual void Play();
	//virtual void Stop();
	//virtual bool IsPlaying();

	virtual ILFImage* Frame();
};


/// implementation  TLFImageSource
TLFImageSource::TLFImageSource(OnMediaNewFrame OnNewFrame):ILFMediaSource(OnNewFrame)
{

}

TLFImageSource::~TLFImageSource()
{

}

bool TLFImageSource::Open(const char* name)
{
	const bool result =  m_image.LoadFromFile(name);
	if (result && m_OnNewFrame != NULL)
		m_OnNewFrame(0, &m_image);
	return result;
}
void TLFImageSource::Close()
{
	m_image.FreeImages();
}

unsigned  int TLFImageSource::NumFrames()
{
	return 1;
}
unsigned int TLFImageSource::CurrentFrame()
{
	return 0;
}
bool TLFImageSource::SetCurrentFrame(unsigned int frame)
{
	return frame == 0;
}

bool TLFImageSource::First()
{
	//dummy
	return true;
}
bool TLFImageSource::Prev()
{
	//dummy
	return true;

}
bool TLFImageSource::Next()
{
	// dummy
	return true;

}
bool TLFImageSource::Last()
{
	// dummy 
	return true;
}
ILFImage* TLFImageSource::Frame()
{
	return &m_image;
}

class TLFImagesSource : public TLFObject, ILFMediaSource
{
protected: 
	TLFStrings m_fileNames;
	TLFImage   m_image;
	unsigned int        m_currentFrame;
	bool	   DecodeCurrentFrame();
public: 
	TLFImagesSource(OnMediaNewFrame OnNewFrame);
	virtual ~TLFImagesSource();

	virtual bool Open(const char* name);
	virtual void Close();

	virtual unsigned int NumFrames();
	virtual unsigned int CurrentFrame();
	virtual bool SetCurrentFrame(unsigned int frame);

	virtual bool First();
	virtual bool Prev();
	virtual bool Next();
	virtual bool Last();

	virtual ILFImage* Frame();
};

static bool _GetImageNames(const char* path, TLFStrings& names)
{
	names.clear();
	TLFStrings allFiles;
	if (!LFGetDirFiles(path, allFiles))
		return false;
	for (unsigned int i = 0; i < allFiles.size(); i++)
	{
		if (LFIsImageFile(allFiles[i].c_str()))
			names.push_back(allFiles[i]);
	}
	return true;
}

/// implementation  TLFImagesSource
TLFImagesSource::TLFImagesSource(OnMediaNewFrame OnNewFrame) :ILFMediaSource(OnNewFrame)
{
	m_currentFrame = 0;
}

TLFImagesSource::~TLFImagesSource()
{
}

bool TLFImagesSource::Open(const char* name)
{
	
	if (!_GetImageNames(name, m_fileNames))
		return false;
	if (m_fileNames.size() == 0)
		return false;
	First();
	return true;
}
void TLFImagesSource::Close()
{
	m_fileNames.clear();
	m_image.FreeImages();
}

unsigned int TLFImagesSource::NumFrames()
{
	return m_fileNames.size();
}
unsigned int TLFImagesSource::CurrentFrame()
{
	return m_currentFrame;
}
bool TLFImagesSource::SetCurrentFrame(unsigned int frame)
{
	if (frame >= m_fileNames.size())
		return false;
	m_currentFrame = frame;
	return DecodeCurrentFrame();
}

bool TLFImagesSource::First()
{
	m_currentFrame = 0;
	return DecodeCurrentFrame();
}
bool TLFImagesSource::Prev()
{
	if (m_currentFrame < 1)
		return true;
	m_currentFrame--;
	return DecodeCurrentFrame();
}
bool TLFImagesSource::Next()
{
	m_currentFrame++;
	if (m_currentFrame >= 0)
	{
		m_currentFrame = 0;
	}
	return DecodeCurrentFrame();
}
bool TLFImagesSource::Last()
{
	m_currentFrame = m_fileNames.size() - 1;
	return DecodeCurrentFrame();
}

ILFImage* TLFImagesSource::Frame()
{
	return &m_image;
}

bool TLFImagesSource::DecodeCurrentFrame()
{
	const char* lpFileName = m_fileNames[m_currentFrame].c_str();
	if (!m_image.LoadFromFile(lpFileName))
		return false;
	if (m_OnNewFrame != NULL)
		m_OnNewFrame(m_currentFrame, &m_image);
	return true;
}

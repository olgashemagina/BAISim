//---------------------------------------------------------------------------

#include <System.hpp>
#include <Vcl.Dialogs.hpp>
#pragma hdrstop

#include "PhReadImagesThread.h"
#include "DIBImage41.h"
#include "PhFrames.h"
#pragma package(smart_init)


static awpImage* _awpFitImage(awpRect rect, awpImage* img)
{
	int W = rect.right - rect.left;
	int H = rect.bottom - rect.top;

	double ar1 = (double) W / (double) H;
	double ar2 = (double) img->sSizeX / (double)img->sSizeY;

	awpImage* result = NULL;
	awpCreateImage(&result, W, H, img->bChannels, AWP_BYTE);

	if (result != NULL)
	{
	  int sx = 0;
	  int sy = 0;
	  int w  = img->sSizeX;
	  int h  = img->sSizeY;
	  int w1, h1;
	  double alfa = 1;

	  if (ar1 > ar2)
	  {
		 h1 = H;
		 alfa = (double)H/(double)h;
		 w1 = floor(alfa*w );
		 sx = (W - w1) /2;
		 if (sx < 0) sx = 0;
		 sy = 0;
	  }
	  else
	  {
		w1 = W;
		alfa = (double)W / (double)w;
		h1 = floor((double)h*alfa );
		sx = 0;
		sy = (H-h1) /2;
		if (sy < 0) sy = 0;
	  }
	  awpPoint p;
	  p.X = sx;
	  p.Y = sy;
	  awpImage* dst = NULL;

	  awpCopyImage(img, &dst);
	  awpResizeBilinear(dst, w1, h1);

	  if (dst != NULL)
	  {
		awpPasteRect( dst, result,  p);
		_AWP_SAFE_RELEASE_(dst)
	  }
	  else
	  {
		_AWP_SAFE_RELEASE_(result)
	  }

	}
	return result;
}
//---------------------------------------------------------------------------
__fastcall IPhThread::IPhThread(TList* items) : TThread(true)
{
    this->m_items = items;
    m_OnProgress = NULL;
    m_cancel = false;
}
void __fastcall IPhThread::Cancel()
{
	m_cancel = true;
	this->Terminate();
}
void __fastcall IPhThread::ProgressHelper()
{
    if (m_OnProgress)
		m_OnProgress(this, m_msg, m_progress);
}
__fastcall  TPhJobThread::TPhJobThread(TList* names, const LPWSTR lpwFolderName, EPhJobReason reason)
: IPhThread(names)
{
	m_reason = reason;
	m_FolderName = lpwFolderName;
	m_mosaic 	= NULL;
	m_targetFormat = jpegFormat;
	m_keepSource = true;
	m_action = copyReplace;
}

int __fastcall TPhJobThread::GetNumSelectedItems()
{
   if (this->m_items == NULL)
	return 0;
	int num_selected = 0;
	for (int i = 0; i < m_items->Count; i++)
	{
		SFrameItem* item = (SFrameItem*)(m_items->Items[i]);
		if (item->selected)
			num_selected++;
	}
	return num_selected;
}


void __fastcall TPhJobThread::Execute()
{
	if (m_items == NULL)
		return;

	 switch(m_reason)
	 {
		case readJob:
			DoReadJob();
		break;
		case copyJob:
			DoCopyJob();
		break;
		case moveJob:
			DoMoveJob();
		break;
		case deleteJob:
			DoDeleteJob();
		break;
		case convertJob:
			DoConvertJob();
		break;
		case processJob:
			DoProcessJob();
		break;
	 }
}

void __fastcall TPhJobThread::DoReadJob()
{
	TDIBImage* dib = new TDIBImage();

    int imgCount =  floor(sqrt((float)this->m_items->Count) + 0.5);

	awpRect rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = m_tmbWidth;
	rect.bottom = m_tmbHeight;
    int num = 0;
	for (int i = 0; i < m_items->Count; i++)
	{
		//
		if (this->Terminated)
		{
			break;
		}
		m_msg = L"Reading... ";
		m_msg += IntToStr(i);
		m_msg += L" of ";
		m_msg += IntToStr(m_items->Count);
		m_progress =  100*(i+1)/m_items->Count;
		Synchronize(&this->ProgressHelper);
		try
		{
			SFrameItem* item = (SFrameItem*)(m_items->Items[i]);
			dib->LoadFromFile(item->strFileName);
			awpImage* tmp = NULL;
			dib->GetAWPImage(&tmp);
			if (tmp != NULL)
			{

				item->width = tmp->sSizeX;
				item->height = tmp->sSizeY;
				awpImage* fit = _awpFitImage(rect, tmp);
                item->img = fit;
				if (fit != NULL)
				{
					num++;
                }
                _AWP_SAFE_RELEASE_(tmp)
			}

        }
        catch(Exception& e)
		{

            //todo: if the thread cannot read the image
            //remove item about it.

			continue;
		}
	 }
	delete dib;
    if (!Terminated)
		DoMosaic();
}
void __fastcall TPhJobThread::DoDeleteJob()
{
	int num_selected = this->GetNumSelectedItems();
	if (num_selected == 0)
		return;


	int c = 0;
	for (int i = m_items->Count - 1; i >= 0 ; i--)
	{
		//
		if (this->Terminated)
			break;
		m_msg = L"Delete images... ";
		m_msg += IntToStr(c);
		m_msg += L" of ";
		m_msg += IntToStr(num_selected);
		m_progress =  100*(c+1)/num_selected;
		Synchronize(&this->ProgressHelper);
		try
		{
			SFrameItem* item = (SFrameItem*)(m_items->Items[i]);
			if (!item->selected)
				continue;
			c++;

			DeleteFile(item->strFileName);
			m_items->Delete(i);
		}
		catch(Exception& e)
		{
			//todo: if the thread cannot read the image
			//remove item about it.
			continue;
		}
	 }
	 this->DoMosaic();
}
void __fastcall TPhJobThread::DoMoveJob()
{
	int num_selected = this->GetNumSelectedItems();
	if (num_selected == 0)
		return;

	int c = 0;
	for (int i = m_items->Count - 1; i >= 0 ; i--)
	{
		//
		if (this->Terminated)
			break;

		m_msg = L"Move images... ";
		m_msg += IntToStr(c);
		m_msg += L" of ";
		m_msg += IntToStr(num_selected);
		m_progress =  100*(c+1)/num_selected;
		Synchronize(&this->ProgressHelper);
		try
		{
			SFrameItem* item = (SFrameItem*)(m_items->Items[i]);
			if (!item->selected)
				continue;
			c++;

			UnicodeString strSrcFile = item->strFileName;
			UnicodeString strDstFile = m_FolderName;
			if (!PhCopyFile(strSrcFile, strDstFile, m_action))
			{
				//todo: add error status
				//ShowMessage(L"Cannot copy files to target folder: " + FolderName);
			}
			DeleteFile(item->strFileName);
			m_items->Delete(i);
		}
		catch(Exception& e)
		{
			//todo: if the thread cannot read the image
			//remove item about it.
			continue;
		}
	 }
	 DoMosaic();
}

void __fastcall TPhJobThread::DoCopyJob()
{
	int num_selected = this->GetNumSelectedItems();
	if (num_selected == 0)
		return;

	int c = 0;
	for (int i = m_items->Count - 1; i >= 0 ; i--)
	{
		//
		if (this->Terminated)
			break;

		m_msg = L"Copy images... ";
		m_msg += IntToStr(c);
		m_msg += L" of ";
		m_msg += IntToStr(num_selected);
		m_progress =  100*(c+1)/num_selected;
		Synchronize(&this->ProgressHelper);
		try
		{
			SFrameItem* item = (SFrameItem*)(m_items->Items[i]);
			if (!item->selected)
				continue;
			c++;

			UnicodeString strSrcFile = item->strFileName;
			UnicodeString strDstFile = m_FolderName;
			if (!PhCopyFile(strSrcFile, strDstFile, m_action))
			{
				//todo: add error status
				//ShowMessage(L"Cannot copy files to target folder: " + FolderName);
			}

		}
		catch(Exception& e)
		{
			//todo: if the thread cannot read the image
			//remove item about it.
			continue;
		}
	 }
}
static UnicodeString _getFormatExt(EPhImageFormats format)
{
	 switch(format)
	 {
		case jpegFormat:
		   return L".jpg";
		case pngFormat:
			return L".png";
		case tiffFormat:
			return L".tif";
		case  tgaFormat:
			return L".tga";
		 case  bmpFormat:
			return L".bmp";
		 case awpFormat:
			return L".awp";
		 case ppmFormat:
			return L".ppm";
	 }
	 return L".jpg";
}

static bool _needConvertImage(EPhImageFormats format, UnicodeString& strExt)
{
	 switch(format)
	 {
		case jpegFormat:
		   return !(strExt == L".jpg" || strExt == L".jpeg");
		case pngFormat:
			return strExt != L".png";
		case tiffFormat:
			return !(strExt == L".tif" || strExt == L".tiff");
		case  tgaFormat:
			return strExt != L".tga";
		 case  bmpFormat:
			return strExt != L".bmp";
		 case awpFormat:
			return strExt != L".awp";
		 case ppmFormat:
			return strExt != L".ppm";
	 }
	 return true;
}
void __fastcall TPhJobThread::DoConvertJob()
{
	int num_selected = this->GetNumSelectedItems();
	if (num_selected == 0)
		return;

	int c = 0;
	for (int i = m_items->Count - 1; i >= 0 ; i--)
	{
		//
		if (this->Terminated)
			break;

		m_msg = L"Convert images... ";
		m_msg += IntToStr(c);
		m_msg += L" of ";
		m_msg += IntToStr(num_selected);
		m_progress =  100*(c+1)/num_selected;
		Synchronize(&this->ProgressHelper);
		try
		{
			SFrameItem* item = (SFrameItem*)(m_items->Items[i]);
			if (!item->selected)
				continue;
			c++;
			UnicodeString strExt = ExtractFileExt(item->strFileName);
			if (!_needConvertImage(m_targetFormat, strExt ))
				continue;
			TDIBImage* image = new TDIBImage();
            image->LoadFromFile(item->strFileName);
			{
				UnicodeString strOutputFile = ChangeFileExt(item->strFileName, _getFormatExt(m_targetFormat));
				image->SaveToFile(strOutputFile);
				delete image;
				if (!this->m_keepSource)
					DeleteFile(item->strFileName);
				item->strFileName = strOutputFile;
			}
		}
		catch(Exception& e)
		{
			//todo: if the thread cannot read the image
			//remove item about it.
			continue;
		}
	 }
}
void __fastcall TPhJobThread::DoProcessJob()
{
    // todo with image processor.
}
void __fastcall TPhJobThread::BeforeDestruction(void)
{
}



void __fastcall TPhJobThread::DoMosaic()
{
	 if (this->m_items == NULL)
		return;

   int imgCount =  ceil(sqrt((float)this->m_items->Count) + 0.5);
   int imgWidth  = m_tmbWidth*imgCount;
   int imgHeight = m_tmbHeight*imgCount;
   awpImage* result = NULL;
   awpCreateImage(&result, imgWidth, imgHeight, 3, AWP_BYTE);
   for (int i = 0; i < m_items->Count; i++)
   {
		SFrameItem* item = (SFrameItem*)(m_items->Items[i]);
		if (item->img != NULL)
        {
             awpPoint p;
             p.Y = (i / imgCount)*m_tmbHeight;
             p.X = (i % imgCount)*m_tmbWidth;
             awpPasteRect(item->img, result, p);
        }
   }
    _AWP_SAFE_RELEASE_(m_mosaic)
	awpCopyImage(result, &m_mosaic);
	_AWP_SAFE_RELEASE_(result)
}

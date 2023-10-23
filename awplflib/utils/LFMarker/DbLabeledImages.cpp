//---------------------------------------------------------------------------
// database of labeled images
// ver 1.0
// implementation of TDbLabeledImages

#pragma hdrstop
#include <math.h>
#include "DbLabeledImages.h"
#include "_LF.h"
#include "MainForm.h"
#include "awpipl.h"
#include "LongProcessForm.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

static bool _IsImageFile(UnicodeString& strFileName)
{
	UnicodeString strExt = ExtractFileExt(strFileName);
	strExt = strExt.LowerCase();

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
	if (strExt == ".tif")
		return true;
	return false;
}
/*
	проверяет, имеет ли файл изображения strFileName
	какое-либо из описаний.
*/
static bool _HasDescription(UnicodeString& strFileName)
{
  if (!_IsImageFile(strFileName))
	return false;

  if (FileExists(ChangeFileExt(strFileName, ".ieye")))
	  return true;
  if (FileExists(ChangeFileExt(strFileName, ".xml")))
	  return true;
  if (FileExists(ChangeFileExt(strFileName, ".face")))
	  return true;

  return false;
}

// очищает каталог.
static bool ClearDir(AnsiString& strDir)
{
  if (DirectoryExists(strDir))
  {
	// удалим все файлы в каталоге
	TSearchRec sr;
	AnsiString strPath = strDir;
	strPath += "\\*.*";
	int iAttr =0;
	iAttr |= faAnyFile;
	if (FindFirst(strPath, iAttr, sr) == 0)
	{
		do
		{
		  AnsiString strName =  strDir + "\\" + sr.Name;
		  DeleteFile(strName);
		}while(FindNext(sr) == 0);
		FindClose(sr);
	}
  }
  else
	CreateDir(strDir);
  return true;
}

static bool ClearDirWithSubdirs(AnsiString& strDir, TStringList* subdirs)
{
	AnsiString strMessage = "All files in the \n";
	strMessage += strDir;
	strMessage += "\n will be deteled. Are You sure?";
	if (MessageDlg(strMessage, mtWarning, TMsgDlgButtons() << mbYes << mbNo,0 ) == mrNo)
		return true;

  if (DirectoryExists(strDir))
  {
    for (int i = 0; i < subdirs->Count; i++)
    {
    	AnsiString strPath = strDir + "\\" + subdirs->Strings[i] + "\\";
		if (DirectoryExists(strPath))
        {
            ClearDir(strPath);
          //  RemoveDir(strPath);
        }
        else
			CreateDir(strPath);
    }
  }
  else
  {
	CreateDir(strDir);
    for (int i = 0; i < subdirs->Count; i++)
    {
    	AnsiString strPath = strDir + "\\" + subdirs->Strings[i] + "\\";
		CreateDir(strPath);
    }
  }
  return true;
}

void _progress(const char* lpMessage, int progress)
{
    LongProcDlg->Label1->Caption = lpMessage;
    LongProcDlg->ProgressBar1->Position = progress;
    Application->ProcessMessages();
}

////////////////////////////////////////////////////////////////////////////////
TDbLabeledImages::TDbLabeledImages()
{
	Clear();
	m_ProgressEvent = NULL;
    m_db.SetProgress(&_progress);
}
/////////////////////////////////////
TDbLabeledImages::~TDbLabeledImages()
{
}
////////////////////////////////////////
void __fastcall TDbLabeledImages::Clear()
{
    m_strDbName = "";
	m_NumImages = 0;
    m_NumXmlFiles =0;
    m_NumXmlItems =0;
	m_engine = NULL;
}
//////////////////////////////////////////////////////////////////////////////////////
bool __fastcall TDbLabeledImages::Init(AnsiString& strDbPath, ILFDetectEngine* engine)
{
	 //Init dictionary
	m_NumImages = 0;
    m_NumXmlFiles =0;
    m_NumXmlItems =0;
	m_engine = NULL;

     AnsiString strDictionaryName = strDbPath;
	 strDictionaryName += "\\";
     strDictionaryName += c_lpDictFileName ;
	 if (!m_db.InitDB(strDbPath.c_str()))
        return false;

    m_NumImages = m_db.GetImagesCount();
    m_NumXmlFiles = m_db.GetDescrFilesCount();
    m_NumXmlItems = m_db.GetItemsCount();
    m_strDbName = strDbPath;
    m_engine = engine;
}

void __fastcall TDbLabeledImages::SaveFragment(awpImage* img, SDbExportOptions& options, int count, TLFRect& scanBox, /*TLFRoiImageDescriptor* roid,*/ const char* lpClassLabel)
{
      srand((unsigned)time(NULL));

      bool save = true;
	  if (options.copyRandom)
			save =  rand() % 100 < options.random;

      if (!save)
      	return;

      awpImage* pFragment = NULL;
      awpRect rr = scanBox.GetRect();
	  awpCopyRect(img, &pFragment, &rr);
      if (pFragment == NULL)
      {
			return;
      }
      awpConvert(pFragment, AWP_CONVERT_3TO1_BYTE);
      AnsiString strFileName = MakeExportFileName(options, count, false, lpClassLabel);

	  /*if (roid != NULL)
      {
		AnsiString strEyeName = ChangeFileExt(strFileName, ".ieye");
		TLFRoiImageDescriptor rd;
        for (int i = 0; i < roid->GetNumRois(); i++)
        {
			TLFRoi* roi = roid->GetRoi(i);
            int dx = -scanBox.Left();
			int dy = -scanBox.Top();
            roi->Shift(dx,dy);
            TROI r = roi->GetRoi();
            if (options.needResize)
            {
                double alfa = (double)options.baseSize / (double)pFragment->sSizeX;
                r.p.X *= alfa;
                r.p.Y *= alfa;
                r.p1.X *= alfa;
                r.p1.Y *= alfa;
            }

                rd.AddRoi(r.p, r.p1);
        }
        rd.SaveToFile(strEyeName.c_str());
	  }*/
      if (options.needResize)
      {
		 int w = options.baseSize;
         int h = (int)floor((double)w*(double)pFragment->sSizeY / (double)pFragment->sSizeX + 0.5);

		 awpResizeBilinear(pFragment, w,h);
      }


		awpSaveImage(strFileName.c_str(), pFragment);


      if (options.needFlip)
      {
          strFileName = MakeExportFileName(options, count, true, lpClassLabel);

		 /* if (roid != NULL)
		  {
            AnsiString strEyeName = ChangeFileExt(strFileName, ".ieye");
			TLFRoiImageDescriptor rd;
            for (int i = 0; i < roid->GetNumRois(); i++)
            {
                TLFRoi* roi = roid->GetRoi(i);
                int dx = -scanBox.Left();
                int dy = -scanBox.Top();
                roi->Shift(dx,dy);
                if (options.needResize)
                {
                    double alfa = (double)options.baseSize / (double)pFragment->sSizeX;
                    roi->Scale(alfa);
				}
                    TROI r = roi->GetRoi();
					r.p.X = pFragment->sSizeX - r.p.X;
                    r.p1.X = pFragment->sSizeX - r.p1.X;
                    rd.AddRoi(r.p, r.p1);
            }
            rd.SaveToFile(strEyeName.c_str());
           }  */

		  awpFlip(&pFragment, AWP_FLIP_HRZT);
		  if (options.needResize)
          {
             int w = options.baseSize;
             int h = (int)floor((double)w*(double)pFragment->sSizeY / (double)pFragment->sSizeX + 0.5);
               awpResizeBilinear(pFragment, w,h);
          }
          awpConvert(pFragment, AWP_CONVERT_3TO1_BYTE);
          awpSaveImage(strFileName.c_str(), pFragment);

      }
      awpReleaseImage(&pFragment);

}
void __fastcall TDbLabeledImages::ExportFragments(SDbExportOptions& options)
{
	if (!ClearDirWithSubdirs(options.strPathToExport, options.ClassLabels))
		return;
   TLFDBLabeledImages db(m_strDbName.c_str());
   TSearchRec sr;
   AnsiString strPath = m_strDbName;
   strPath += "\\*.xml";

   int iAttr =0;
   iAttr |= faAnyFile;
   int num = 0;
   int count = 0;

	if (FindFirst(strPath, iAttr, sr) == 0)
	{
	  do
	  {
		 TLFSemanticImageDescriptor sd;
		 num++;
		 awpImage* img = NULL;
		 bool exist = false;
		 AnsiString strTmpFile = ChangeFileExt(sr.Name, ".jpg");
		 AnsiString strFullName = m_strDbName;
		 strFullName += "\\";
		 if (FileExists(strTmpFile))
		 {
		   exist = true;
		   strFullName += strTmpFile;
		   awpLoadImage(strFullName.c_str(), &img);
		 }
		 else
		 {
			strTmpFile = ChangeFileExt(sr.Name, ".jpeg");
			if (FileExists(strTmpFile))
			{
				strFullName += strTmpFile;
				awpLoadImage(strFullName.c_str(), &img);
				exist = true;
			}
			else
			{
			  strTmpFile = ChangeFileExt(sr.Name, ".awp");
			  if (FileExists(strTmpFile))
			  {
				  strFullName += strTmpFile;
				  awpLoadImage(strFullName.c_str(), &img);
				  exist = true;
			  }
			}
		 }
		 if (!exist)
			continue;

		 /*TLFRoiImageDescriptor* rd = NULL;
		 if (options.copyIeye)
		 {
			rd = new TLFRoiImageDescriptor();
			AnsiString strIeyeFile = ChangeFileExt(sr.Name, ".ieye");
			rd->LoadFromFile(strIeyeFile.c_str());
			if (rd->GetNumRois() == 0)
			{
				delete rd;
				rd = NULL;
			}
		 }  */

		 AnsiString _ansi = sr.Name;
		 sd.LoadXML(_ansi.c_str());
		 sd.SetImage(img);

		 for (int i = 0; i < sd.GetItemsCount(); i++)
		 {
			TLFDetectedItem*  di = sd.GetDetectedItem(i);
			if (di == NULL)
				continue;
			if (di->GetRacurs() != options.racurs)
				continue;
			UnicodeString strClassLabel;
			std::string str1 = db.GetDictinary()->GetWordByUUID(di->GetType().c_str());
			strClassLabel = str1.c_str();
			if (options.ClassLabels->IndexOf(strClassLabel) < 0)
				continue;
			   AnsiString _ansi = strClassLabel;
			TLFRect* bounds = di->GetBounds();
			awpRect r = bounds->GetRect();
			if (options.useScanner)
			{
			  TLFScanner scanner;
			  TLFImage   lfimg;

   		     scanner.SetBaseWidth(di->GetBW());
			 scanner.SetBaseHeight(di->GetBH());


			  lfimg.SetImage(img);
			  scanner.Scan(lfimg.GetImage()->sSizeX, lfimg.GetImage()->sSizeY);
			  if (!options.exportNearlest)
			  {
				for (int j = 0; j < scanner.GetFragmentsCount(); j++)
				{
				  awpRect r = scanner.GetFragmentRect(j);
				  TLFRect scanBox; scanBox.SetRect(r);
				  if (options.exportCovered && !scanBox.IsCover(*bounds))
				  {
                      continue;
				  }
				  if (bounds->RectOverlap(scanBox) > options.scannerThreshold)
				  {
					  count++;
					  SaveFragment(img, options, count, scanBox, /*rd,*/ _ansi.c_str());
				  }
				}
			  }
			  else
			  {
				float maxOverlap = 0;
				TLFRect maxRect;
				for (int j = 0; j < scanner.GetFragmentsCount(); j++)
				{
				  awpRect r = scanner.GetFragmentRect(j);
				  TLFRect scanBox; scanBox.SetRect(r);
				  float overlap = bounds->RectOverlap(scanBox);
				  if (overlap > maxOverlap)
				  {
					  maxOverlap = overlap;
					  maxRect = scanBox;
				  }
				}
			  //  if (maxOverlap >= options.scannerThreshold)
				{
					count++;
					SaveFragment(img, options, count, maxRect, /*rd,*/  _ansi.c_str());
				}
			  }
			}
			else
			{
				count++;
				TLFRect scanBox;
				scanBox.SetRect(r);
				SaveFragment(img, options, count, scanBox, /*rd,*/  _ansi.c_str());
			}
		 }
		 /*if (rd != NULL)
		 {
			delete rd;
			rd = NULL;
		 }  */
		 awpReleaseImage(&img);
		 if (m_ProgressEvent != NULL)
		 {
			AnsiString strComment = "Processed ";
			strComment += IntToStr (num);
			strComment += " of ";
			strComment += IntToStr(m_NumXmlFiles);
			m_ProgressEvent(int(100 *num / m_NumXmlFiles),strComment);
		 }

		 Application->ProcessMessages();

	  }while(FindNext(sr) == 0);
	  FindClose(sr);
	}

	if (options.copyBackground)
	{
		this->SaveBackground(options);
	}
}

AnsiString  TDbLabeledImages::MakeExportFileName(SDbExportOptions& options, int num, bool fliped, const char* lpClassLabel)
{
	UUID id;
	UuidCreate(&id);
	std::string str = LFGUIDToString(&id);

	AnsiString strName;
	strName = options.strPathToExport;
	strName += "\\";
    if (lpClassLabel != NULL)
	{
		strName += lpClassLabel;
		strName += "\\";
    }
	if (fliped)
		strName += "fliped_";
	strName += str.c_str();
	strName += options.exportFormat == jpeg?".jpg":".awp";
	return strName;
}

static bool _HasImage(UnicodeString& strFileName)
{
   if (FileExists(ChangeFileExt(strFileName, ".jpg")))
	return true;
   if (FileExists(ChangeFileExt(strFileName, ".jpeg")))
	return true;
   if (FileExists(ChangeFileExt(strFileName, ".awp")))
	return true;
   return false;
}
//////////////////////////////////////////////////////////////////////////
static AnsiString GetImageName(AnsiString& strFileName)
{
   if (FileExists(ChangeFileExt(strFileName, ".jpg")))
	return ChangeFileExt(strFileName, ".jpg");
   if (FileExists(ChangeFileExt(strFileName, ".jpeg")))
	return ChangeFileExt(strFileName, ".jpeg");
   if (FileExists(ChangeFileExt(strFileName, ".awp")))
	return ChangeFileExt(strFileName, ".awp");
   return "";
}
////////////////////////////////////////////////////////////////////////////////
void __fastcall TDbLabeledImages::ConvertDatabase(SDbConvertOptions& options)
{
   TSearchRec sr;
   AnsiString strPath = Form1->DirectoryListBox1->Directory + "\\";// m_strDbName;
   TStringList* sl = new TStringList();
   strPath += "\\*.*";
   int iAttr =0;
   iAttr |= faAnyFile;
   int num = 0;

   if (FindFirst(strPath, iAttr, sr) == 0)
   {
	  do
	  {
		 if (_IsImageFile(sr.Name))
		 {
			num++;
			sl->Add(sr.Name);
		 }

	  }while(FindNext(sr) == 0);

	  FindClose(sr);
   }

   for (int i = 0; i < sl->Count; i++)
   {
		AnsiString str = sl->Strings[i];
		Form1->PhImage1->Bitmap->LoadFromFile(str);
		awpImage* img = NULL;
		Form1->PhImage1->GetAwpImage(&img);

		if (img)
		{
		  TLFSemanticImageDescriptor* sd = NULL;
		  AnsiString strXml = ChangeFileExt(str, ".xml");
		  if (FileExists(ChangeFileExt(str, ".xml")))
		  {
			sd = new TLFSemanticImageDescriptor();
			sd->LoadXML(strXml.c_str());
		  }
		  if (options.needResize)
		  {
			  // resize
			  int width  = options.baseWidth;
			  double factor = (double)width / (double)img->sSizeX;
			  int height = floor(factor*img->sSizeY + 0.5);
			  if (!options.interpolation)
				awpResize(img, width, height);
			  else
				awpResizeBilinear(img, width, height);
			  if (sd != NULL)
			  {

				sd->Resize(factor);
				sd->SetImage(img);
			  }
		  }
		  if (options.RenameToUUID)
		  {
			  //////////////////////
			  UUID id;
			  LF_UUID_CREATE(id);
			  std::string strUUID = LFGUIDToString(&id);
			  AnsiString strFileName = Form1->DirectoryListBox1->Directory + "\\";
			  strFileName += strUUID.c_str();
			  AnsiString strExt = options.format == awp ? ".awp" : ".jpg";
			  strFileName += strExt;
			  awpSaveImage(strFileName.c_str(), img);
			  strFileName = ChangeFileExt(strFileName, ".xml");
			  if (sd != NULL)
			  {
				sd->SaveXML(strFileName.c_str());
				DeleteFile(strXml);
			  }
			  DeleteFile(str);

		  }
		  else
		  {
			  if (sd != NULL)
				sd->SaveXML(strXml.c_str());
			  // make new filename
			  AnsiString strFileName = Form1->DirectoryListBox1->Directory + "\\" + str;
			  AnsiString strExt = options.format == awp ? ".awp" : ".jpg";
			  strFileName = ChangeFileExt(strFileName, strExt);
			  if (strExt != ExtractFileExt(str))
				DeleteFile(str);
			  awpSaveImage(strFileName.c_str(), img);
		  }
		  awpReleaseImage(&img);

		  if (m_ProgressEvent != NULL)
		  {
			AnsiString _ansi = strXml;
			m_ProgressEvent(int(100 *i / sl->Count),_ansi );
		  }
		}
		Application->ProcessMessages();
	 }


   delete sl;
}
// копирование базы данных, в соответствии с опициями.
void __fastcall TDbLabeledImages::CopyDatabase(SDbCopyOptions& options)
{
   if (m_NumImages==0)
	  return;
   if (this->DbName == options.strPathToCopy)
	  return;

//	if (!ClearDir(options.strPathToCopy))
//		return;

   TSearchRec sr;
   AnsiString strPath = m_strDbName;

   strPath += "\\*.*";
   int iAttr =0;
   iAttr |= faAnyFile;
   int num = 0;

   int total = options.copyImages ? m_NumImages : 0;
   total += options.copySemantic ? NumXmlFiles : 0;

   if (FindFirst(strPath, iAttr, sr) == 0)
   {
	  do
	  {
		 num++;
		 bool was_copy = false;
		 if (options.copyImages && _IsImageFile(sr.Name))
		 {
			String strFileName = options.strPathToCopy;
			strFileName += "\\";
			strFileName += ExtractFileName(sr.Name);
			String _ansi = this->m_strDbName;
			_ansi += "\\";
			_ansi+= sr.Name;
			CopyFile(_ansi.c_str(), strFileName.c_str(), true);
			was_copy = true;
		 }

		 AnsiString strExt = ExtractFileExt(sr.Name);
		 strExt.LowerCase();
		 if (options.copySemantic && strExt == ".xml")
		 {
			String strFileName = options.strPathToCopy;
			strFileName += "\\";
			strFileName += ExtractFileName(sr.Name);
			String _ansi = this->m_strDbName;
			_ansi += "\\";
			_ansi+= sr.Name;
			CopyFile(_ansi.c_str(), strFileName.c_str(), true);
			was_copy = true;
		 }
		 if (m_ProgressEvent != NULL && was_copy)
		 {
			AnsiString _ansi = sr.Name;
			m_ProgressEvent(int(100 *num / total),_ansi );
		 }

		 Application->ProcessMessages();

	  }while(FindNext(sr) == 0);
	  FindClose(sr);
   }
}

awpImage* FitImage(awpRect rect, awpImage* img)
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

awpImage* __fastcall TDbLabeledImages::GetDbThumbinals(int thmbWidth, int thmbHeight)
{
   if (this->m_NumImages == 0)
	return NULL;
   int imgCount =  floor(sqrt((float)this->m_NumImages) + 0.5);
   int imgWidth  = thmbWidth*imgCount;
   int imgHeight = thmbHeight*imgCount;
   awpImage* result = NULL;
   awpCreateImage(&result, imgWidth, imgHeight, 3, AWP_BYTE);

   TSearchRec sr;
   AnsiString strPath = m_strDbName;

   strPath += "\\*.*";
   int iAttr =0;
   iAttr |= faAnyFile;
   int num = 0;
   awpRect rect;
   rect.left = 0;
   rect.top = 0;
   rect.right = thmbWidth;
   rect.bottom = thmbHeight;
   if (FindFirst(strPath, iAttr, sr) == 0)
   {
	  do
	  {

		 if (_IsImageFile(sr.Name))
		 {
			Form1->PhImage1->Bitmap->LoadFromFile(sr.Name);
			awpImage* img = NULL;
			Form1->PhImage1->GetAwpImage(&img);
			if (img)
			{
				awpImage* fit = FitImage(rect, img);
				if (fit && FileExists(ChangeFileExt(sr.Name, ".xml")))
				{
					awpRect rect1;
					rect1.left = rect.left + 1;
					rect1.top  = rect.top+1;
					rect1.right = rect.right -2;
					rect1.bottom = rect.bottom - 2;
					awpDrawCRect(fit, &rect1, 255,255,255, 1);
				}

				if (fit)
				{
					 awpPoint p;
					 p.Y = (num / imgCount)*thmbHeight;
					 p.X = (num % imgCount)*thmbWidth;

					 awpPasteRect(fit, result, p);

					_AWP_SAFE_RELEASE_(fit)
				}
				_AWP_SAFE_RELEASE_(img)
			}
			 num++;
		 }

		 if (m_ProgressEvent != NULL)
		 {
			AnsiString _ansi = "";
			m_ProgressEvent(int(100 *num / this->m_NumImages),_ansi );
		 }

		 Application->ProcessMessages();

	  }while(FindNext(sr) == 0);
	  FindClose(sr);
   }


   return result;
}

awpImage* __fastcall TDbLabeledImages::MakeSemanticThumbinals(int thmbWidth, int thmbHeight)
{
   if (this->m_NumImages == 0)
	return NULL;
   if (this->m_NumXmlFiles == 0)
	return NULL;
   if (this->m_NumXmlItems == 0)
	return NULL;

   int imgCount =  floor(sqrt((float)this->m_NumXmlItems) + 0.5);
   int imgWidth  = thmbWidth*imgCount;
   int imgHeight = thmbHeight*imgCount;
   awpImage* result = NULL;
   awpCreateImage(&result, imgWidth, imgHeight, 3, AWP_BYTE);


   TSearchRec sr;
   AnsiString strPath = m_strDbName;

   strPath += "\\*.*";
   int iAttr =0;
   iAttr |= faAnyFile;
   int num = 0;
   awpRect rect;
   rect.left = 0;
   rect.top = 0;
   rect.right = thmbWidth;
   rect.bottom = thmbHeight;
   if (FindFirst(strPath, iAttr, sr) == 0)
   {
	  do
	  {

		 if (_IsImageFile(sr.Name))
		 {
			Form1->PhImage1->Bitmap->LoadFromFile(sr.Name);
			awpImage* img = NULL;
			Form1->PhImage1->GetAwpImage(&img);
			if (img)
			{
				awpImage* fit = NULL;
				if (FileExists(ChangeFileExt(sr.Name, ".xml")))
				{

					AnsiString strXmlName = ChangeFileExt(sr.Name, ".xml");
					TLFSemanticImageDescriptor descr;
					descr.LoadXML(strXmlName.c_str());
					for (int i = 0; i < descr.GetItemsCount(); i++)
					{
						TLFDetectedItem* item = descr.GetDetectedItem(i);
						if (item)
						{
							awpRect item_rect = item->GetBounds()->GetRect();
							awpImage* image1 = NULL;
							awpCopyRect(img, &image1, &item_rect);
							if (image1)
							{
								fit = FitImage(rect, image1);
								if (fit)
								{
									 awpPoint p;
									 p.Y = (num / imgCount)*thmbHeight;
									 p.X = (num % imgCount)*thmbWidth;

									 awpPasteRect(fit, result, p);

									_AWP_SAFE_RELEASE_(fit)
									 num++;
								}
								_AWP_SAFE_RELEASE_(image1)
							}


						}

					}

				}

				_AWP_SAFE_RELEASE_(img)
			}

		 }

		 if (m_ProgressEvent != NULL)
		 {
			AnsiString _ansi = "";
			m_ProgressEvent(int(100 *num / this->m_NumImages),_ansi );
		 }

		 Application->ProcessMessages();

	  }while(FindNext(sr) == 0);
	  FindClose(sr);
   }


   return result;
}
////////////////////////////////////////////////////////////////////////////////
void __fastcall TDbLabeledImages::DoMarking(ILFDetectEngine* engine)
{
   TSearchRec sr;
   AnsiString strPath = m_strDbName;

   strPath += "\\*.*";
   int iAttr =0;
   iAttr |= faAnyFile;
   int num = 0;
   awpRect rect;
   rect.left = 0;
   rect.top = 0;
   if (FindFirst(strPath, iAttr, sr) == 0)
   {
	  do
	  {
		 AnsiString str = sr.Name;
		 Application->ProcessMessages();

		 if (_IsImageFile(sr.Name))
		 {
			num++;
			TLFImage img;

			img.LoadFromFile(str.c_str());
			if (img.GetImage() != NULL)
			{
				engine->SetSourceImage(img.GetImage(), true);
				if (engine->GetItemsCount() > 0)
				{
				   TLFSemanticImageDescriptor* d = engine->GetSemantic();
				   AnsiString strXmlName = ChangeFileExt(sr.Name, ".xml");
				   d->SaveXML(strXmlName.c_str());
				}

				 if (m_ProgressEvent != NULL)
				 {
					AnsiString _ansi = sr.Name;
					m_ProgressEvent(int(100 *num / this->m_NumImages),_ansi );
				 }

			}
		}
	  }
	  while(FindNext(sr) == 0);
	  FindClose(sr);
   }
}

TLFSemanticDictinary* __fastcall TDbLabeledImages::GetDictionary()
{
	return m_db.GetDictinary();
}

void __fastcall TDbLabeledImages::SaveBackground(SDbExportOptions& options)
{
    //
    if (options.ClassLabels == NULL)
    	return;
    for(int i = 0; i < options.ClassLabels->Count; i++)
    {
		AnsiString strPath = options.strPathToExport;
        strPath += "\\";
        strPath += options.ClassLabels->Strings[i];
        strPath += "\\";
        strPath += "bkg\\";
	    // clear old background
        if (DirectoryExists(strPath))
			ClearDir(strPath);
        else
			CreateDir(strPath);

        // copty data to this path
       TSearchRec sr;
       AnsiString strSourcePath = m_strDbName;
       strSourcePath += "\\*.xml";

       int iAttr =0;
	   iAttr |= faAnyFile;
       int num = 0;
       int count = 0;
       AnsiString strClassLabel = options.ClassLabels->Strings[i];
        if (FindFirst(strSourcePath, iAttr, sr) == 0)
        {
          do
          {
             TLFSemanticImageDescriptor sd;
             num++;
             awpImage* img = NULL;
             bool exist = false;
             AnsiString strTmpFile = ChangeFileExt(sr.Name, ".jpg");
             AnsiString strFullName = m_strDbName;
             strFullName += "\\";
             if (FileExists(strTmpFile))
             {
               exist = true;
               strFullName += strTmpFile;
               awpLoadImage(strFullName.c_str(), &img);
             }
			 else
             {
                strTmpFile = ChangeFileExt(sr.Name, ".jpeg");
                if (FileExists(strTmpFile))
                {
                    strFullName += strTmpFile;
                    awpLoadImage(strFullName.c_str(), &img);
                    exist = true;
                }
                else
				{
                  strTmpFile = ChangeFileExt(sr.Name, ".awp");
                  if (FileExists(strTmpFile))
                  {
                      strFullName += strTmpFile;
                      awpLoadImage(strFullName.c_str(), &img);
                      exist = true;
                  }
                }
             }
             if (!exist)
                continue;

             AnsiString _ansi = sr.Name;
             sd.LoadXML(_ansi.c_str());
             sd.SetImage(img);

             for (int i = sd.GetItemsCount() - 1; i >= 0 ; i--)
             {
                TLFDetectedItem*  di = sd.GetDetectedItem(i);
                if (di == NULL)
					continue;

                UnicodeString strClassLabel0 = di->GetType().c_str();
                if (options.ClassLabels->IndexOf(strClassLabel0) < 0)
                    continue;
                AnsiString _ansi = strClassLabel0;
                if (_ansi != strClassLabel)
                {
					sd.Delete(i);
                }
			 }

             AnsiString strOutFileName = strPath;
             strOutFileName += ExtractFileName(strTmpFile);
             awpSaveImage(strOutFileName.c_str(), img);
             strOutFileName = ChangeFileExt(strOutFileName, ".xml");
             sd.SaveXML(strOutFileName.c_str());

             awpReleaseImage(&img);
             if (m_ProgressEvent != NULL)
             {
                AnsiString strComment = "Processed ";
                strComment += IntToStr (num);
                strComment += " of ";
                strComment += IntToStr(m_NumXmlFiles);
                strComment += " for class label ";
                strComment +=  strClassLabel;
                m_ProgressEvent(int(100 *num / m_NumXmlFiles),strComment);
             }

             Application->ProcessMessages();

          }while(FindNext(sr) == 0);
          FindClose(sr);
        }
    }
}


void __fastcall TDbLabeledImages::SplitDatabase(UnicodeString strPath, UnicodeString strPath1, UnicodeString strpath2, double propValue )
{
	srand ( time(NULL) );
    AnsiString str = strPath.c_str();
	string path 	     =  str.c_str();
	path += "\\";
    str  = strPath1.c_str();
	string g_path1       =  str.c_str();
    str  = strpath2.c_str();
	string g_path2       =  str.c_str();;
	int g_proc = propValue;

	if (!LFDirExist(g_path1.c_str()))
		LFCreateDir(g_path1.c_str());
	if (!LFDirExist(g_path2.c_str()))
		LFCreateDir(g_path2.c_str());

	AnsiString _ansi = g_path1.c_str();
	ClearDir(_ansi);
	_ansi = g_path2.c_str();
	ClearDir(_ansi);

	// copy dictionary
	string str_dict_name = path;
	str_dict_name += "\\dictionary.xml";
	if (LFFileExists(str_dict_name))
	{
		string str_dst_dict_name = g_path1;
		str_dst_dict_name += "\\dictionary.xml";
		UnicodeString _unic1 = str_dict_name.c_str();
		UnicodeString _unic2 = str_dst_dict_name.c_str();
		CopyFile(_unic1.c_str(), _unic2.c_str(), true);
		str_dst_dict_name = g_path2;
		str_dst_dict_name += "\\dictionary.xml";
		_unic2 = str_dst_dict_name.c_str();
		CopyFile(_unic1.c_str(), _unic2.c_str(), true);
	}

	_finddata_t filesInfo;
	int num_images = 0;
	long handle = 0;

	if ( (handle = _findfirst( (char*)((path+"*.*").c_str()), &filesInfo)) != -1 )
	{
		do
		{
			string name = path + filesInfo.name;
			TLFImage image;
			UnicodeString _unicode = name.c_str();
			if (!_IsImageFile(_unicode))
				continue;

			if (!image.LoadFromFile(name.c_str()))
			{
				printf("cannot load image %s \n", name.c_str());
				continue;
			}
			num_images++;
			std::string FileName;

			int value = rand() % 100;
			if (value < g_proc)
				FileName = g_path1;
			else
				FileName = g_path2;

			FileName += LFGetFileName(name);
			FileName += ".awp";
			printf("%s\n", FileName.c_str());
			image.SaveToFile(FileName.c_str());

			name = LFChangeFileExt(name, ".xml");
			if (LFFileExists(name))
			{
				TLFSemanticImageDescriptor d;
				d.LoadXML(name.c_str());
				FileName = LFChangeFileExt(FileName, ".xml");
				d.SaveXML(FileName.c_str());
			}

			  if (m_ProgressEvent != NULL)
			  {
				AnsiString _ansi = FileName.c_str();
				m_ProgressEvent(int(100 *num_images / this->m_NumImages),_ansi );
			  }
			   Application->ProcessMessages();

		}while(!_findnext( handle, &filesInfo ));
	}
	_findclose( handle );
}

TLFDBLabeledImages*   __fastcall TDbLabeledImages::GetDatabase()
{
    return &m_db;
}

int __fastcall TDbLabeledImages::GetNumLabels()
{
    return Dictionary->GetCount();
}
// deletes all the markup data from the database and clears the dictionary
void __fastcall TDbLabeledImages::ClearDatabase()
{
    m_db.SetProgress(_progress);
    m_db.ClearDatabase();
    m_db.SetProgress(NULL);
}

void __fastcall TDbLabeledImages::UpdateDatabase()
{
    m_db.SetProgress(_progress);
    m_db.UpdateDatabase();
    m_db.SetProgress(NULL);
}


